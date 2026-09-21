#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <utility>

template< class T >
class List
{
  template< class K, class V >
  friend class HashMap;

  struct Node
  {
    T data;
    Node* next_;
  };

public:
  class Iterator
  {
    friend class List;
  public:
    Iterator() : node_(nullptr) {}

    bool operator==(const Iterator& other) const { return node_ == other.node_; }
    bool operator!=(const Iterator& other) const { return node_ != other.node_; }

    T& operator*()  const { return node_->data; }
    T* operator->() const { return &node_->data; }

    Iterator& operator++()
    {
      if (node_) node_ = node_->next_;
      return *this;
    }

  private:
    explicit Iterator(Node* node) : node_(node) {}
    Node* node_;
  };

  class ConstIterator
  {
    friend class List;
  public:
    ConstIterator() : node_(nullptr) {}

    bool operator==(const ConstIterator& other) const { return node_ == other.node_; }
    bool operator!=(const ConstIterator& other) const { return node_ != other.node_; }

    const T& operator*()  const { return node_->data; }
    const T* operator->() const { return &node_->data; }

    ConstIterator& operator++()
    {
      if (node_) node_ = node_->next_;
      return *this;
    }

  private:
    explicit ConstIterator(const Node* node) : node_(node) {}
    const Node* node_;
  };

  List() : head_(nullptr), size_(0) {}

  List(const List& other) : head_(nullptr), size_(0)
  {
    copyFrom(other);
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

  bool empty() const { return size_ == 0; }
  std::size_t size() const { return size_; }

  Iterator begin() { return Iterator(head_); }
  Iterator end()   { return Iterator(nullptr); }

  ConstIterator begin() const { return ConstIterator(head_); }
  ConstIterator end()   const { return ConstIterator(nullptr); }

  T* find(const T& value)
  {
    for (Node* n = head_; n; n = n->next_)
      if (n->data == value) return &n->data;
    return nullptr;
  }

  const T* find(const T& value) const
  {
    for (const Node* n = head_; n; n = n->next_)
      if (n->data == value) return &n->data;
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
  Node* head() { return head_; }
  void  setHead(Node* node) { head_ = node; }

  void copyFrom(const List& other)
  {
    if (!other.head_) return;

    head_ = new Node{other.head_->data, nullptr};
    Node* curr = head_;
    Node* src  = other.head_->next_;
    while (src)
    {
      curr->next_ = new Node{src->data, nullptr};
      curr       = curr->next_;
      src        = src->next_;
    }
    size_ = other.size_;
  }

  Node* head_;
  std::size_t size_;
};

#endif
