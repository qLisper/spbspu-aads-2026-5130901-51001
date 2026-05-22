#ifndef STACK_HPP
#define STACK_HPP

#include <stdexcept>
#include "node.hpp"

template< class T >
class Stack {
public:
  Stack(): head_(nullptr), size_(0) {}
  ~Stack() { clear(); }

  void push(const T & value) {
    Node * node = new Node(value);
    node->next_ = head_;
    head_ = node;
    ++size_;
  }

  void push(T && value) {
    Node * node = new Node(static_cast<T&&>(value));
    node->next_ = head_;
    head_ = node;
    ++size_;
  }

  void pop() {
    if (empty()) throw std::underflow_error("Stack underflow");
    Node * tmp = head_;
    head_ = head_->next_;
    delete tmp;
    --size_;
  }

  T & top() {
    if (empty()) throw std::underflow_error("Stack empty");
    return head_->value_;
  }

  const T & top() const {
    if (empty()) throw std::underflow_error("Stack empty");
    return head_->value_;
  }

  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

private:
  using Node = detail::Node<T>;
  void clear() {
    while (head_) {
      Node * tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    size_ = 0;
  }

  Node * head_;
  size_t size_;
};

#endif
