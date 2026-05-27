#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <sstream>
#include "hashmap.hpp"
#include "item.hpp"
#include "list.hpp"
#include "crafting.hpp"

inline void listItems(std::ostream& out,
                      const HashMap<std::string, Item>& items)
{
  out << "(" << items.size() << " items)\n";
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    out << it->value.name << '\n';
  }
}

inline void showRecipe(std::ostream& out,
  const std::string& id,
  const HashMap<std::string, Item>& items,
  const HashMap<std::string, Recipe>& recipes)
{
  Item* item = items.find(id);
  if (!item)
  {
    out << "Item not found.\n";
    return;
  }
  Recipe* rec = recipes.find(id);
  if (!rec)
  {
    out << "No crafting recipe for " << item->name << ".\n";
    return;
  }
  out << "Recipe for " << item->name << ":\n";
  out << "[Crafting grid 3x3]\n";
  for (const Ingredient& ing : rec->ingredients)
  {
    out << ing.count << "x " << ing.item_id << "\n";
  }
}

inline void showUses(std::ostream& out,
  const std::string& id,
  const HashMap<std::string, Item>& items,
  const HashMap<std::string, List<std::string>>& useMap)
{
  Item* item = items.find(id);
  if (!item)
  {
    out << "Item not found.\n";
    return;
  }
  List<std::string>* lst = useMap.find(id);
  if (!lst || lst->empty())
  {
    out << "No recipes use " << item->name << " as an ingredient.\n";
    return;
  }
  out << "Uses for " << item->name << ":\n";
  auto* node = lst->head();
  while (node)
  {
    out << "- " << node->data << '\n';
    node = node->next_;
  }
}

inline void search(std::ostream& out,
  const std::string& prefix,
  const HashMap<std::string, Item>& items)
{
  out << "Items starting with '" << prefix << "':\n";
  bool found = false;
  std::string lower_prefix = prefix;
  for (char& c : lower_prefix)
  {
    c = static_cast<char>(std::tolower(c));
  }
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    std::string name_lower = it->value.name;
    for (char& c : name_lower)
    {
      c = static_cast<char>(std::tolower(c));
    }
    if (name_lower.compare(0, lower_prefix.size(), lower_prefix) == 0)
    {
      out << it->value.name << '\n';
      found = true;
    }
  }
  if (!found)
  {
    out << "No items starting with '" << prefix << "' found.\n";
  }
}

inline void itemInfo(std::ostream& out,
                     const std::string& id,
                     const HashMap<std::string, Item>& items)
{
  Item* item = items.find(id);
  if (!item)
  {
    out << "Item not found.\n";
    return;
  }
  out << "Item: " << item->name << '\n';
  std::string typeStr;
  switch (item->type)
  {
    case ItemType::Material: typeStr = "Material"; break;
    case ItemType::Weapon:   typeStr = "Weapon"; break;
    case ItemType::Armor:    typeStr = "Armor"; break;
    case ItemType::Food:     typeStr = "Food"; break;
    case ItemType::Other:    typeStr = "Other"; break;
  }
  out << "Type: " << typeStr << '\n';
  out << "Durability: ";
  if (item->durability >= 0)
    out << item->durability << '\n';
  else
    out << "N/A\n";
  out << "Max stack size: " << item->maxStack << '\n';
  out << "Category: ";
  if (item->type == ItemType::Weapon || item->type == ItemType::Armor)
    out << "Combat\n";
  else
    out << "Other\n";
}
inline void bestPvpPack(std::ostream& out,
                        const std::string& args,
                        const HashMap<std::string, Item>& items,
                        const HashMap<std::string, Recipe>& recipes)
{
  std::istringstream iss(args);
  HashMap<std::string, int> inventory;
  std::string token;
  while (iss >> token)
  {
    int* c = inventory.find(token);
    if (c) *c += 1;
    else inventory.insert(token, 1);
  }

  HashMap<int, List<Item*>> slotCandidates;
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

  struct Best
  {
    double power = -1.0;
    int usedResources = 0;
    List<const Item*> items;
    HashMap<std::string, int> remaining;
  } best;

  std::vector<int> slots;
  for (auto it = slotCandidates.begin(); it != slotCandidates.end(); ++it)
  {
    slots.push_back(it->key);
  }

  std::function<void(size_t, List<const Item*>, HashMap<std::string, int>)> dfs;
  dfs = [&](size_t idx, List<const Item*> currentSet, HashMap<std::string, int> currentInv)
  {
    if (idx == slots.size())
    {
      double totalPower = 0.0;
      for (auto* node = currentSet.head(); node; node = node->next_)
      {
        totalPower += node->data->power;
      }
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
      {
        setIds.pushFront(n->data->id);
      }
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

  if (best.power < 0)
  {
    out << "No PvP items can be crafted from the given resources.\n";
    return;
  }

  out << "Optimal PvP pack from given resources:\n";
  for (auto* node = best.items.head(); node; node = node->next_)
  {
    const Item* item = node->data;
    out << "- " << item->name << " [" << (item->slot == SlotType::Weapon ? "Weapon" :
        item->slot == SlotType::Head ? "Head" :
        item->slot == SlotType::Chest ? "Chest" :
        item->slot == SlotType::Legs ? "Legs" :
        item->slot == SlotType::Feet ? "Feet" :
        item->slot == SlotType::Offhand ? "Offhand" : "None") << "]\n";
  }
  out << "Resources used: ";
  bool first = true;
  for (auto it = inventory.begin(); it != inventory.end(); ++it)
  {
    int* rem = best.remaining.find(it->key);
    int remain = rem ? *rem : 0;
    if (it->value > remain)
    {
      if (!first) out << ", ";
      out << (it->value - remain) << "x " << it->key;
      first = false;
    }
  }
  out << '\n';
  out << "Remaining resources: ";
  first = true;
  for (auto it = best.remaining.begin(); it != best.remaining.end(); ++it)
  {
    if (!first) out << ", ";
    out << it->value << "x " << it->key;
    first = false;
  }
  out << '\n';
  out << "Combat efficiency score: " << best.power << '\n';
}
#endif
