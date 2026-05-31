#ifndef MUSORIN_NODE_HPP
#define MUSORIN_NODE_HPP
namespace musorin {
namespace detail {
template< class T >
struct Node {
  T value_;
  Node * next_;
  Node * prev_;
  explicit Node(const T & v):
    value_(v),
    next_(nullptr),
    prev_(nullptr)
  {}
  explicit Node(T && v):
    value_(static_cast< T && >(v)),
    next_(nullptr),
    prev_(nullptr)
  {}
};
}
}
#endif
