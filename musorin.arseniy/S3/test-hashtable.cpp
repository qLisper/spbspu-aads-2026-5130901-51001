#include <iostream>
#include <string>
#include "hashtable.hpp"

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
  HashTable<int, int> ht;
  check(ht.empty(), "empty: empty() true");
  check(ht.size() == 0, "empty: size 0");
}

static void testAddHas()
{
  HashTable<int, std::string> ht(5);
  ht.add(1, "one");
  ht.add(2, "two");
  check(ht.has(1), "add: has 1");
  check(ht.has(2), "add: has 2");
  check(!ht.has(3), "add: no 3");
  check(ht.size() == 2, "add: size 2");
}

static void testOverwrite()
{
  HashTable<int, int> ht(3);
  ht.add(1, 10);
  ht.add(1, 20);
  check(ht.at(1) == 20, "overwrite: value 20");
  check(ht.size() == 1, "overwrite: size 1");
}

static void testDrop()
{
  HashTable<int, int> ht(4);
  ht.add(10, 100);
  ht.add(20, 200);
  int v = ht.drop(10);
  check(v == 100, "drop: return 100");
  check(!ht.has(10), "drop: no 10");
  check(ht.size() == 1, "drop: size 1");
  bool except = false;
  try
  {
    ht.drop(100);
  }
  catch (const std::exception&)
  {
    except = true;
  }
  check(except, "drop: exception for missing key");
}

static void testFull()
{
  HashTable<int, int> ht(2);
  ht.add(1, 1);
  ht.add(2, 2);
  bool full = false;
  try
  {
    ht.add(3, 3);
  }
  catch (const std::exception&)
  {
    full = true;
  }
  check(full, "full: exception on full table");
}

static void testRehash()
{
  HashTable<int, int> ht(2);
  ht.add(1, 10);
  ht.add(2, 20);
  ht.rehash(5);
  check(ht.size() == 2, "rehash: size preserved");
  check(ht.has(1) && ht.has(2), "rehash: keys present");
  check(ht.at(1) == 10, "rehash: value 1");
  check(ht.at(2) == 20, "rehash: value 2");
}

static void testIterators()
{
  HashTable<int, int> ht(5);
  ht.add(5, 50);
  ht.add(3, 30);
  ht.add(7, 70);
  size_t count = 0;
  int sum = 0;
  for (auto it = ht.begin(); it != ht.end(); ++it)
  {
    ++count;
    sum += it->second;
  }
  check(count == 3, "iter: count 3");
  check(sum == 150, "iter: sum 150");
}

}

int main()
{
  musorin::testEmpty();
  musorin::testAddHas();
  musorin::testOverwrite();
  musorin::testDrop();
  musorin::testFull();
  musorin::testRehash();
  musorin::testIterators();

  int total = musorin::passed + musorin::failed;
  if (musorin::failed == 0)
  {
    std::cout << "All " << total << " tests passed." << '\n';
    return 0;
  }
  std::cout << musorin::failed << " / " << total << " tests FAILED." << '\n';
  return 1;
}
