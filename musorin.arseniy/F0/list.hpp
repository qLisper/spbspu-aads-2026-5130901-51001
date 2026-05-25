#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>

template< class T >
class List
{
public:
  List():
    head_(nullptr),
    size_(0)
  {}

  ~List()
  {
    clear();
  }

  void pushFront(const T& value)
  {
    Node* node = new Node{value, head_};
    head_ = node;
    ++size_;
  }

  void popFront()
  {
    if (head_)
    {
      Node* tmp = head_;
      head_ = head_->next_;
      delete tmp;
      --size_;
    }
  }

  bool empty() const
  {
    return size_ == 0;
  }

  std::size_t size() const
  {
    return size_;
  }

  T* find(const T& value)
  {
    Node* curr = head_;
    while (curr)
    {
      if (curr->data == value)
      {
        return &curr->data;
      }
      curr = curr->next_;
    }
    return nullptr;
  }

  void clear()
  {
    while (head_)
    {
      Node* tmp = head_;
      head_ = head_->next_;
      delete tmp;
    }
    size_ = 0;
  }

private:
  struct Node
  {
    T data;
    Node* next_;
  };

  Node* head_;
  size_t size_;
};

#endif
