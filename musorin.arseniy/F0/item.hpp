#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <vector>

enum class ItemType
{
  Material,
  Weapon,
  Armor,
  Food,
  Other
};

enum class SlotType
{
  Weapon,
  Head,
  Chest,
  Legs,
  Feet,
  Offhand,
  None
};

struct Item
{
  std::string id;
  std::string name;
  ItemType type;
  int durability;
  int maxStack;
  SlotType slot;
  double power;
};

struct Recipe
{
  std::string result_id;
  std::vector<Ingredient> ingredients;
};
struct Ingredient
{
  std::string item_id;
  int count;
};

#endif
