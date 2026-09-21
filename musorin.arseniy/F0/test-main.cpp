cat > test.cpp << 'EOF'
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include "data.hpp"
#include "hashmap.hpp"
#include "list.hpp"
#include "commands.hpp"
#include "crafting.hpp"

static int passed = 0;
static int failed = 0;

static void check(bool cond, const char* desc)
{
  if (cond)
  {
    ++passed;
  }
  else
  {
    ++failed;
    std::cerr << "FAIL: " << desc << '\n';
  }
}

void testLoadData()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);

  check(items.size() == 40, "loadData: 40 items");
  check(items.contains("iron_sword"), "loadData: iron_sword exists");
  check(items.contains("planks"), "loadData: planks exists");
  check(recipes.size() == 27, "loadData: 27 recipes");
  check(useMap.contains("stick"), "loadData: stick in useMap");
}

void testHashMap()
{
  HashMap<std::string, int> map;
  map.insert("a", 1);
  map.insert("b", 2);
  check(map.size() == 2, "HashMap: size 2");
  check(*map.find("a") == 1, "HashMap: find a");
  check(!map.contains("c"), "HashMap: not contains c");

  int sum = 0;
  for (auto it = map.begin(); it != map.end(); ++it)
  {
    sum += it->value;
  }
  check(sum == 3, "HashMap: iterate sum");
}

void testList()
{
  List<int> lst;
  lst.pushFront(1);
  lst.pushFront(2);
  check(lst.size() == 2, "List: size 2");
  check(lst.find(1) != nullptr, "List: find 1");
  check(lst.find(2) != nullptr, "List: find 2");
  check(lst.find(3) == nullptr, "List: find 3 absent");
}

void testSearch()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);
  std::ostringstream out;
  search(out, "iron", items);
  std::string res = out.str();
  check(res.find("Iron Sword") != std::string::npos, "search: found Iron Sword");
  check(res.find("Iron Ingot") != std::string::npos, "search: found Iron Ingot");
}

void testShowRecipe()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);
  std::ostringstream out;
  showRecipe(out, "iron_sword", items, recipes);
  check(out.str().find("2x iron_ingot") != std::string::npos, "showRecipe: iron_ingot mentioned");
}

void testCanCraft()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);

  HashMap<std::string, int> inv;
  inv.insert("iron_ingot", 2);
  inv.insert("stick", 1);
  HashMap<std::string, int> used;
  bool ok = canCraftItem("iron_sword", inv, recipes, used);
  check(ok, "canCraftItem: iron_sword craftable");
}

void testBestPvpPack()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);
  std::ostringstream out;
  bestPvpPack(out, "iron_ingot iron_ingot stick", items, recipes);
  std::string res = out.str();
  check(res.find("Iron Sword") != std::string::npos, "bestPvpPack: Iron Sword in result");
}

void testWhatToAdd()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;
  loadData(items, recipes, useMap);
  std::ostringstream out;
  whatToAdd(out, "iron_ingot iron_ingot stick", items, recipes);
  std::string res = out.str();
  // Либо предлагает улучшения, либо говорит, что можно улучшить из остатков
  check(res.find("Upgrade") != std::string::npos ||
        res.find("improve") != std::string::npos,
        "whatToAdd: suggests upgrades");
}

int main()
{
  testLoadData();
  testHashMap();
  testList();
  testSearch();
  testShowRecipe();
  testCanCraft();
  testBestPvpPack();
  testWhatToAdd();

  std::cout << passed << " tests passed, " << failed << " tests failed.\n";
  return failed == 0 ? 0 : 1;
}
EOF
