#ifndef CRAFTING_HPP
#define CRAFTING_HPP

#include "hashmap.hpp"
#include "item.hpp"
#include <utility>

inline bool canCraftItem(const std::string& id,
                          const HashMap<std::string, int>& inventory,
                          const HashMap<std::string, Recipe>& recipes,
                          HashMap<std::string, int>& used)
{
  int* count = inventory.find(id);
  if (count && *count > 0)
  {
    int* u = used.find(id);
    if (u)
      *u += 1;
    else
      used.insert(id, 1);
    return true;
  }

  Recipe* rec = recipes.find(id);
  if (!rec)
    return false;

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
        if (*v <= 0)
          inv.remove(key);
      }
    }

    node = node->next_;
  }

  remainingInventory = inv;
  return true;
}

#endif
