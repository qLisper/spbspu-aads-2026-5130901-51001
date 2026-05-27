#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include "hashmap.hpp"
#include "item.hpp"
#include "list.hpp"
#include "crafting.hpp"

inline void listItems(std::ostream& out, const HashMap<std::string, Item>& items)
{
  out << "(" << items.size() << " items)\n";
  for (auto it = items.begin(); it != items.end(); ++it)
    out << it->value.name << '\n';
}

inline void showRecipe(std::ostream& out, const std::string& id,
                       const HashMap<std::string, Item>& items,
                       const HashMap<std::string, Recipe>& recipes)
{
  Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }
  Recipe* rec = recipes.find(id);
  if (!rec) { out << "No crafting recipe for " << item->name << ".\n"; return; }
  out << "Recipe for " << item->name << ":\n[Crafting grid 3x3]\n";
  for (const Ingredient& ing : rec->ingredients)
    out << ing.count << "x " << ing.item_id << "\n";
}

inline void showUses(std::ostream& out, const std::string& id,
                     const HashMap<std::string, Item>& items,
                     const HashMap<std::string, List<std::string>>& useMap)
{
  Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }
  List<std::string>* lst = useMap.find(id);
  if (!lst || lst->empty()) { out << "No recipes use " << item->name << " as an ingredient.\n"; return; }
  out << "Uses for " << item->name << ":\n";
  for (auto* node = lst->head(); node; node = node->next_)
    out << "- " << node->data << '\n';
}

inline void search(std::ostream& out, const std::string& prefix,
                   const HashMap<std::string, Item>& items)
{
  out << "Items starting with '" << prefix << "':\n";
  bool found = false;
  std::string lower_prefix = prefix;
  for (char& c : lower_prefix) c = static_cast<char>(std::tolower(c));
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    std::string name_lower = it->value.name;
    for (char& c : name_lower) c = static_cast<char>(std::tolower(c));
    if (name_lower.compare(0, lower_prefix.size(), lower_prefix) == 0)
    {
      out << it->value.name << '\n';
      found = true;
    }
  }
  if (!found) out << "No items starting with '" << prefix << "' found.\n";
}

inline void itemInfo(std::ostream& out, const std::string& id,
                     const HashMap<std::string, Item>& items)
{
  Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }
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
  out << "Durability: " << (item->durability >= 0 ? std::to_string(item->durability) : "N/A") << '\n';
  out << "Max stack size: " << item->maxStack << '\n';
  out << "Category: " << (item->type == ItemType::Weapon || item->type == ItemType::Armor ? "Combat" : "Other") << '\n';
}

