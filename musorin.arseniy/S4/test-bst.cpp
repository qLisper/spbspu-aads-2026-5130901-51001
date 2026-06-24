#include <iostream>
#include <string>
#include "bstree.hpp"

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

static void testEmpty()
{
  BSTree<int, int> tree;
  check(tree.empty(), "empty: empty() true");
  check(tree.size() == 0, "empty: size 0");
  check(tree.begin() == tree.end(), "empty: begin == end");
}

static void testPushAndGet()
{
  BSTree<int, std::string> tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(7, "seven");
  check(tree.has(5), "push: has 5");
  check(tree.has(3), "push: has 3");
  check(tree.has(7), "push: has 7");
  check(!tree.has(10), "push: no 10");
  check(tree.size() == 3, "push: size 3");
  check(tree.get(3) == "three", "push: get 3");
}

static void testOverwrite()
{
  BSTree<int, int> tree;
  tree.push(1, 100);
  tree.push(1, 200);
  check(tree.get(1) == 200, "overwrite: value 200");
  check(tree.size() == 1, "overwrite: size 1");
}

static void testIteration()
{
  BSTree<int, std::string> tree;
  tree.push(2, "b");
  tree.push(1, "a");
  tree.push(3, "c");
  auto it = tree.begin();
  check((*it).first == 1 && (*it).second == "a", "iter: 1 a"); ++it;
  check((*it).first == 2 && (*it).second == "b", "iter: 2 b"); ++it;
  check((*it).first == 3 && (*it).second == "c", "iter: 3 c"); ++it;
  check(it == tree.end(), "iter: end");
}

static void testDrop()
{
  BSTree<int, int> tree;
  tree.push(10, 100);
  tree.push(5, 50);
  tree.push(15, 150);
  tree.push(3, 30);
  tree.push(7, 70);
  
  int v = tree.drop(3);
  check(v == 30, "drop leaf: return 30");
  check(!tree.has(3), "drop leaf: no 3");
  check(tree.size() == 4, "drop leaf: size 4");
  
  tree.drop(5);
  check(!tree.has(5), "drop single child: no 5");
  check(tree.has(7), "drop single child: child 7 exists");
  check(tree.size() == 3, "drop single child: size 3");

  tree.drop(10);
  check(!tree.has(10), "drop two children: no 10");
  check(tree.has(15) && tree.has(7), "drop two children: children exist");
  check(tree.size() == 2, "drop two children: size 2");

  bool except = false;
  try
  {
    tree.drop(100);
  }
  catch (const std::exception&)
  {
    except = true;
  }
  check(except, "drop missing: exception");
}

static void testCopy()
{
  BSTree<int, std::string> a;
  a.push(1, "one");
  a.push(2, "two");
  BSTree<int, std::string> b(a);
  check(b.size() == 2, "copy ctor: size 2");
  check(b.get(1) == "one", "copy ctor: value ok");
  b.push(3, "three");
  check(a.size() == 2, "copy ctor: original unchanged");
}

static void testMove()
{
  BSTree<int, int> a;
  a.push(1, 10);
  a.push(2, 20);
  BSTree<int, int> b(static_cast<BSTree<int, int>&&>(a));
  check(b.size() == 2, "move ctor: size 2");
  check(a.empty(), "move ctor: a empty");
}

static void testHeight()
{
  BSTree<int, int> tree;
  check(tree.height() == 0, "height: empty 0");
  tree.push(5, 1);
  tree.push(3, 2);
  tree.push(7, 3);
  tree.push(2, 4);
  check(tree.height() == 3, "height: tree 3");
  auto it = tree.find(3);
  check(tree.height(it) == 2, "height: subtree 2");
}

static void testRotations()
{
  BSTree<int, int> tree;
  tree.push(1, 10);
  tree.push(2, 20);
  tree.push(3, 30);
  auto it = tree.find(1);
  tree.rotateLeft(it);  
  check(tree.begin().node_->key == 1, "rotate left: root->left == 1");
  check(tree.find(2).node_->left != nullptr && tree.find(2).node_->right != nullptr, "rotate left: 2 has both children");
}

}

int main()
{
  musorin::testEmpty();
  musorin::testPushAndGet();
  musorin::testOverwrite();
  musorin::testIteration();
  musorin::testDrop();
  musorin::testCopy();
  musorin::testMove();
  musorin::testHeight();
  musorin::testRotations();

  int total = musorin::passed + musorin::failed;
  if (musorin::failed == 0)
  {
    std::cout << "All " << total << " tests passed." << '\n';
    return 0;
  }
  std::cout << musorin::failed << " / " << total << " tests FAILED." << '\n';
  return 1;
}
