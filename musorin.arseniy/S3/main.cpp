#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include "hashtable.hpp"
#include "graph.hpp"

namespace
{

using musorin::HashTable;
using musorin::Graph;
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

bool loadGraphs(std::istream& in, HashTable<std::string, Graph>& graphs)
{
  std::string line;
  while (std::getline(in, line))
  {
    if (line.empty()) continue;
    List<std::string> tokens;
    parseLine(line, tokens);
    if (tokens.size() < 2)
    {
      std::cerr << "Error: invalid graph header\n";
      return false;
    }
    auto it = tokens.cbegin();
    std::string graphName = *it; ++it;
    size_t edgesCount = 0;
    try
    {
      edgesCount = static_cast<size_t>(std::stoull(*it));
    }
    catch (...)
    {
      std::cerr << "Error: invalid edges count\n";
      return false;
    }
    Graph g;
    for (size_t i = 0; i < edgesCount; ++i)
    {
      if (!std::getline(in, line))
      {
        std::cerr << "Error: missing edge data\n";
        return false;
      }
      if (line.empty()) { --i; continue; }
      List<std::string> edgeTokens;
      parseLine(line, edgeTokens);
      if (edgeTokens.size() < 3)
      {
        std::cerr << "Error: invalid edge format\n";
        return false;
      }
      auto eit = edgeTokens.cbegin();
      std::string from = *eit; ++eit;
      std::string to = *eit; ++eit;
      size_t weight = 0;
      try
      {
        weight = static_cast<size_t>(std::stoull(*eit));
      }
      catch (...)
      {
        std::cerr << "Error: invalid weight\n";
        return false;
      }
      g.addEdge(from, to, weight);
    }
    graphs.add(graphName, g);
  }
  return true;
}

template< class T >
void sortList(List<T>& list)
{
  size_t n = list.size();
  T* arr = new T[n];
  size_t i = 0;
  for (auto it = list.cbegin(); it != list.cend(); ++it)
  {
    arr[i++] = *it;
  }
  std::sort(arr, arr + n);
  list.clear();
  for (size_t j = 0; j < n; ++j)
  {
    list.pushBack(arr[j]);
  }
  delete[] arr;
}

void printInvalidCommand()
{
  std::cout << "<INVALID COMMAND>\n";
}

}

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

  musorin::HashTable<std::string, musorin::Graph> graphs;
  if (!loadGraphs(file, graphs))
  {
    return 1;
  }

  using CommandHandler = std::function<void(List<std::string>&)>;
  musorin::HashTable<std::string, CommandHandler> commands;


  commands.add("graphs", [&graphs](List<std::string>&) {
    List<std::string> names;
    for (auto it = graphs.begin(); it != graphs.end(); ++it)
    {
      names.pushBack(it->first);
    }
    sortList(names);
    for (auto nit = names.cbegin(); nit != names.cend(); ++nit)
    {
      std::cout << *nit << '\n';
    }
  });

  commands.add("vertexes", [&graphs](List<std::string>& args) {
    if (args.size() != 1)
    {
      printInvalidCommand();
      return;
    }
    std::string graphName = args.front();
    if (!graphs.has(graphName))
    {
      printInvalidCommand();
      return;
    }
    musorin::Graph& g = graphs.at(graphName);
    List<std::string> verts = g.getVertexList();
    sortList(verts);
    for (auto it = verts.cbegin(); it != verts.cend(); ++it)
    {
      std::cout << *it << '\n';
    }
  });

  commands.add("create", [&graphs](List<std::string>& args) {
    if (args.size() < 2)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string gname = *it; ++it;
    size_t k = 0;
    try
    {
      k = static_cast<size_t>(std::stoull(*it));
    }
    catch (...)
    {
      printInvalidCommand();
      return;
    }
    if (args.size() != 2 + k)
    {
      printInvalidCommand();
      return;
    }
    if (graphs.has(gname))
    {
      printInvalidCommand();
      return;
    }
    musorin::Graph g;
    for (size_t i = 0; i < k; ++i)
    {
      ++it;
      g.addVertex(*it);
    }
    graphs.add(gname, g);
  });

  commands.add("bind", [&graphs](List<std::string>& args) {
    if (args.size() != 4)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string gname = *it; ++it;
    std::string from = *it; ++it;
    std::string to = *it; ++it;
    size_t weight = 0;
    try
    {
      weight = static_cast<size_t>(std::stoull(*it));
    }
    catch (...)
    {
      printInvalidCommand();
      return;
    }
    if (!graphs.has(gname))
    {
      printInvalidCommand();
      return;
    }
    graphs.at(gname).addEdge(from, to, weight);
  });

  commands.add("cut", [&graphs](List<std::string>& args) {
    if (args.size() != 4)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string gname = *it; ++it;
    std::string from = *it; ++it;
    std::string to = *it; ++it;
    size_t weight = 0;
    try
    {
      weight = static_cast<size_t>(std::stoull(*it));
    }
    catch (...)
    {
      printInvalidCommand();
      return;
    }
    if (!graphs.has(gname))
    {
      printInvalidCommand();
      return;
    }
    musorin::Graph& g = graphs.at(gname);
    if (!g.hasVertex(from) || !g.hasVertex(to))
    {
      printInvalidCommand();
      return;
    }
    if (!g.removeEdge(from, to, weight))
    {
      printInvalidCommand();
      return;
    }
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
