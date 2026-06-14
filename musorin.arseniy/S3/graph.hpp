#ifndef MUSORIN_GRAPH_HPP
#define MUSORIN_GRAPH_HPP

#include <string>
#include <functional>
#include "hashtable.hpp"
#include "list.hpp"

namespace musorin
{

struct EdgeKey
{
  std::string from;
  std::string to;
};

struct EdgeHash
{
  size_t operator()(const EdgeKey& k) const
  {
    return std::hash<std::string>()(k.from) ^ (std::hash<std::string>()(k.to) << 1);
  }
};

struct EdgeEqual
{
  bool operator()(const EdgeKey& a, const EdgeKey& b) const
  {
    return a.from == b.from && a.to == b.to;
  }
};

class Graph
{
public:
  Graph() {}

  void addVertex(const std::string& name)
  {
    if (!vertexSet_.has(name))
    {
      vertexSet_.add(name, true);
      vertexList_.pushBack(name);
    }
  }

  bool hasVertex(const std::string& name) const
  {
    return vertexSet_.has(name);
  }

  void addEdge(const std::string& from, const std::string& to, size_t weight)
  {
    addVertex(from);
    addVertex(to);
    EdgeKey key{from, to};
    auto it = edges_.find(key);
    if (it != edges_.end())
    {
      it->second.pushBack(weight);
    }
    else
    {
      List<size_t> weights;
      weights.pushBack(weight);
      edges_.add(key, weights);
    }
  }

  bool removeEdge(const std::string& from, const std::string& to, size_t weight)
  {
    if (!hasVertex(from) || !hasVertex(to))
    {
      return false;
    }
    EdgeKey key{from, to};
    auto it = edges_.find(key);
    if (it == edges_.end())
    {
      return false;
    }
    List<size_t>& weights = it->second;
    for (auto wit = weights.begin(); wit != weights.end(); ++wit)
    {
      if (*wit == weight)
      {
        weights.erase(wit);
        if (weights.empty())
        {
          edges_.drop(key);
        }
        return true;
      }
    }
    return false;
  }

  const List<std::string>& getVertexList() const
  {
    return vertexList_;
  }

  const HashTable<EdgeKey, List<size_t>, EdgeHash, EdgeEqual>& getEdges() const
  {
    return edges_;
  }

  static Graph merge(const Graph& a, const Graph& b)
  {
    Graph res;
    for (auto it = a.vertexList_.cbegin(); it != a.vertexList_.cend(); ++it)
    {
      res.addVertex(*it);
    }
    for (auto it = b.vertexList_.cbegin(); it != b.vertexList_.cend(); ++it)
    {
      res.addVertex(*it);
    }
    auto addEdges = [&res](const Graph& g)
    {
      for (auto eit = g.edges_.cbegin(); eit != g.edges_.cend(); ++eit)
      {
        const auto& p = *eit;
        const EdgeKey& ek = p.first;
        const List<size_t>& wlist = p.second;
        for (auto wit = wlist.cbegin(); wit != wlist.cend(); ++wit)
        {
          res.addEdge(ek.from, ek.to, *wit);
        }
      }
    };
    addEdges(a);
    addEdges(b);
    return res;
  }

  static Graph extract(const Graph& g, const List<std::string>& verts)
  {
    Graph res;
    for (auto it = verts.cbegin(); it != verts.cend(); ++it)
    {
      if (!g.hasVertex(*it))
      {
        throw std::runtime_error("Vertex not found");
      }
      res.addVertex(*it);
    }
    for (auto eit = g.edges_.cbegin(); eit != g.edges_.cend(); ++eit)
    {
      const auto& p = *eit;
      const EdgeKey& ek = p.first;
      if (res.hasVertex(ek.from) && res.hasVertex(ek.to))
      {
        const List<size_t>& wlist = p.second;
        for (auto wit = wlist.cbegin(); wit != wlist.cend(); ++wit)
        {
          res.addEdge(ek.from, ek.to, *wit);
        }
      }
    }
    return res;
  }

private:
  HashTable<std::string, bool> vertexSet_;
  List<std::string> vertexList_;
  HashTable<EdgeKey, List<size_t>, EdgeHash, EdgeEqual> edges_;
};

}

#endif
