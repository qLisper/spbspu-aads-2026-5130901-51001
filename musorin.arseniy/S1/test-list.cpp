#include <iostream>
#include <string>
#include "list.hpp"
namespace musorin {
static int passed = 0;
static int failed = 0;
static void check(bool cond, const char * desc)
{
  if (cond) {
    ++passed;
  } else {
    ++failed;
    std::cerr << "FAIL: " << desc << '\n';
  }
}
static void testEmpty()
{
  List< int > lst;
  check(lst.empty(), "empty: empty() true");
  check(lst.size() == 0, "empty: size 0");
  check(lst.begin() == lst.end(), "empty: begin == end");
}
static void testPushBack()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);
  check(lst.size() == 3, "pushBack: size 3");
  check(lst.front() == 1, "pushBack: front 1");
  check(lst.back() == 3, "pushBack: back 3");
  auto it = lst.begin();
  check(*it == 1, "pushBack iter: 1"); ++it;
  check(*it == 2, "pushBack iter: 2"); ++it;
  check(*it == 3, "pushBack iter: 3"); ++it;
  check(it == lst.end(), "pushBack iter: end");
}
static void testPushFront()
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);
  check(lst.front() == 1, "pushFront: front 1");
  check(lst.back() == 3, "pushFront: back 3");
  check(lst.size() == 3, "pushFront: size 3");
}
static void testPopFront()
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);
  lst.popFront();
  check(lst.front() == 20, "popFront: front 20");
  check(lst.size() == 2, "popFront: size 2");
  lst.popFront();
  lst.popFront();
  check(lst.empty(), "popFront: empty");
}
static void testPopBack()
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);
  lst.popBack();
  check(lst.back() == 20, "popBack: back 20");
  check(lst.size() == 2, "popBack: size 2");
  lst.popBack();
  lst.popBack();
  check(lst.empty(), "popBack: empty");
}
static void testBidirectional()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);
  auto it = lst.begin();
  ++it; ++it;
  check(*it == 3, "bidir: forward to 3");
  --it;
  check(*it == 2, "bidir: back to 2");
  --it;
  check(*it == 1, "bidir: back to 1");
}
static void testCopyCtor()
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);
  List< int > b(a);
  check(b.size() == 2, "copy ctor: size 2");
  check(b.front() == 1, "copy ctor: front 1");
  b.popFront();
  check(a.size() == 2, "copy ctor: a unchanged");
}
static void testMoveCtor()
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);
  List< int > b(static_cast< List< int > && >(a));
  check(b.size() == 2, "move ctor: size 2");
  check(a.empty(), "move ctor: a empty");
}
static void testInsert()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(3);
  auto it = lst.begin();
  ++it;
  lst.insert(it, 2);
  check(lst.size() == 3, "insert: size 3");
  auto cur = lst.begin();
  check(*cur == 1, "insert: 1"); ++cur;
  check(*cur == 2, "insert: 2"); ++cur;
  check(*cur == 3, "insert: 3");
}
static void testErase()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);
  auto it = lst.begin();
  ++it;
  lst.erase(it);
  check(lst.size() == 2, "erase: size 2");
  auto cur = lst.begin();
  check(*cur == 1, "erase: 1"); ++cur;
  check(*cur == 3, "erase: 3");
}
static void testSwap()
{
  List< int > a;
  a.pushBack(1);
  List< int > b;
  b.pushBack(10);
  b.pushBack(20);
  a.swap(b);
  check(a.size() == 2, "swap: a size 2");
  check(b.size() == 1, "swap: b size 1");
  check(a.front() == 10, "swap: a front 10");
  check(b.front() == 1, "swap: b front 1");
}
static void testClear()
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.clear();
  check(lst.empty(), "clear: empty");
  lst.pushBack(42);
  check(lst.front() == 42, "clear: reuse");
}
static void testString()
{
  List< std::string > lst;
  lst.pushBack("hello");
  lst.pushBack("world");
  check(lst.front() == "hello", "string: front");
  check(lst.back() == "world", "string: back");
}
}
int main()
{
  musorin::testEmpty();
  musorin::testPushBack();
  musorin::testPushFront();
  musorin::testPopFront();
  musorin::testPopBack();
  musorin::testBidirectional();
  musorin::testCopyCtor();
  musorin::testMoveCtor();
  musorin::testInsert();
  musorin::testErase();
  musorin::testSwap();
  musorin::testClear();
  musorin::testString();
  int total = musorin::passed + musorin::failed;
  if (musorin::failed == 0) {
    std::cout << "All " << total << " tests passed." << '\n';
    return 0;
  }
  std::cout << musorin::failed << " / " << total << " tests FAILED." << '\n';
  return 1;
}
