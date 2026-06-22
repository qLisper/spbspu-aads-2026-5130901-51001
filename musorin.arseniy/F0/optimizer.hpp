#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP
#include "hashmap.hpp"
#include "item.hpp"
#include "crafting.hpp"
#include "list.hpp"
#include <string>
#include <vector>
#include <algorithm>
static const SlotType SLOT_PRIORITY[] = {
  SlotType::Weapon,
  SlotType::Chest,
  SlotType::Legs,
  SlotType::Head,
  SlotType::Feet,
  SlotType::Offhand
};
struct PvpCandidate
{
  std::string id;
  SlotType    slot;
  double      power;
};
inline std::vector<PvpCandidate> buildCandidates(
  const HashMap<std::string, Item>& items)
{
  std::vector<PvpCandidate> cands;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    const Item& item = it->value;
    if (item.type == ItemType::Weapon ||
        item.type == ItemType::Armor  ||
        (item.type == ItemType::Other && item.slot == SlotType::Offhand))
    {
      cands.push_back({item.id, item.slot, item.power});
    }
  }
  return cands;
}

inline double calcEfficiency(const std::vector<std::string>& pack,
                             const HashMap<std::string, Item>& items)
{
  double score = 0.0;
  for (const std::string& id : pack)
  {
    Item* item = items.find(id);
    if (item) score += item->power;
  }
  return score;
}

inline bool tryBuild(const std::string& id,
                     HashMap<std::string, int>& inv,
                     const HashMap<std::string, Recipe>& recipes)
{
  HashMap<std::string, int> used;
  if (!canCraftItem(id, inv, recipes, used))
    return false;
  for (auto it = used.begin(); it != used.end(); ++it)
  {
    int* v = inv.find(it->key);
    if (v)
    {
      *v -= it->value;
      if (*v <= 0)
        inv.remove(it->key);
    }
  }
  return true;
}

inline bool bestPvpPack(const HashMap<std::string, int>& inventory,
                        const HashMap<std::string, Item>& items,
                        const HashMap<std::string, Recipe>& recipes,
                        std::vector<std::string>& outPack,
                        HashMap<std::string, int>& outRemaining,
                        double& outScore)
{
  std::vector<PvpCandidate> cands = buildCandidates(items);
  auto slotOrder = [](SlotType s) -> int {
    for (int i = 0; i < 6; ++i)
      if (SLOT_PRIORITY[i] == s) return i;
    return 99;
  };
  std::sort(cands.begin(), cands.end(),
    [&slotOrder](const PvpCandidate& a, const PvpCandidate& b) {
      int oa = slotOrder(a.slot), ob = slotOrder(b.slot);
      if (oa != ob) return oa < ob;
      return a.power > b.power;
    });
  HashMap<std::string, int> inv = inventory;
  std::vector<std::string> pack;
  SlotType lastSlot = SlotType::None;
  for (const PvpCandidate& cand : cands)
  {
    if (cand.slot != SlotType::None && cand.slot == lastSlot)
      continue;
    if (tryBuild(cand.id, inv, recipes))
    {
      pack.push_back(cand.id);
      if (cand.slot != SlotType::None)
        lastSlot = cand.slot;
    }
  }
  if (pack.empty())
    return false;
  outPack      = pack;
  outRemaining = inv;
  outScore     = calcEfficiency(pack, items);
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
    Item* cur = items.find(id);
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
      Item* upItem = items.find(bestUpgradeId);
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
    Item* item = items.find(id);
    if (item && item->power < weakestPower)
    {
      weakestPower = item->power;
      weakestId    = id;
    }
  }
  if (weakestId.empty()) return;
  Item* weakest = items.find(weakestId);
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
  Item* nextItem = items.find(nextId);
  Recipe* rec    = recipes.find(nextId);
  out << "To upgrade " << weakest->name << " to " << nextItem->name << ", add:\n";
  if (rec)
  {
    for (const Ingredient& ing : rec->ingredients)
    {
      int have = 0;
      int* v = inventory.find(ing.item_id);
      if (v) have = *v;
      int need = ing.count - have;
      Item* ingItem = items.find(ing.item_id);
      std::string ingName = ingItem ? ingItem->name : ing.item_id;
      if (need > 0)
        out << "- " << ingName << " x" << need << "\n";
    }
  }
  out << "Efficiency would increase from " << currentScore
      << " to " << (currentScore - weakest->power + nextPower) << "\n";
}

#endif
