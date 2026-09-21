#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <stdexcept>
#include "node.hpp"

template< class T >
class Queue
{
public:
  Queue(): head_(nullptr), tail_(nullptr), size_(0) {}
  ~Queue() { clear(); }

  void push(const T& value)
  {
    Node* node = new Node(value);
    if (tail_)
    {
      tail_->next_ = node;
      tail_ = node;
    }
    else
    {
      head_ = tail_ = node;
    }
    ++size_;
  }

  void push(T&& value)
  {
    Node* node = new Node(static_cast<T&&>(value));
    if (tail_)
    {
      tail_->next_ = node;
      tail_ = node;
    }
    else
    {
      head_ = tail_ = node;
    }
    ++size_;
  }

  void pop()
  {
    if (empty()) throw std::underflow_error("Queue underflow");
    Node* tmp = head_;
    head_ = head_->next_;
    if (!head_) tail_ = nullptr;
    delete tmp;
    --size_;
  }

  T& front()
  {
    if (empty()) throw std::underflow_error("Queue empty");
    return head_->value_;
  }

  const T& front() const
  {
    if (empty()) throw std::underflow_error("Queue empty");
    return head_->value_;
  }

  bool empty() const { return size_ == 0; }
  std::size_t size() const { return size_; }

private:
  using Node = detail::Node<T>;
  void clear()
  {
    while (head_)
    {
      Node* tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    tail_ = nullptr;
    size_ = 0;
  }

  Node* head_;
  Node* tail_;
  std::size_t size_;
};

#endif
