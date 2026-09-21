#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <string>
#include <cctype>
#include "hashmap.hpp"
#include "item.hpp"
#include "list.hpp"

inline void listItems(std::ostream& out,
                      const HashMap<std::string, Item>& items)
{
  out << "(" << items.size() << " items)\n";
  for (auto it = items.begin(); it != items.end(); ++it)
    out << it->value.name << '\n';
}

inline void showRecipe(std::ostream& out, const std::string& id,
                       const HashMap<std::string, Item>& items,
                       const HashMap<std::string, Recipe>& recipes)
{
  const Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }

  const Recipe* rec = recipes.find(id);
  if (!rec) { out << "No crafting recipe for " << item->name << ".\n"; return; }

  out << "Recipe for " << item->name << ":\n";
  bool first = true;
  for (const Ingredient& ing : rec->ingredients)
  {
    if (!first) out << ", ";
    out << ing.count << " " << ing.item_id;
    first = false;
  }
  out << '\n';
}

inline void showUses(std::ostream& out, const std::string& id,
                     const HashMap<std::string, Item>& items,
                     const HashMap<std::string, List<std::string>>& useMap)
{
  const Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }

  const List<std::string>* lst = useMap.find(id);
  if (!lst || lst->empty())
  {
    out << "No recipes use " << item->name << " as an ingredient.\n";
    return;
  }

  out << "Uses for " << item->name << ":\n";
  for (auto it = lst->begin(); it != lst->end(); ++it)
  {
    const Item* used = items.find(*it);
    out << "- " << (used ? used->name : *it) << '\n';
  }
}

inline void search(std::ostream& out, const std::string& prefix,
                   const HashMap<std::string, Item>& items)
{
  std::string lower_prefix = prefix;
  for (char& c : lower_prefix) c = static_cast<char>(std::tolower(c));

  std::vector<const Item*> hits;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    std::string name_lower = it->value.name;
    for (char& c : name_lower) c = static_cast<char>(std::tolower(c));
    if (name_lower.compare(0, lower_prefix.size(), lower_prefix) == 0)
      hits.push_back(&it->value);
  }

  if (hits.empty())
  {
    out << "No items starting with '" << prefix << "' found.\n";
    return;
  }

  out << "Items starting with '" << prefix << "':\n";
  for (const Item* it : hits)
    out << it->name << '\n';
}

inline void itemInfo(std::ostream& out, const std::string& id,
                     const HashMap<std::string, Item>& items)
{
  const Item* item = items.find(id);
  if (!item) { out << "Item not found.\n"; return; }

  out << "Item: " << item->name << '\n';

  std::string typeStr;
  switch (item->type)
  {
    case ItemType::Material: typeStr = "Material"; break;
    case ItemType::Weapon:   typeStr = "Weapon";   break;
    case ItemType::Armor:    typeStr = "Armor";    break;
    case ItemType::Food:     typeStr = "Food";     break;
    case ItemType::Other:    typeStr = "Other";    break;
  }
  out << "Type: " << typeStr << '\n';
  out << "Durability: "
      << (item->durability >= 0 ? std::to_string(item->durability) : "N/A")
      << '\n';
  out << "Max stack size: " << item->maxStack << '\n';

  bool enchantable = (item->type == ItemType::Weapon ||
                      item->type == ItemType::Armor);
  out << "Enchantable: " << (enchantable ? "Yes" : "No") << '\n';

  out << "Category: "
      << (enchantable ? "Combat" : "Other")
      << '\n';
}

#endif
