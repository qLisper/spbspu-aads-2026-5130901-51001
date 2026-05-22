#ifndef NODE_HPP
#define NODE_HPP

namespace detail {
template< class T >
struct Node {
  T value_;
  Node * next_;
  explicit Node(const T & v):
    value_(v),
    next_(nullptr)
  {}
  explicit Node(T && v):
    value_(static_cast< T && >(v)),
    next_(nullptr)
  {}
};
}

#endif
