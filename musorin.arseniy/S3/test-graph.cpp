#include <iostream>
#include <string>
#include "graph.hpp"

namespace musorin
{

static int passed = 0;
static int failed = 0;

static void check(bool cond, const char * desc)
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

static void testAddVertex()
{
  Graph g;
  g.addVertex("a");
  g.addVertex("b");
  check(g.hasVertex("a"), "addVertex: has a");
  check(g.hasVertex("b"), "addVertex: has b");
  check(!g.hasVertex("c"), "addVertex: no c");
}

static void testAddEdge()
{
  Graph g;
  g.addEdge("x", "y", 10);
  check(g.hasVertex("x") && g.hasVertex("y"), "addEdge: vertices added");
  const auto& edges = g.getEdges();
  EdgeKey key{"x", "y"};
  check(edges.has(key), "addEdge: edge exists");
  const List<size_t>& weights = edges.at(key);
  check(weights.size() == 1, "addEdge: one weight");
  check(weights.front() == 10, "addEdge: weight 10");
}

static void testMultipleEdges()
{
  Graph g;
  g.addEdge("a", "b", 5);
  g.addEdge("a", "b", 15);
  EdgeKey key{"a", "b"};
  const List<size_t>& weights = g.getEdges().at(key);
  check(weights.size() == 2, "multipleEdges: two weights");
  bool has5 = false, has15 = false;
  for (auto it = weights.cbegin(); it != weights.cend(); ++it)
  {
    if (*it == 5) has5 = true;
    if (*it == 15) has15 = true;
  }
  check(has5 && has15, "multipleEdges: weights 5 and 15 present");
}

static void testRemoveEdge()
{
  Graph g;
  g.addEdge("a", "b", 100);
  g.addEdge("a", "b", 200);
  bool removed = g.removeEdge("a", "b", 100);
  check(removed, "removeEdge: removed 100");
  EdgeKey key{"a", "b"};
  const auto& edges = g.getEdges();
  check(edges.has(key), "removeEdge: edge still exists with 200");
  check(edges.at(key).size() == 1, "removeEdge: one weight remains");
  removed = g.removeEdge("a", "b", 200);
  check(removed, "removeEdge: removed 200");
  check(!edges.has(key), "removeEdge: edge completely removed");
}

static void testRemoveNonExisting()
{
  Graph g;
  g.addEdge("a", "b", 10);
  bool removed = g.removeEdge("a", "b", 999);
  check(!removed, "removeNonExisting: weight 999 not found");
  removed = g.removeEdge("x", "y", 10);
  check(!removed, "removeNonExisting: missing vertices");
}

static void testMerge()
{
  Graph a;
  a.addEdge("a", "b", 1);
  a.addEdge("b", "c", 2);

  Graph b;
  b.addEdge("b", "c", 3);
  b.addEdge("c", "d", 4);

  Graph merged = Graph::merge(a, b);
  check(merged.hasVertex("a") && merged.hasVertex("b") &&
        merged.hasVertex("c") && merged.hasVertex("d"), "merge: all vertices present");

  EdgeKey k1{"a", "b"};
  check(merged.getEdges().has(k1), "merge: edge a->b exists");
  check(merged.getEdges().at(k1).size() == 1, "merge: a->b has 1 weight");

  EdgeKey k2{"b", "c"};
  check(merged.getEdges().has(k2), "merge: edge b->c exists");
  check(merged.getEdges().at(k2).size() == 2, "merge: b->c has 2 weights (from both graphs)");
}

static void testExtract()
{
  Graph g;
  g.addEdge("a", "b", 1);
  g.addEdge("b", "c", 2);
  g.addEdge("c", "a", 3);

  List<std::string> verts;
  verts.pushBack("a");
  verts.pushBack("b");

  Graph sub = Graph::extract(g, verts);
  check(sub.hasVertex("a") && sub.hasVertex("b"), "extract: vertices a,b present");
  check(!sub.hasVertex("c"), "extract: vertex c not present");

  EdgeKey keep{"a", "b"};
  check(sub.getEdges().has(keep), "extract: edge a->b present");
  EdgeKey remove{"b", "c"};
  check(!sub.getEdges().has(remove), "extract: edge b->c removed");
}

static void testExtractInvalidVertex()
{
  Graph g;
  g.addVertex("a");
  List<std::string> verts;
  verts.pushBack("z");
  bool except = false;
  try
  {
    Graph sub = Graph::extract(g, verts);
  }
  catch (const std::exception&)
  {
    except = true;
  }
  check(except, "extractInvalidVertex: exception thrown");
}

}

int main()
{
  musorin::testAddVertex();
  musorin::testAddEdge();
  musorin::testMultipleEdges();
  musorin::testRemoveEdge();
  musorin::testRemoveNonExisting();
  musorin::testMerge();
  musorin::testExtract();
  musorin::testExtractInvalidVertex();

  int total = musorin::passed + musorin::failed;
  if (musorin::failed == 0)
  {
    std::cout << "All " << total << " tests passed." << '\n';
    return 0;
  }
  std::cout << musorin::failed << " / " << total << " tests FAILED." << '\n';
  return 1;
}
