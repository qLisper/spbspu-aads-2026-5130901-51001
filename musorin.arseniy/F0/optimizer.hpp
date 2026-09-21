#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <ostream>
#include "hashmap.hpp"
#include "item.hpp"
#include "crafting.hpp"
#include "list.hpp"

static const SlotType SLOT_PRIORITY[] = {
  SlotType::Weapon,
  SlotType::Chest,
  SlotType::Legs,
  SlotType::Head,
  SlotType::Feet,
  SlotType::Offhand
};

struct PvpSlot
{
  SlotType slot;
  std::vector<const Item*> items; 
};

inline std::vector<PvpSlot> buildSlotCandidates(
  const HashMap<std::string, Item>& items)
{
  std::vector<PvpSlot> result;
  for (SlotType s : SLOT_PRIORITY)
  {
    PvpSlot entry;
    entry.slot = s;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
      const Item& item = it->value;
      if (item.slot != s) continue;
      if (item.power <= 0.0) continue;
      entry.items.push_back(&item);
    }
    std::sort(entry.items.begin(), entry.items.end(),
      [](const Item* a, const Item* b) { return a->power > b->power; });
    result.push_back(entry);
  }
  return result;
}

inline double calcEfficiency(const std::vector<std::string>& pack,
                             const HashMap<std::string, Item>& items)
{
  double score = 0.0;
  for (const std::string& id : pack)
  {
    const Item* item = items.find(id);
    if (item) score += item->power;
  }
  return score;
}

struct PvpSearchState
{
  bool   found     = false;
  double bestScore = -1.0;
  int    bestCost  = 0;
  std::vector<std::string>  bestPack;
  HashMap<std::string, int> bestRemaining;
};

inline void pvpDfs(int slotIdx,
                   const std::vector<PvpSlot>& slots,
                   HashMap<std::string, int>& inv,
                   std::vector<const Item*>& chosen,
                   double score,
                   int cost,
                   const HashMap<std::string, Recipe>& recipes,
                   PvpSearchState& state)
{
  if (slotIdx == static_cast<int>(slots.size()))
  {
    if (chosen.empty()) return;

    bool better = false;
    if      (!state.found)                                       better = true;
    else if (score >  state.bestScore)                           better = true;
    else if (score == state.bestScore && cost < state.bestCost)  better = true;

    if (better)
    {
      state.found = true;
      state.bestScore = score;
      state.bestCost  = cost;
      state.bestPack.clear();
      for (const Item* item : chosen)
        state.bestPack.push_back(item->id);
      state.bestRemaining = inv;
    }
    return;
  }

  pvpDfs(slotIdx + 1, slots, inv, chosen, score, cost, recipes, state);

  for (const Item* item : slots[slotIdx].items)
  {
    HashMap<std::string, int> used;
    if (!canCraftItem(item->id, inv, recipes, used)) continue;

    HashMap<std::string, int> invBackup = inv;
    int addCost = 0;
    bool ok = true;
    for (auto it = used.begin(); it != used.end(); ++it)
    {
      const int* v = inv.find(it->key);
      if (!v || *v < it->value) { ok = false; break; }
    }
    if (!ok) { inv = invBackup; continue; }

    for (auto it = used.begin(); it != used.end(); ++it)
    {
      int* v = inv.find(it->key);
      *v -= it->value;
      if (*v <= 0) inv.remove(it->key);
      addCost += it->value;
    }

    chosen.push_back(item);
    pvpDfs(slotIdx + 1, slots, inv, chosen,
           score + item->power, cost + addCost, recipes, state);
    chosen.pop_back();
    inv = invBackup;
  }
}

inline bool bestPvpPack(const HashMap<std::string, int>& inventory,
                        const HashMap<std::string, Item>& items,
                        const HashMap<std::string, Recipe>& recipes,
                        std::vector<std::string>& outPack,
                        HashMap<std::string, int>& outRemaining,
                        double& outScore)
{
  std::vector<PvpSlot> slots = buildSlotCandidates(items);

  PvpSearchState state;
  HashMap<std::string, int> inv = inventory;
  std::vector<const Item*> chosen;

  pvpDfs(0, slots, inv, chosen, 0.0, 0, recipes, state);

  if (!state.found)
    return false;

  outPack      = state.bestPack;
  outRemaining = state.bestRemaining;
  outScore     = state.bestScore;
  return true;
}

inline void whatToAdd(const HashMap<std::string, int>& inventory,
                      const HashMap<std::string, Item>& items,
                      const HashMap<std::string, Recipe>& recipes,
                      std::ostream& out)
{
  std::vector<std::string> pack;
  HashMap<std::string, int> remaining;
  double currentScore = 0.0;
  if (!bestPvpPack(inventory, items, recipes, pack, remaining, currentScore))
  {
    out << "No PvP items can be crafted from the given resources.\n";
    return;
  }

  bool upgraded = false;
  for (const std::string& id : pack)
  {
    const Item* cur = items.find(id);
    if (!cur) continue;
    std::string bestUpgradeId;
    double bestUpgradePower = cur->power;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
      const Item& cand = it->value;
      if (cand.slot != cur->slot) continue;
      if (cand.power <= bestUpgradePower) continue;
      HashMap<std::string, int> used;
      if (canCraftItem(cand.id, remaining, recipes, used))
      {
        bestUpgradeId    = cand.id;
        bestUpgradePower = cand.power;
      }
    }
    if (!bestUpgradeId.empty())
    {
      const Item* upItem = items.find(bestUpgradeId);
      out << "With remaining resources you can upgrade "
          << cur->name << " -> " << upItem->name << "\n";
      upgraded = true;
    }
  }
  if (upgraded) return;

  std::string weakestId;
  double weakestPower = 1e18;
  for (const std::string& id : pack)
  {
    const Item* item = items.find(id);
    if (item && item->power < weakestPower)
    {
      weakestPower = item->power;
      weakestId    = id;
    }
  }
  if (weakestId.empty()) return;
  const Item* weakest = items.find(weakestId);
  std::string nextId;
  double nextPower = 1e18;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    const Item& cand = it->value;
    if (cand.slot != weakest->slot) continue;
    if (cand.power <= weakest->power) continue;
    if (cand.power < nextPower)
    {
      nextPower = cand.power;
      nextId    = cand.id;
    }
  }
  if (nextId.empty())
  {
    out << "Current pack is already at maximum level.\n";
    return;
  }
  const Item* nextItem = items.find(nextId);
  const Recipe* rec    = recipes.find(nextId);
  out << "To upgrade " << weakest->name << " to " << nextItem->name << ", add:\n";
  if (rec)
  {
    for (const Ingredient& ing : rec->ingredients)
    {
      int have = 0;
      const int* v = inventory.find(ing.item_id);
      if (v) have = *v;
      int need = ing.count - have;
      const Item* ingItem = items.find(ing.item_id);
      std::string ingName = ingItem ? ingItem->name : ing.item_id;
      if (need > 0)
        out << "- " << ingName << " x" << need << "\n";
    }
  }
  out << "Efficiency would increase from " << currentScore
      << " to " << (currentScore - weakest->power + nextPower) << "\n";
}

#endif
