#ifndef MUSORIN_LIST_HPP
#define MUSORIN_LIST_HPP
#include <cstddef>
#include "iterator.hpp"
#include "node.hpp"
namespace musorin {
template< class T >
class List {
public:
  using iterator = LIter< T >;
  using const_iterator = LCIter< T >;
  List():
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {}
  List(const List & other):
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {
    try {
      for (auto it = other.cbegin(); it != other.cend(); ++it) {
        pushBack(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }
  List(List && other) noexcept:
    head_(other.head_),
    tail_(other.tail_),
    size_(other.size_)
  {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }
  ~List()
  {
    clear();
  }
  List & operator=(const List & other)
  {
    if (this != &other) {
      List tmp(other);
      swap(tmp);
    }
    return *this;
  }
  List & operator=(List && other) noexcept
  {
    if (this != &other) {
      clear();
      head_ = other.head_;
      tail_ = other.tail_;
      size_ = other.size_;
      other.head_ = nullptr;
      other.tail_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }
  iterator begin()
  {
    return iterator(head_);
  }
  iterator end()
  {
    return iterator(nullptr);
  }
  const_iterator begin() const
  {
    return const_iterator(head_);
  }
  const_iterator end() const
  {
    return const_iterator(nullptr);
  }
  const_iterator cbegin() const
  {
    return const_iterator(head_);
  }
  const_iterator cend() const
  {
    return const_iterator(nullptr);
  }
  T & front()
  {
    return head_->value_;
  }
  const T & front() const
  {
    return head_->value_;
  }
  T & back()
  {
    return tail_->value_;
  }
  const T & back() const
  {
    return tail_->value_;
  }
  bool empty() const
  {
    return size_ == 0;
  }
  std::size_t size() const
  {
    return size_;
  }
  void pushFront(const T & value)
  {
    Node * node = new Node(value);
    linkFront(node);
  }
  void pushFront(T && value)
  {
    Node * node = new Node(static_cast< T && >(value));
    linkFront(node);
  }
  void pushBack(const T & value)
  {
    Node * node = new Node(value);
    linkBack(node);
  }
  void pushBack(T && value)
  {
    Node * node = new Node(static_cast< T && >(value));
    linkBack(node);
  }
  void popFront()
  {
    Node * tmp = head_;
    head_ = head_->next_;
    if (head_ != nullptr) {
      head_->prev_ = nullptr;
    } else {
      tail_ = nullptr;
    }
    delete tmp;
    --size_;
  }
  void popBack()
  {
    Node * tmp = tail_;
    tail_ = tail_->prev_;
    if (tail_ != nullptr) {
      tail_->next_ = nullptr;
    } else {
      head_ = nullptr;
    }
    delete tmp;
    --size_;
  }
  iterator insert(const_iterator pos, const T & value)
  {
    if (pos.node_ == nullptr) {
      pushBack(value);
      return iterator(tail_);
    }
    Node * next = const_cast< Node * >(pos.node_);
    Node * prev = next->prev_;
    Node * node = new Node(value);
    node->next_ = next;
    node->prev_ = prev;
    if (prev != nullptr) {
      prev->next_ = node;
    } else {
      head_ = node;
    }
    next->prev_ = node;
    ++size_;
    return iterator(node);
  }
  iterator erase(iterator pos)
  {
    if (pos == end()) {
      return end();
    }
    Node * curr = pos.node_;
    Node * nextNode = curr->next_;
    if (curr->prev_ != nullptr) {
      curr->prev_->next_ = curr->next_;
    } else {
      head_ = curr->next_;
    }
    if (curr->next_ != nullptr) {
      curr->next_->prev_ = curr->prev_;
    } else {
      tail_ = curr->prev_;
    }
    delete curr;
    --size_;
    return iterator(nextNode);
  }
  void clear()
  {
    while (head_ != nullptr) {
      Node * tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    tail_ = nullptr;
    size_ = 0;
  }
  void swap(List & other) noexcept
  {
    Node * tmpH = head_;
    Node * tmpT = tail_;
    std::size_t tmpS = size_;
    head_ = other.head_;
    tail_ = other.tail_;
    size_ = other.size_;
    other.head_ = tmpH;
    other.tail_ = tmpT;
    other.size_ = tmpS;
  }
private:
  using Node = detail::Node< T >;
  void linkFront(Node * node)
  {
    if (head_ == nullptr) {
      head_ = node;
      tail_ = node;
    } else {
      node->next_ = head_;
      head_->prev_ = node;
      head_ = node;
    }
    ++size_;
  }
  void linkBack(Node * node)
  {
    if (tail_ == nullptr) {
      head_ = node;
      tail_ = node;
    } else {
      node->prev_ = tail_;
      tail_->next_ = node;
      tail_ = node;
    }
    ++size_;
  }
  Node * head_;
  Node * tail_;
  std::size_t size_;
};
}
#endif
