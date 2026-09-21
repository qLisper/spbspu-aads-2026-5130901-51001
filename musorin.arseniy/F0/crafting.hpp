#ifndef CRAFTING_HPP
#define CRAFTING_HPP

#include "hashmap.hpp"
#include "item.hpp"

inline bool canCraftItemImpl(const std::string& id,
                             HashMap<std::string, int>& inv,
                             const HashMap<std::string, Recipe>& recipes,
                             HashMap<std::string, int>& used)
{

  int* count = inv.find(id);
  if (count && *count > 0)
  {
    --(*count);
    if (*count == 0)
      inv.remove(id);

    int* u = used.find(id);
    if (u) *u += 1;
    else   used.insert(id, 1);
    return true;
  }

  const Recipe* rec = recipes.find(id);
  if (!rec)
    return false;

  HashMap<std::string, int> invBackup  = inv;
  HashMap<std::string, int> usedBackup = used;

  for (const Ingredient& ing : rec->ingredients)
  {
    for (int i = 0; i < ing.count; ++i)
    {
      if (!canCraftItemImpl(ing.item_id, inv, recipes, used))
      {
        inv  = invBackup;
        used = usedBackup;
        return false;
      }
    }
  }
  return true;
}

inline bool canCraftItem(const std::string& id,
                          const HashMap<std::string, int>& inv,
                          const HashMap<std::string, Recipe>& recipes,
                          HashMap<std::string, int>& used)
{
  HashMap<std::string, int> localInv = inv;
  HashMap<std::string, int> localUsed;

  if (!canCraftItemImpl(id, localInv, recipes, localUsed))
    return false;

  used = localUsed;
  return true;
}

#endif
