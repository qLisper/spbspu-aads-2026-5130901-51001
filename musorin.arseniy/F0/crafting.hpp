#ifndef CRAFTING_HPP
#define CRAFTING_HPP

#include "hashmap.hpp"
#include "item.hpp"

inline bool canCraft(const std::string& id,
                     HashMap<std::string, int>& inventory,
                     const HashMap<std::string, Recipe>& recipes)
{

  int* count = inventory.find(id);
  if (count && *count > 0)
  {
    --(*count);
    return true;
  }


  Recipe* rec = recipes.find(id);
  if (!rec)
  {
    return false;
  }


  HashMap<std::string, int> backup = inventory;

  for (const Ingredient& ing : rec->ingredients)
  {
    for (int i = 0; i < ing.count; ++i)
    {
      if (!canCraft(ing.item_id, inventory, recipes))
      {
        inventory = backup;
        return false;
      }
    }
  }
  return true;
}

#endif
