#include <iostream>
#include <string>
#include <sstream>
#include "data.hpp"
#include "commands.hpp"

int main()
{
  HashMap<std::string, Item> items;
  HashMap<std::string, Recipe> recipes;
  HashMap<std::string, List<std::string>> useMap;

  loadData(items, recipes, useMap);

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty()) continue;
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    if (cmd == "list-items")
    {
      listItems(std::cout, items);
    }
    else if (cmd == "show-recipe")
    {
      std::string id;
      if (!(iss >> id))
      {
        std::cerr << "Usage: show-recipe <item-id>\n";
        continue;
      }
      showRecipe(std::cout, id, items, recipes);
    }
    else if (cmd == "show-uses")
    {
      std::string id;
      if (!(iss >> id))
      {
        std::cerr << "Usage: show-uses <item-id>\n";
        continue;
      }
      showUses(std::cout, id, items, useMap);
    }
    else if (cmd == "search")
    {
      std::string prefix;
      if (!(iss >> prefix))
      {
        std::cerr << "Usage: search <prefix>\n";
        continue;
      }
      search(std::cout, prefix, items);
    }
    else if (cmd == "item-info")
    {
      std::string id;
      if (!(iss >> id))
      {
        std::cerr << "Usage: item-info <item-id>\n";
        continue;
      }
      itemInfo(std::cout, id, items);
    }
    else
    {
      std::cerr << "Unknown command: " << cmd << '\n';
    }
  }
  return 0;
}
