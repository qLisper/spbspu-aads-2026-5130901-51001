#ifndef CRAFTING_HPP
#define CRAFTING_HPP

#include "hashmap.hpp"
#include "item.hpp"
#include <vector>
#include <functional>

struct OptimalPack
{
  double power = -1.0;
  int usedResources = 0;
  List<const Item*> items;
  HashMap<std::string, int> remaining;
};

inline void buildSlotCandidates(const HashMap<std::string, Item>& items,
                                HashMap<int, List<Item*>>& slotCandidates)
{
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    Item* item = &it->value;
    if (item->slot != SlotType::None)
    {
      int slot = static_cast<int>(item->slot);
      List<Item*>* lst = slotCandidates.find(slot);
      if (!lst)
      {
        List<Item*> empty;
        slotCandidates.insert(slot, empty);
        lst = slotCandidates.find(slot);
      }
      lst->pushFront(item);
    }
  }
}

inline OptimalPack computeOptimalPack(const HashMap<std::string, int>& inventory,
                                      const HashMap<std::string, Item>& items,
                                      const HashMap<std::string, Recipe>& recipes)
{
  OptimalPack best;

  HashMap<int, List<Item*>> slotCandidates;
  buildSlotCandidates(items, slotCandidates);

  std::vector<int> slots;
  for (auto it = slotCandidates.begin(); it != slotCandidates.end(); ++it)
    slots.push_back(it->key);

  std::function<void(size_t, List<const Item*>, HashMap<std::string, int>)> dfs;
  dfs = [&](size_t idx, List<const Item*> currentSet, HashMap<std::string, int> currentInv)
  {
    if (idx == slots.size())
    {
      double totalPower = 0.0;
      for (auto* node = currentSet.head(); node; node = node->next_)
        totalPower += node->data->power;

      int spent = 0;
      for (auto it = inventory.begin(); it != inventory.end(); ++it)
      {
        int* v = currentInv.find(it->key);
        int have = v ? *v : 0;
        spent += (it->value - have);
      }

      if (totalPower > best.power || (totalPower == best.power && spent < best.usedResources))
      {
        best.power = totalPower;
        best.usedResources = spent;
        best.items = currentSet;
        best.remaining = currentInv;
      }
      return;
    }

    int slot = slots[idx];
    List<Item*>* cand = slotCandidates.find(slot);
    if (!cand)
    {
      dfs(idx + 1, currentSet, currentInv);
      return;
    }

    dfs(idx + 1, currentSet, currentInv);

      for (auto* node = cand->head(); node; node = node->next_)
    {
      Item* item = node->data;
      List<std::string> setIds;
      for (auto* n = currentSet.head(); n; n = n->next_)
        setIds.pushFront(n->data->id);
      setIds.pushFront(item->id);

      HashMap<std::string, int> remaining;
      if (canCraftSet(setIds, inventory, recipes, remaining))
      {
        List<const Item*> newSet = currentSet;
        newSet.pushFront(item);
        dfs(idx + 1, newSet, remaining);
      }
    }
  };

  List<const Item*> emptySet;
  dfs(0, emptySet, inventory);

  return best;
}

inline bool canCraftItem(const std::string& id,
                          const HashMap<std::string, int>& inventory,
                          const HashMap<std::string, Recipe>& recipes,
                          HashMap<std::string, int>& used)
{
  int* count = inventory.find(id);
  if (count && *count > 0)
  {
    int* u = used.find(id);
    if (u) *u += 1;
    else used.insert(id, 1);
    return true;
  }

  Recipe* rec = recipes.find(id);
  if (!rec) return false;

  HashMap<std::string, int> backup = used;
  for (const Ingredient& ing : rec->ingredients)
  {
    for (int i = 0; i < ing.count; ++i)
    {
      if (!canCraftItem(ing.item_id, inventory, recipes, used))
      {
        used = backup;
        return false;
      }
    }
  }
  return true;
}

inline bool canCraftSet(const List<std::string>& ids,
                         const HashMap<std::string, int>& inventory,
                         const HashMap<std::string, Recipe>& recipes,
                         HashMap<std::string, int>& remainingInventory)
{
  HashMap<std::string, int> inv = inventory;
  auto* node = ids.head();
  while (node)
  {
    const std::string& id = node->data;
    HashMap<std::string, int> used;
    if (!canCraftItem(id, inv, recipes, used))
      return false;
    for (auto it = used.begin(); it != used.end(); ++it)
    {
      std::string key = it->key;
      int spent = it->value;
      int* v = inv.find(key);
      if (v)
      {
        *v -= spent;
        if (*v <= 0) inv.remove(key);
      }
    }
    node = node->next_;
  }
  remainingInventory = inv;
  return true;
}

#endif