inline void bestPvpPack(std::ostream& out, const std::string& args,
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

  OptimalPack best = computeOptimalPack(inventory, items, recipes);

  if (best.power < 0)
  {
    out << "No PvP items can be crafted from the given resources.\n";
    return;
  }

  out << "Optimal PvP pack from given resources:\n";
  for (auto* node = best.items.head(); node; node = node->next_)
  {
    const Item* item = node->data;
    out << "- " << item->name << " [";
    switch (item->slot)
    {
      case SlotType::Weapon: out << "Weapon"; break;
      case SlotType::Head:   out << "Head"; break;
      case SlotType::Chest:  out << "Chest"; break;
      case SlotType::Legs:   out << "Legs"; break;
      case SlotType::Feet:   out << "Feet"; break;
      case SlotType::Offhand:out << "Offhand"; break;
      default: out << "None";
    }
    out << "]\n";
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
  out << "\nRemaining resources: ";
  first = true;
  for (auto it = best.remaining.begin(); it != best.remaining.end(); ++it)
  {
    if (!first) out << ", ";
    out << it->value << "x " << it->key;
    first = false;
  }
  out << "\nCombat efficiency score: " << best.power << '\n';
}

inline void whatToAdd(std::ostream& out, const std::string& args,
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

  OptimalPack current = computeOptimalPack(inventory, items, recipes);
  if (current.power < 0)
  {
    out << "Cannot craft any PvP items from the given resources. No suggestions.\n";
    return;
  }

  bool improved = false;
  List<const Item*> extraItems;
  HashMap<std::string, int> newRemaining = current.remaining;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    const Item& item = it->value;
    if (item.power <= 0.0 || item.slot != SlotType::None) continue;
    bool already = false;
    for (auto* node = current.items.head(); node; node = node->next_)
    {
      if (node->data->id == item.id) { already = true; break; }
    }
    if (already) continue;
    HashMap<std::string, int> used;
    if (canCraftItem(item.id, newRemaining, recipes, used))
    {
      extraItems.pushFront(&item);
      for (auto uit = used.begin(); uit != used.end(); ++uit)
      {
        std::string key = uit->key;
        int spent = uit->value;
        int* v = newRemaining.find(key);
        if (v)
        {
          *v -= spent;
          if (*v <= 0) newRemaining.remove(key);
        }
      }
      improved = true;
    }
  }

  if (improved)
  {
    out << "You can further improve your pack using leftover resources:\n";
    for (auto* node = extraItems.head(); node; node = node->next_)
      out << "- " << node->data->name << " (adds " << node->data->power << " efficiency)\n";
    out << "Remaining after crafting these: ";
    bool first = true;
    for (auto it = newRemaining.begin(); it != newRemaining.end(); ++it)
    {
      if (!first) out << ", ";
      out << it->value << "x " << it->key;
      first = false;
    }
    out << '\n';
    out << "Total combat efficiency would be " << (current.power + extraItems.head()->data.power) << ".\n";
    return;
  }

  out << "No direct improvements from leftover resources.\n";
  out << "To improve your PvP pack, consider upgrading:\n";

  HashMap<int, List<Item*>> slotCandidates;
  buildSlotCandidates(items, slotCandidates);

  bool suggestionFound = false;
  for (auto* node = current.items.head(); node; node = node->next_)
  {
    const Item* curItem = node->data;
    int slot = static_cast<int>(curItem->slot);
    if (slot == static_cast<int>(SlotType::None)) continue;

    List<Item*>* candList = slotCandidates.find(slot);
    if (!candList) continue;

    std::vector<Item*> sortedCand;
    for (auto* cn = candList->head(); cn; cn = cn->next_)
      sortedCand.push_back(cn->data);
    std::sort(sortedCand.begin(), sortedCand.end(),
              [](const Item* a, const Item* b) { return a->power < b->power; });

    auto it = std::find_if(sortedCand.begin(), sortedCand.end(),
                           [curItem](const Item* i) { return i->id == curItem->id; });
    if (it == sortedCand.end()) continue;
    auto next = std::next(it);
    if (next == sortedCand.end()) continue;

    const Item* nextItem = *next;
    HashMap<std::string, int> required;
    std::function<void(const std::string&)> gatherReq = [&](const std::string& id) {
      int* r = required.find(id);
      if (r) *r += 1;
      else required.insert(id, 1);
      Recipe* rec = recipes.find(id);
      if (rec)
      {
        for (const Ingredient& ing : rec->ingredients)
          for (int i = 0; i < ing.count; ++i)
            gatherReq(ing.item_id);
      }
    };
    gatherReq(nextItem->id);

    HashMap<std::string, int> deficit;
    for (auto rit = required.begin(); rit != required.end(); ++rit)
    {
      int need = rit->value;
      int* have = current.remaining.find(rit->key);
      if (have) need -= *have;
      if (need > 0) deficit.insert(rit->key, need);
    }

    if (!deficit.empty())
    {
      out << "- Upgrade " << curItem->name << " (" << curItem->power << ") -> "
          << nextItem->name << " (" << nextItem->power << "), add: ";
      bool firstDef = true;
      for (auto dit = deficit.begin(); dit != deficit.end(); ++dit)
      {
        if (!firstDef) out << ", ";
        out << dit->value << "x " << dit->key;
        firstDef = false;
      }
      out << '\n';
      suggestionFound = true;
    }
  }

  if (!suggestionFound)
  {
    out << "All equipped items are already the best possible in their slots.\n";
  }
}

#endif
