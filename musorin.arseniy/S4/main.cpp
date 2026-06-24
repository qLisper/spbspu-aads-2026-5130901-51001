#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>
#include "hashtable.hpp"
#include "bstree.hpp"

namespace
{

using musorin::HashTable;
using musorin::BSTree;
using musorin::List;

void parseLine(const std::string& line, List<std::string>& args)
{
  size_t start = 0;
  while (start < line.size())
  {
    while (start < line.size() && line[start] == ' ') ++start;
    if (start == line.size()) break;
    size_t end = start;
    while (end < line.size() && line[end] != ' ') ++end;
    args.pushBack(line.substr(start, end - start));
    start = end;
  }
}

bool loadDatasets(std::istream& in, HashTable<std::string, BSTree<int, std::string>>& datasets)
{
  std::string line;
  while (std::getline(in, line))
  {
    if (line.empty()) continue;
    List<std::string> tokens;
    parseLine(line, tokens);
    if (tokens.size() < 3 || tokens.size() % 2 != 1)
    {
      std::cerr << "Error: invalid dataset format\n";
      return false;
    }
    auto it = tokens.cbegin();
    std::string name = *it; ++it;
    BSTree<int, std::string> tree;
    while (it != tokens.cend())
    {
      int key = 0;
      try
      {
        key = std::stoi(*it);
      }
      catch (...)
      {
        std::cerr << "Error: invalid key\n";
        return false;
      }
      ++it;
      if (it == tokens.cend())
      {
        std::cerr << "Error: missing value\n";
        return false;
      }
      std::string value = *it; ++it;
      tree.push(key, value);
    }
    datasets.add(name, tree);
  }
  return true;
}

void printDataset(const std::string& name, const BSTree<int, std::string>& tree)
{
  if (tree.empty())
  {
    std::cout << "<EMPTY>\n";
    return;
  }
  std::cout << name;
  for (auto it = tree.cbegin(); it != tree.cend(); ++it)
  {
    std::cout << ' ' << (*it).first << ' ' << (*it).second;
  }
  std::cout << '\n';
}

BSTree<int, std::string> complementTrees(const BSTree<int, std::string>& a,
                                         const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result;
  for (auto it = a.cbegin(); it != a.cend(); ++it)
  {
    if (!b.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

BSTree<int, std::string> intersectTrees(const BSTree<int, std::string>& a,
                                        const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result;
  for (auto it = a.cbegin(); it != a.cend(); ++it)
  {
    if (b.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

BSTree<int, std::string> unionTrees(const BSTree<int, std::string>& a,
                                    const BSTree<int, std::string>& b)
{
  BSTree<int, std::string> result = a;
  for (auto it = b.cbegin(); it != b.cend(); ++it)
  {
    if (!result.has((*it).first))
    {
      result.push((*it).first, (*it).second);
    }
  }
  return result;
}

void printInvalidCommand()
{
  std::cout << "<INVALID COMMAND>\n";
}

} // anonymous namespace

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: ./lab filename\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << "Error: cannot open file " << argv[1] << '\n';
    return 1;
  }

  musorin::HashTable<std::string, musorin::BSTree<int, std::string>> datasets;
  if (!loadDatasets(file, datasets))
  {
    return 1;
  }

  using CommandHandler = std::function<void(List<std::string>&)>;
  musorin::HashTable<std::string, CommandHandler> commands;

  commands.add("print", [&datasets](List<std::string>& args) {
    if (args.size() != 1)
    {
      printInvalidCommand();
      return;
    }
    std::string name = args.front();
    if (!datasets.has(name))
    {
      printInvalidCommand();
      return;
    }
    printDataset(name, datasets.at(name));
  });

  commands.add("complement", [&datasets](List<std::string>& args) {
    if (args.size() != 3)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string newName = *it; ++it;
    std::string name1 = *it; ++it;
    std::string name2 = *it;
    if (!datasets.has(name1) || !datasets.has(name2) || datasets.has(newName))
    {
      printInvalidCommand();
      return;
    }
    datasets.add(newName, complementTrees(datasets.at(name1), datasets.at(name2)));
  });

  commands.add("intersect", [&datasets](List<std::string>& args) {
    if (args.size() != 3)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string newName = *it; ++it;
    std::string name1 = *it; ++it;
    std::string name2 = *it;
    if (!datasets.has(name1) || !datasets.has(name2) || datasets.has(newName))
    {
      printInvalidCommand();
      return;
    }
    datasets.add(newName, intersectTrees(datasets.at(name1), datasets.at(name2)));
  });

  commands.add("union", [&datasets](List<std::string>& args) {
    if (args.size() != 3)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string newName = *it; ++it;
    std::string name1 = *it; ++it;
    std::string name2 = *it;
    if (!datasets.has(name1) || !datasets.has(name2) || datasets.has(newName))
    {
      printInvalidCommand();
      return;
    }
    datasets.add(newName, unionTrees(datasets.at(name1), datasets.at(name2)));
  });

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty()) continue;
    List<std::string> args;
    parseLine(line, args);
    if (args.empty()) continue;
    std::string cmd = args.front();
    args.popFront();
    if (!commands.has(cmd))
    {
      printInvalidCommand();
      continue;
    }
    try
    {
      commands.at(cmd)(args);
    }
    catch (const std::exception&)
    {
      printInvalidCommand();
    }
  }

  return 0;
}
