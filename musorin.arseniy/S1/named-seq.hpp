#ifndef MUSORIN_NAMED_SEQ_HPP
#define MUSORIN_NAMED_SEQ_HPP
#include <string>
#include "list.hpp"
namespace musorin {
struct NamedSeq {
  std::string name;
  List< std::size_t > nums;
  NamedSeq():
    name(),
    nums()
  {}
  explicit NamedSeq(const std::string & n):
    name(n),
    nums()
  {}
};
}
#endif
