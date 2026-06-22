#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>

template< class T >
class List
{
  template < class K, class V >
  friend class HashMap;
public:
  List():
    head_(nullptr),
    size_(0)
  {}

  ~List()
  {
    clear();
  }
  List(const List& other) : head_(nullptr), size_(0)
  {
    if (!other.head_)
    {
      return;
    }  
    head_ = new Node{other.head_->data, nullptr};
    Node* curr = head_;
    Node* src = other.head_->next_;
    while (src)
    {
      curr->next_ = new Node{src->data, nullptr};
      curr = curr->next_;
      src = src->next_;
    }
    size_ = other.size_;
  }

  List& operator=(const List& other)
  {
    if (this != &other)
    {
      List tmp(other);
      std::swap(head_, tmp.head_);
      std::swap(size_, tmp.size_);
    }
    return *this;
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

  size_t size() const
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
    Node* head()
  {
    return head_;
  }

private:
  struct Node
  {
    T data;
    Node* next_;
  };

  void setHead(Node* node)
  {
    head_ = node;
  }

  Node* head_;
  size_t size_;
};

#endif
