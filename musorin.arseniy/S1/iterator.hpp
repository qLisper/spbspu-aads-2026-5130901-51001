#ifndef MUSORIN_ITERATOR_HPP
#define MUSORIN_ITERATOR_HPP
#include "node.hpp"
namespace musorin {
template< class T >
class List;
template< class T >
class LCIter;
template< class T >
class LIter {
  friend class List< T >;
  friend class LCIter< T >;
public:
  LIter():
    node_(nullptr)
  {}
  T & operator*() const
  {
    return node_->value_;
  }
  T * operator->() const
  {
    return &(node_->value_);
  }
  LIter & operator++()
  {
    node_ = node_->next_;
    return *this;
  }
  LIter operator++(int)
  {
    LIter tmp(*this);
    node_ = node_->next_;
    return tmp;
  }
  LIter & operator--()
  {
    node_ = node_->prev_;
    return *this;
  }
  LIter operator--(int)
  {
    LIter tmp(*this);
    node_ = node_->prev_;
    return tmp;
  }
  bool operator==(const LIter & rhs) const
  {
    return node_ == rhs.node_;
  }
  bool operator!=(const LIter & rhs) const
  {
    return node_ != rhs.node_;
  }
private:
  using Node = detail::Node< T >;
  explicit LIter(Node * node):
    node_(node)
  {}
  Node * node_;
};
template< class T >
class LCIter {
  friend class List< T >;
public:
  LCIter():
    node_(nullptr)
  {}
  LCIter(const LIter< T > & other):
    node_(other.node_)
  {}
  const T & operator*() const
  {
    return node_->value_;
  }
  const T * operator->() const
  {
    return &(node_->value_);
  }
  LCIter & operator++()
  {
    node_ = node_->next_;
    return *this;
  }
  LCIter operator++(int)
  {
    LCIter tmp(*this);
    node_ = node_->next_;
    return tmp;
  }
  LCIter & operator--()
  {
    node_ = node_->prev_;
    return *this;
  }
  LCIter operator--(int)
  {
    LCIter tmp(*this);
    node_ = node_->prev_;
    return tmp;
  }
  bool operator==(const LCIter & rhs) const
  {
    return node_ == rhs.node_;
  }
  bool operator!=(const LCIter & rhs) const
  {
    return node_ != rhs.node_;
  }
private:
  using Node = detail::Node< T >;
  explicit LCIter(const Node * node):
    node_(node)
  {}
  const Node * node_;
};
}
#endif
