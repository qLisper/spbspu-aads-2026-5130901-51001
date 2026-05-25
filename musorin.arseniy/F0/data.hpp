#ifndef DATA_HPP
#define DATA_HPP

#include "item.hpp"
#include "hashmap.hpp"
#include "list.hpp"

inline void loadData(HashMap<std::string, Item>& items,
                     HashMap<std::string, Recipe>& recipes,
                     HashMap<std::string, List<std::string>>& useMap)
{
  // Материалы 
  items.insert("planks", {"planks", "Planks", ItemType::Material, -1, 64, SlotType::None, 0.1});
  items.insert("stick", {"stick", "Stick", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("iron_ingot", {"iron_ingot", "Iron Ingot", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("gold_ingot", {"gold_ingot", "Gold Ingot", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("diamond", {"diamond", "Diamond", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("netherite_ingot", {"netherite_ingot", "Netherite Ingot", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("flint", {"flint", "Flint", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("feather", {"feather", "Feather", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("string", {"string", "String", ItemType::Material, -1, 64, SlotType::None, 0.0});
  items.insert("apple", {"apple", "Apple", ItemType::Food, -1, 64, SlotType::None, 0.0});

  // Оружие
  items.insert("wooden_sword", {"wooden_sword", "Wooden Sword", ItemType::Weapon, 59, 1, SlotType::Weapon, 12.8});
  items.insert("golden_sword", {"golden_sword", "Golden Sword", ItemType::Weapon, 32, 1, SlotType::Weapon, 12.8});
  items.insert("iron_sword", {"iron_sword", "Iron Sword", ItemType::Weapon, 250, 1, SlotType::Weapon, 19.2});
  items.insert("diamond_sword", {"diamond_sword", "Diamond Sword", ItemType::Weapon, 1561, 1, SlotType::Weapon, 22.4});
  items.insert("netherite_sword", {"netherite_sword", "Netherite Sword", ItemType::Weapon, 2031, 1, SlotType::Weapon, 25.6});

  // Броня
  items.insert("golden_helmet", {"golden_helmet", "Golden Helmet", ItemType::Armor, 77, 1, SlotType::Head, 1.5});
  items.insert("iron_helmet", {"iron_helmet", "Iron Helmet", ItemType::Armor, 165, 1, SlotType::Head, 3.0});
  items.insert("diamond_helmet", {"diamond_helmet", "Diamond Helmet", ItemType::Armor, 363, 1, SlotType::Head, 6.0});
  items.insert("netherite_helmet", {"netherite_helmet", "Netherite Helmet", ItemType::Armor, 407, 1, SlotType::Head, 6.75});

  items.insert("golden_chestplate", {"golden_chestplate", "Golden Chestplate", ItemType::Armor, 112, 1, SlotType::Chest, 3.75});
  items.insert("iron_chestplate", {"iron_chestplate", "Iron Chestplate", ItemType::Armor, 240, 1, SlotType::Chest, 9.0});
  items.insert("diamond_chestplate", {"diamond_chestplate", "Diamond Chestplate", ItemType::Armor, 528, 1, SlotType::Chest, 13.5});
  items.insert("netherite_chestplate", {"netherite_chestplate", "Netherite Chestplate", ItemType::Armor, 592, 1, SlotType::Chest, 14.25});

  items.insert("golden_leggings", {"golden_leggings", "Golden Leggings", ItemType::Armor, 105, 1, SlotType::Legs, 2.25});
  items.insert("iron_leggings", {"iron_leggings", "Iron Leggings", ItemType::Armor, 225, 1, SlotType::Legs, 7.5});
  items.insert("diamond_leggings", {"diamond_leggings", "Diamond Leggings", ItemType::Armor, 495, 1, SlotType::Legs, 10.5});
  items.insert("netherite_leggings", {"netherite_leggings", "Netherite Leggings", ItemType::Armor, 555, 1, SlotType::Legs, 11.25}) 
  
  items.insert("golden_boots", {"golden_boots", "Golden Boots", ItemType::Armor, 91, 1, SlotType::Feet, 0.75});
  items.insert("iron_boots", {"iron_boots", "Iron Boots", ItemType::Armor, 195, 1, SlotType::Feet, 3.0});
  items.insert("diamond_boots", {"diamond_boots", "Diamond Boots", ItemType::Armor, 429, 1, SlotType::Feet, 6.0});
  items.insert("netherite_boots", {"netherite_boots", "Netherite Boots", ItemType::Armor, 481, 1, SlotType::Feet, 6.75});

  // Еда
  items.insert("golden_apple", {"golden_apple", "Golden Apple", ItemType::Food, -1, 64, SlotType::None, 2.0});

  // Вспомогательные
  items.insert("bow", {"bow", "Bow", ItemType::Weapon, 384, 1, SlotType::Weapon, 5.0});
  items.insert("arrow", {"arrow", "Arrow", ItemType::Material, -1, 64, SlotType::None, 0.1});
  items.insert("ender_pearl", {"ender_pearl", "Ender Pearl", ItemType::Material, -1, 16, SlotType::None, 1.0});
  items.insert("lava_bucket", {"lava_bucket", "Lava Bucket", ItemType::Other, -1, 1, SlotType::None, 5.0});
  items.insert("water_bucket", {"water_bucket", "Water Bucket", ItemType::Other, -1, 1, SlotType::None, 3.0});
  items.insert("flint_and_steel", {"flint_and_steel", "Flint and Steel", ItemType::Other, 64, 1, SlotType::None, 2.0});
  items.insert("fishing_rod", {"fishing_rod", "Fishing Rod", ItemType::Other, 64, 1, SlotType::None, 2.0});
  items.insert("totem_of_undying", {"totem_of_undying", "Totem of Undying", ItemType::Other, -1, 1, SlotType::Offhand, 5.0});
  items.insert("shield", {"shield", "Shield", ItemType::Other, 336, 1, SlotType::Offhand, 6.0});

  // Оружие
  recipes.insert("wooden_sword", {"wooden_sword", {{"planks", 2}, {"stick", 1}}});
  recipes.insert("golden_sword", {"golden_sword", {{"gold_ingot", 2}, {"stick", 1}}});
  recipes.insert("iron_sword", {"iron_sword", {{"iron_ingot", 2}, {"stick", 1}}});
  recipes.insert("diamond_sword", {"diamond_sword", {{"diamond", 2}, {"stick", 1}}});
  recipes.insert("netherite_sword", {"netherite_sword", {{"diamond_sword", 1}, {"netherite_ingot", 1}}});
  // Броня
  recipes.insert("golden_helmet", {"golden_helmet", {{"gold_ingot", 5}}});
  recipes.insert("iron_helmet", {"iron_helmet", {{"iron_ingot", 5}}});
  recipes.insert("diamond_helmet", {"diamond_helmet", {{"diamond", 5}}});
  recipes.insert("netherite_helmet", {"netherite_helmet", {{"diamond_helmet", 1}, {"netherite_ingot", 1}}});

  recipes.insert("golden_chestplate", {"golden_chestplate", {{"gold_ingot", 8}}});
  recipes.insert("iron_chestplate", {"iron_chestplate", {{"iron_ingot", 8}}});
  recipes.insert("diamond_chestplate", {"diamond_chestplate", {{"diamond", 8}}});
  recipes.insert("netherite_chestplate", {"netherite_chestplate", {{"diamond_chestplate", 1}, {"netherite_ingot", 1}}});

  recipes.insert("golden_leggings", {"golden_leggings", {{"gold_ingot", 7}}});
  recipes.insert("iron_leggings", {"iron_leggings", {{"iron_ingot", 7}}});
  recipes.insert("diamond_leggings", {"diamond_leggings", {{"diamond", 7}}});
  recipes.insert("netherite_leggings", {"netherite_leggings", {{"diamond_leggings", 1}, {"netherite_ingot", 1}}});

  recipes.insert("golden_boots", {"golden_boots", {{"gold_ingot", 4}}});
  recipes.insert("iron_boots", {"iron_boots", {{"iron_ingot", 4}}});
  recipes.insert("diamond_boots", {"diamond_boots", {{"diamond", 4}}});
  recipes.insert("netherite_boots", {"netherite_boots", {{"diamond_boots", 1}, {"netherite_ingot", 1}}});
  // другое
  recipes.insert("golden_apple", {"golden_apple", {{"apple", 1}, {"gold_ingot", 8}}});
  recipes.insert("bow", {"bow", {{"stick", 3}, {"string", 3}}});
  recipes.insert("arrow", {"arrow", {{"flint", 1}, {"stick", 1}, {"feather", 1}}});
  recipes.insert("flint_and_steel", {"flint_and_steel", {{"iron_ingot", 1}, {"flint", 1}}});
  recipes.insert("fishing_rod", {"fishing_rod", {{"stick", 3}, {"string", 2}}});
  recipes.insert("shield", {"shield", {{"iron_ingot", 1}, {"planks", 5}}});

  auto addUse = [&](const std::string& ingredient, const std::string& result)
  {
    List<std::string>* lst = useMap.find(ingredient);
    if (!lst)
    {
      List<std::string> emptyList;
      useMap.insert(ingredient, emptyList);
      lst = useMap.find(ingredient);
    }
    lst->pushFront(result);
  };

  std::string recipeIds[] = {
    "wooden_sword", "golden_sword", "iron_sword", "diamond_sword", "netherite_sword",
    "golden_helmet", "iron_helmet", "diamond_helmet", "netherite_helmet",
    "golden_chestplate", "iron_chestplate", "diamond_chestplate", "netherite_chestplate",
    "golden_leggings", "iron_leggings", "diamond_leggings", "netherite_leggings",
    "golden_boots", "iron_boots", "diamond_boots", "netherite_boots",
    "golden_apple", "bow", "arrow", "flint_and_steel", "fishing_rod", "shield"
  };

  for (const std::string& rid : recipeIds)
  {
    Recipe* rec = recipes.find(rid);
    if (rec)
    {
      for (const Ingredient& ing : rec->ingredients)
      {
        addUse(ing.item_id, rec->result_id);
      }
    }
  }
}

#endif
