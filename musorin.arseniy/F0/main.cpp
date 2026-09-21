#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>
#include "data.hpp"
#include "commands.hpp"
#include "crafting.hpp"
#include "optimizer.hpp"

static bool isNumberToken(const std::string& s)
{
  if (s.empty()) return false;
  for (char c : s)
    if (!std::isdigit(static_cast<unsigned char>(c))) return false;
  return true;
}

static bool parseInventory(const std::string& cmd,
                           std::istringstream& iss,
                           const HashMap<std::string, Item>& items,
                           HashMap<std::string, int>& out)
{
  std::vector<std::string> toks;
  std::string t;
  while (iss >> t) toks.push_back(t);

  std::size_t i = 0;
  while (i < toks.size())
  {
    int cnt = 1;
    if (isNumberToken(toks[i]))
    {
      cnt = std::stoi(toks[i]);
      ++i;
      if (i >= toks.size())
      {
        std::cerr << "Usage: " << cmd << " [count] <item-id> ...\n";
        return false;
      }
    }

    std::string id = toks[i];
    std::size_t consumed = 1;

    while (i + consumed < toks.size() && !isNumberToken(toks[i + consumed]))
    {
      std::string ext = id + "_" + toks[i + consumed];
      if (!items.find(ext)) break;
      id = ext;
      ++consumed;
    }

    if (!items.find(id))
    {
      std::cout << "<ITEM NOT FOUND: " << toks[i] << ">\n";
      return false;
    }

    int* v = out.find(id);
    if (v) *v += cnt;
    else   out.insert(id, cnt);

    i += consumed;
  }
  return true;
}

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
    else if (cmd == "best-pvp-pack")
    {
      HashMap<std::string, int> inventory;
      if (!parseInventory(cmd, iss, items, inventory)) continue;

      std::vector<std::string> pack;
      HashMap<std::string, int> remaining;
      double score = 0.0;
      if (!bestPvpPack(inventory, items, recipes, pack, remaining, score))
      {
        std::cout << "No PvP items can be crafted from the given resources.\n";
        continue;
      }

      std::cout << "Optimal PvP pack from given resources:\n";
      for (const std::string& id : pack)
      {
        const Item* item = items.find(id);
        if (!item) continue;
        std::string slotName;
        switch (item->slot)
        {
          case SlotType::Weapon:  slotName = "Weapon";  break;
          case SlotType::Head:    slotName = "Head";    break;
          case SlotType::Chest:   slotName = "Chest";   break;
          case SlotType::Legs:    slotName = "Legs";    break;
          case SlotType::Feet:    slotName = "Feet";    break;
          case SlotType::Offhand: slotName = "Offhand"; break;
          default:                slotName = "Other";   break;
        }
        std::cout << "- " << item->name << " [" << slotName << "]\n";
      }

      bool firstUsed = true;
      std::cout << "Resources used: ";
      for (auto it = inventory.begin(); it != inventory.end(); ++it)
      {
        const int* remPtr = remaining.find(it->key);
        int rem = remPtr ? *remPtr : 0;
        int used = it->value - rem;
        if (used <= 0) continue;

        if (!firstUsed) std::cout << ", ";
        const Item* ui = items.find(it->key);
        std::cout << used << " " << (ui ? ui->name : it->key);
        firstUsed = false;
      }
      std::cout << "\n";

      bool firstRem = true;
      for (auto it = remaining.begin(); it != remaining.end(); ++it)
      {
        if (it->value <= 0) continue;
        if (firstRem)
        {
          std::cout << "Remaining resources: ";
          firstRem = false;
        }
        else
        {
          std::cout << ", ";
        }
        const Item* ri = items.find(it->key);
        std::cout << it->value << " " << (ri ? ri->name : it->key);
      }
      if (!firstRem) std::cout << "\n";

      std::cout << "Combat efficiency score: " << score << "\n";
    }
    else if (cmd == "what-to-add")
    {
      HashMap<std::string, int> inventory;
      if (!parseInventory(cmd, iss, items, inventory)) continue;
      whatToAdd(inventory, items, recipes, std::cout);
    }
    else
    {
      std::cerr << "Unknown command: " << cmd << '\n';
    }
  }
  return 0;
}
