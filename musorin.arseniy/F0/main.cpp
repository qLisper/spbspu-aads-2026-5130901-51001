#include <iostream>
#include "data.hpp"
#include "hashmap.hpp"
#include "item.hpp"

int main()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;

  loadData(items, recipes, useMap);

  std::cout << "(" << items.size() << " items)\n";
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    std::cout << it->value.name << '\n';
  }

  return 0;
}
