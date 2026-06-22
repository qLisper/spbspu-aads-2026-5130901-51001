#ifndef MUSORIN_BST_ITERATOR_HPP
#define MUSORIN_BST_ITERATOR_HPP

#include "bst-node.hpp"

namespace musorin
{

template< class Key, class Value, class Compare >
class BSTree;

template< class Key, class Value >
class BSTIterator
{
  friend class BSTree<Key, Value, std::less<Key>>;
  template< class K, class V, class C >
  friend class BSTree;

public:
  BSTIterator():
    node_(nullptr)
  {}

  std::pair<const Key&, Value&> operator*() const
  {
    return { node_->key, node_->value };
  }

  std::pair<const Key&, Value&>* operator->() const
  {
    return nullptr;
  }

  BSTIterator& operator++()
  {
    if (node_ == nullptr) return *this;
    if (node_->right != nullptr)
    {
      node_ = node_->right;
      while (node_->left != nullptr)
      {
        node_ = node_->left;
      }
    }
    else
    {
      BSTNode<Key, Value>* parent = node_->parent;
      while (parent != nullptr && node_ == parent->right)
      {
        node_ = parent;
        parent = parent->parent;
      }
      node_ = parent;
    }
    return *this;
  }

  BSTIterator operator++(int)
  {
    BSTIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  BSTIterator& operator--()
  {
    if (node_ == nullptr)
    {
      return *this;
    }
    if (node_->left != nullptr)
    {
      node_ = node_->left;
      while (node_->right != nullptr)
      {
        node_ = node_->right;
      }
    }
    else
    {
      BSTNode<Key, Value>* parent = node_->parent;
      while (parent != nullptr && node_ == parent->left)
      {
        node_ = parent;
        parent = parent->parent;
      }
      node_ = parent;
    }
    return *this;
  }

  BSTIterator operator--(int)
  {
    BSTIterator tmp(*this);
    --(*this);
    return tmp;
  }

  bool operator==(const BSTIterator& rhs) const
  {
    return node_ == rhs.node_;
  }

  bool operator!=(const BSTIterator& rhs) const
  {
    return node_ != rhs.node_;
  }

private:
  explicit BSTIterator(BSTNode<Key, Value>* node):
    node_(node)
  {}

  BSTNode<Key, Value>* node_;
};

template< class Key, class Value >
class BSTConstIterator
{
  friend class BSTree<Key, Value, std::less<Key>>;
  template< class K, class V, class C >
  friend class BSTree;

public:
  BSTConstIterator():
    node_(nullptr)
  {}

  BSTConstIterator(const BSTIterator<Key, Value>& it):
    node_(it.node_)
  {}

  std::pair<const Key&, const Value&> operator*() const
  {
    return { node_->key, node_->value };
  }

  const std::pair<const Key&, const Value&>* operator->() const
  {
    return nullptr;
  }

  BSTConstIterator& operator++()
  {
    if (node_ == nullptr) return *this;
    if (node_->right != nullptr)
    {
      node_ = node_->right;
      while (node_->left != nullptr)
      {
        node_ = node_->left;
      }
    }
    else
    {
      BSTNode<Key, Value>* parent = node_->parent;
      while (parent != nullptr && node_ == parent->right)
      {
        node_ = parent;
        parent = parent->parent;
      }
      node_ = parent;
    }
    return *this;
  }

  BSTConstIterator operator++(int)
  {
    BSTConstIterator tmp(*this);
    ++(*this);
    return tmp;
  }

  BSTConstIterator& operator--()
  {
    if (node_ == nullptr)
    {
      return *this;
    }
    if (node_->left != nullptr)
    {
      node_ = node_->left;
      while (node_->right != nullptr)
      {
        node_ = node_->right;
      }
    }
    else
    {
      BSTNode<Key, Value>* parent = node_->parent;
      while (parent != nullptr && node_ == parent->left)
      {
        node_ = parent;
        parent = parent->parent;
      }
      node_ = parent;
    }
    return *this;
  }

  BSTConstIterator operator--(int)
  {
    BSTConstIterator tmp(*this);
    --(*this);
    return tmp;
  }

  bool operator==(const BSTConstIterator& rhs) const
  {
    return node_ == rhs.node_;
  }

  bool operator!=(const BSTConstIterator& rhs) const
  {
    return node_ != rhs.node_;
  }

private:
  explicit BSTConstIterator(const BSTNode<Key, Value>* node):
    node_(node)
  {}

  const BSTNode<Key, Value>* node_;
};

}

#endif
