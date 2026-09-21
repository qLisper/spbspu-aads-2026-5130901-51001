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

inline void gatherRaw(const std::string& id,
                      HashMap<std::string, int>& need,
                      const HashMap<std::string, Recipe>& recipes)
{
  const Recipe* rec = recipes.find(id);
  if (!rec)
  {
    int* v = need.find(id);
    if (v) *v += 1;
    else   need.insert(id, 1);
    return;
  }
  for (const Ingredient& ing : rec->ingredients)
  {
    for (int i = 0; i < ing.count; ++i)
      gatherRaw(ing.item_id, need, recipes);
  }
}

inline bool isSwordId(const std::string& id)
{
  static const std::string suf = "_sword";
  return id.size() >= suf.size()
      && id.compare(id.size() - suf.size(), suf.size(), suf) == 0;
}

struct UpgradeChoice
{
  bool   have         = false;
  const Item* item    = nullptr;
  int    deficitTotal = 0;
  HashMap<std::string, int> deficit;
};

inline UpgradeChoice pickUpgrade(const HashMap<std::string, int>& remaining,
                                 const HashMap<std::string, Item>& items,
                                 const HashMap<std::string, Recipe>& recipes,
                                 SlotType weakestSlot,
                                 double   weakestPower,
                                 bool     requireUsesLeftover)
{
  UpgradeChoice best;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    const Item& cand = it->value;
    if (cand.slot != weakestSlot) continue;
    if (cand.power <= weakestPower) continue;

    if (weakestSlot == SlotType::Weapon && !isSwordId(cand.id)) continue;

    HashMap<std::string, int> need;
    gatherRaw(cand.id, need, recipes);

    if (requireUsesLeftover)
    {
      bool uses = false;
      for (auto nit = need.begin(); nit != need.end(); ++nit)
      {
        const int* have = remaining.find(nit->key);
        if (have && *have > 0) { uses = true; break; }
      }
      if (!uses) continue;
    }

    HashMap<std::string, int> deficit;
    int deficitTotal = 0;
    for (auto nit = need.begin(); nit != need.end(); ++nit)
    {
      int have = 0;
      const int* v = remaining.find(nit->key);
      if (v) have = *v;
      int d = nit->value - have;
      if (d > 0) { deficit.insert(nit->key, d); deficitTotal += d; }
    }

    bool better = false;
    if (!best.have) better = true;
    else if (deficitTotal < best.deficitTotal) better = true;
    else if (deficitTotal == best.deficitTotal)
    {
      if (cand.power < best.item->power) better = true;
      else if (cand.power == best.item->power)
      {
        if (cand.durability > best.item->durability) better = true;
        else if (cand.durability == best.item->durability
                 && cand.id < best.item->id) better = true;
      }
    }

    if (better)
    {
      best.have         = true;
      best.item         = &cand;
      best.deficitTotal = deficitTotal;
      best.deficit      = deficit;
    }
  }
  return best;
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

  HashMap<int, const Item*> slotItem;
  for (const std::string& id : pack)
  {
    const Item* it = items.find(id);
    if (it) slotItem.insert(static_cast<int>(it->slot), it);
  }


  SlotType    weakestSlot  = SlotType::None;
  double      weakestPower = 1e18;
  const Item* weakestItem  = nullptr;

  for (SlotType s : SLOT_PRIORITY)
  {
    const Item* const* exPtr = slotItem.find(static_cast<int>(s));
    const Item* cur = exPtr ? *exPtr : nullptr;
    double p = cur ? cur->power : 0.0;
    if (p < weakestPower)
    {
      weakestPower = p;
      weakestItem  = cur;
      weakestSlot  = s;
    }
  }

  if (weakestSlot == SlotType::None)
  {
    out << "No PvP slots to upgrade.\n";
    return;
  }

  UpgradeChoice choice =
      pickUpgrade(remaining, items, recipes, weakestSlot, weakestPower,
                  /*requireUsesLeftover=*/true);

  if (!choice.have)
  {
    choice = pickUpgrade(remaining, items, recipes, weakestSlot,
                         weakestPower, /*requireUsesLeftover=*/false);
  }

  if (!choice.have)
  {
    if (weakestItem)
      out << "Weakest slot (" << weakestItem->name
          << ") is already at maximum level.\n";
    else
      out << "No further upgrades available for the weakest slot.\n";
    return;
  }

  out << "To achieve better PvP pack, add:\n";
  for (auto it = choice.deficit.begin(); it != choice.deficit.end(); ++it)
  {
    const Item* ri = items.find(it->key);
    out << it->value << "x " << (ri ? ri->name : it->key) << "\n";
  }
  out << "\nWith these additions, you can craft:\n";
  out << choice.item->name << "\n\n";

  double newScore = currentScore - weakestPower + choice.item->power;
  out << "Combat efficiency would increase from "
      << currentScore << " to " << newScore << ".\n";
}

#endif
