#include <iostream>
#include <string>
#include "bstree.hpp"
#include "dataset-ops.hpp"

namespace musorin
{

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

static BSTree<int, std::string> makeTree(std::initializer_list<std::pair<int, std::string>> il)
{
  BSTree<int, std::string> t;
  for (auto& p : il)
  {
    t.push(p.first, p.second);
  }
  return t;
}

static bool treeEquals(const BSTree<int, std::string>& a,
                       const BSTree<int, std::string>& b)
{
  auto itA = a.cbegin();
  auto itB = b.cbegin();
  while (itA != a.cend() && itB != b.cend())
  {
    if ((*itA).first != (*itB).first || (*itA).second != (*itB).second)
      return false;
    ++itA; ++itB;
  }
  return itA == a.cend() && itB == b.cend();
}

static void testComplement()
{
  auto a = makeTree({{1, "one"}, {2, "two"}, {3, "three"}});
  auto b = makeTree({{2, "two"}, {4, "four"}});
  auto c = complementTrees(a, b);
  check(treeEquals(c, makeTree({{1, "one"}, {3, "three"}})), "complement: {1,3}");
  auto d = complementTrees(b, a);
  check(treeEquals(d, makeTree({{4, "four"}})), "complement: {4}");
  auto e = complementTrees(a, a);
  check(e.empty(), "complement self: empty");
}

static void testIntersect()
{
  auto a = makeTree({{1, "a1"}, {2, "a2"}, {3, "a3"}});
  auto b = makeTree({{2, "b2"}, {3, "b3"}, {4, "b4"}});
  auto c = intersectTrees(a, b);
  check(treeEquals(c, makeTree({{2, "a2"}, {3, "a3"}})), "intersect: left priority");
  auto d = intersectTrees(b, a);
  check(treeEquals(d, makeTree({{2, "b2"}, {3, "b3"}})), "intersect: left priority reversed");
}

static void testUnion()
{
  auto a = makeTree({{1, "a1"}, {2, "a2"}});
  auto b = makeTree({{2, "b2"}, {3, "b3"}});
  auto c = unionTrees(a, b);
  check(treeEquals(c, makeTree({{1, "a1"}, {2, "a2"}, {3, "b3"}})), "union: left priority");
  auto d = unionTrees(b, a);
  check(treeEquals(d, makeTree({{1, "a1"}, {2, "b2"}, {3, "b3"}})), "union: left priority reversed");
}

static void testEmptyOperands()
{
  auto empty = BSTree<int, std::string>();
  auto a = makeTree({{1, "one"}});
  check(treeEquals(complementTrees(a, empty), a), "complement with empty right: a");
  check(complementTrees(empty, a).empty(), "complement with empty left: empty");
  check(intersectTrees(a, empty).empty(), "intersect with empty: empty");
  check(treeEquals(unionTrees(a, empty), a), "union with empty: a");
}

} 

int main()
{
  musorin::testComplement();
  musorin::testIntersect();
  musorin::testUnion();
  musorin::testEmptyOperands();

  int total = musorin::passed + musorin::failed;
  if (musorin::failed == 0)
  {
    std::cout << "All " << total << " tests passed." << '\n';
    return 0;
  }
  std::cout << musorin::failed << " / " << total << " tests FAILED." << '\n';
  return 1;
}
