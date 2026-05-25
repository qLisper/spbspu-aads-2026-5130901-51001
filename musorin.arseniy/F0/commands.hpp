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
  // Тип
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

#endif
