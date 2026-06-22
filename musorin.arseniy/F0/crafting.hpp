#ifndef CRAFTING_HPP
#define CRAFTING_HPP
#include "hashmap.hpp"
#include "item.hpp"
inline bool canCraftItem(const std::string& id,
                          const HashMap<std::string, int>& inv,
                          const HashMap<std::string, Recipe>& recipes,
                          HashMap<std::string, int>& used)
{
  int* count = inv.find(id);
  if (count && *count > 0)
  {
    int* u = used.find(id);
    if (u) *u += 1;
    else   used.insert(id, 1);
    return true;
  }
  Recipe* rec = recipes.find(id);
  if (!rec) return false;
  HashMap<std::string, int> backup = used;
  for (const Ingredient& ing : rec->ingredients)
  {
    for (int i = 0; i < ing.count; ++i)
    {
      if (!canCraftItem(ing.item_id, inv, recipes, used))
      {
        used = backup;
        return false;
      }
    }
  }
  return true;
}
#endif
