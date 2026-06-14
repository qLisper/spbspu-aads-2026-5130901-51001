#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
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
    while (start < line.size() && line[start] == ') ++start;
    if (start == line.size()) break;
    size_t end = start;
    while (end < line.size() && line[end] != ') ++end;
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

 
  return 0;
}
