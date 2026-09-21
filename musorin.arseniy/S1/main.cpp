#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include "list.hpp"
#include "named-seq.hpp"
namespace {
using musorin::List;
using musorin::NamedSeq;
using musorin::LIter;
using musorin::LCIter;
void readInput(std::istream & in, List< NamedSeq > & seqs)
{
  std::string name;
  while (in >> name) {
    NamedSeq seq(name);
    std::size_t num = 0;
    while (in >> num) {
      seq.nums.pushBack(num);
    }
    seqs.pushBack(static_cast< NamedSeq && >(seq));
    in.clear();
  }
}
void printNames(std::ostream & out, const List< NamedSeq > & seqs)
{
  bool first = true;
  for (auto it = seqs.cbegin(); it != seqs.cend(); ++it) {
    if (!first) {
      out << ' ';
    }
    out << it->name;
    first = false;
  }
  out << '\n';
}
std::size_t checkedAdd(std::size_t a, std::size_t b)
{
  if (std::numeric_limits< std::size_t >::max() - a < b) {
    throw std::overflow_error("overflow");
  }
  return a + b;
}
bool collectRow(List< LIter< std::size_t > > & iters,
    List< NamedSeq > & seqs,
    List< std::size_t > & rowVals)
{
  auto iterIt = iters.begin();
  auto seqIt = seqs.begin();
  while (seqIt != seqs.end()) {
    if (*iterIt != seqIt->nums.end()) {
      rowVals.pushBack(**iterIt);
      ++(*iterIt);
    }
    ++seqIt;
    ++iterIt;
  }
  return !rowVals.empty();
}
void printRow(std::ostream & out, const List< std::size_t > & row)
{
  bool first = true;
  for (auto it = row.cbegin(); it != row.cend(); ++it) {
    if (!first) {
      out << ' ';
    }
    out << *it;
    first = false;
  }
  out << '\n';
}
std::size_t sumRow(const List< std::size_t > & row)
{
  std::size_t s = 0;
  for (auto it = row.cbegin(); it != row.cend(); ++it) {
    s = checkedAdd(s, *it);
  }
  return s;
}
void processAndPrint(std::ostream & out, List< NamedSeq > & seqs,
    List< std::size_t > & rowSums)
{
  List< LIter< std::size_t > > iters;
  for (auto it = seqs.begin(); it != seqs.end(); ++it) {
    iters.pushBack(it->nums.begin());
  }
  List< std::size_t > rowVals;
  while (collectRow(iters, seqs, rowVals)) {
    printRow(out, rowVals);
    rowSums.pushBack(sumRow(rowVals));
    rowVals.clear();
  }
}
void printSums(std::ostream & out, const List< std::size_t > & sums)
{
  bool first = true;
  for (auto it = sums.cbegin(); it != sums.cend(); ++it) {
    if (!first) {
      out << ' ';
    }
    out << *it;
    first = false;
  }
  out << '\n';
}
}
int main()
{
  musorin::List< musorin::NamedSeq > seqs;
  readInput(std::cin, seqs);
  if (seqs.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }
  printNames(std::cout, seqs);
  musorin::List< std::size_t > rowSums;
  try {
    processAndPrint(std::cout, seqs, rowSums);
  } catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  if (rowSums.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }
  printSums(std::cout, rowSums);
  return 0;
}
