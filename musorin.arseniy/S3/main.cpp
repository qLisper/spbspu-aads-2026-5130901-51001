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

struct Neighbor
{
  std::string vertex;
  List<size_t> weights;
};

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

void sortNeighbors(List<Neighbor>& neighbors)
{
  size_t n = neighbors.size();
  Neighbor* arr = new Neighbor[n];
  size_t i = 0;
  for (auto it = neighbors.begin(); it != neighbors.end(); ++it) { arr[i++] = *it; }
  std::sort(arr, arr + n, [](const Neighbor& a, const Neighbor& b) {
    return a.vertex < b.vertex;
  });
  neighbors.clear();
  for (size_t j = 0; j < n; ++j) { neighbors.pushBack(arr[j]); }
  delete[] arr;
}

void printInvalidCommand()
{
  std::cout << "<INVALID COMMAND>\n";
}

void printNeighbor(const Neighbor& nb)
{
  std::cout << nb.vertex;
  List<size_t> sortedWeights = nb.weights;
  sortList(sortedWeights);
  for (auto it = sortedWeights.cbegin(); it != sortedWeights.cend(); ++it)
  {
    std::cout << ' ' << *it;
  }
  std::cout << '\n';
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
    if (args.empty())
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string gname = *it;
    size_t k = 0;
    if (args.size() >= 2)
    {
      ++it;
      try
      {
        k = static_cast<size_t>(std::stoull(*it));
      }
      catch (...)
      {
        printInvalidCommand();
        return;
      }
    }
    size_t expected = (k > 0 || args.size() >= 2) ? 2 + k : 1;
    if (args.size() != expected)
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

  commands.add("outbound", [&graphs](List<std::string>& args) {
    if (args.size() != 2)
    {
      printInvalidCommand();
      return;
    }
    auto ait = args.cbegin();
    std::string graphName = *ait; ++ait;
    std::string vertex = *ait;
    if (!graphs.has(graphName) || !graphs.at(graphName).hasVertex(vertex))
    {
      printInvalidCommand();
      return;
    }
    const Graph& g = graphs.at(graphName);
    List<Neighbor> result;
    for (auto eit = g.getEdges().cbegin(); eit != g.getEdges().cend(); ++eit)
    {
      const auto& p = *eit;
      const musorin::EdgeKey& ek = p.first;
      if (ek.from == vertex)
      {
        Neighbor nb;
        nb.vertex = ek.to;
        nb.weights = p.second;
        result.pushBack(nb);
      }
    }
    sortNeighbors(result);
    for (auto rit = result.cbegin(); rit != result.cend(); ++rit)
    {
      printNeighbor(*rit);
    }
  });

  commands.add("inbound", [&graphs](List<std::string>& args) {
    if (args.size() != 2)
    {
      printInvalidCommand();
      return;
    }
    auto ait = args.cbegin();
    std::string graphName = *ait; ++ait;
    std::string vertex = *ait;
    if (!graphs.has(graphName) || !graphs.at(graphName).hasVertex(vertex))
    {
      printInvalidCommand();
      return;
    }
    const Graph& g = graphs.at(graphName);
    List<Neighbor> result;
    for (auto eit = g.getEdges().cbegin(); eit != g.getEdges().cend(); ++eit)
    {
      const auto& p = *eit;
      const musorin::EdgeKey& ek = p.first;
      if (ek.to == vertex)
      {
        Neighbor nb;
        nb.vertex = ek.from;
        nb.weights = p.second;
        result.pushBack(nb);
      }
    }
    sortNeighbors(result);
    for (auto rit = result.cbegin(); rit != result.cend(); ++rit)
    {
      printNeighbor(*rit);
    }
  });

  commands.add("merge", [&graphs](List<std::string>& args) {
    if (args.size() != 3)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string newName = *it; ++it;
    std::string g1 = *it; ++it;
    std::string g2 = *it;
    if (!graphs.has(g1) || !graphs.has(g2) || graphs.has(newName))
    {
      printInvalidCommand();
      return;
    }
    Graph merged = Graph::merge(graphs.at(g1), graphs.at(g2));
    graphs.add(newName, merged);
  });

  commands.add("extract", [&graphs](List<std::string>& args) {
    if (args.size() < 3)
    {
      printInvalidCommand();
      return;
    }
    auto it = args.cbegin();
    std::string newName = *it; ++it;
    std::string oldName = *it; ++it;
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
    if (args.size() != 3 + k)
    {
      printInvalidCommand();
      return;
    }
    if (!graphs.has(oldName) || graphs.has(newName))
    {
      printInvalidCommand();
      return;
    }
    List<std::string> verts;
    for (size_t i = 0; i < k; ++i)
    {
      ++it;
      std::string v = *it;
      if (!graphs.at(oldName).hasVertex(v))
      {
        printInvalidCommand();
        return;
      }
      verts.pushBack(v);
    }
    try
    {
      Graph extracted = Graph::extract(graphs.at(oldName), verts);
      graphs.add(newName, extracted);
    }
    catch (const std::exception&)
    {
      printInvalidCommand();
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
