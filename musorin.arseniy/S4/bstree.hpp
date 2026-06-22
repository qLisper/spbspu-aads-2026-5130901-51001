#ifndef MUSORIN_BSTREE_HPP
#define MUSORIN_BSTREE_HPP

#include <stdexcept>
#include <functional>
#include "bst-node.hpp"
#include "bst-iterator.hpp"

namespace musorin
{

template< class Key, class Value, class Compare = std::less<Key> >
class BSTree
{
public:
  using iterator = BSTIterator<Key, Value>;
  using const_iterator = BSTConstIterator<Key, Value>;

  friend class BSTIterator<Key, Value>;
  friend class BSTConstIterator<Key, Value>;

  BSTree():
    root_(nullptr),
    size_(0)
  {}

  ~BSTree()
  {
    clear();
  }

  BSTree(const BSTree& other):
    root_(nullptr),
    size_(0)
  {
    root_ = copyTree(other.root_, nullptr);
    size_ = other.size_;
  }

  BSTree(BSTree&& other) noexcept:
    root_(other.root_),
    size_(other.size_)
  {
    other.root_ = nullptr;
    other.size_ = 0;
  }

  BSTree& operator=(const BSTree& other)
  {
    if (this != &other)
    {
      BSTree tmp(other);
      swap(tmp);
    }
    return *this;
  }

  BSTree& operator=(BSTree&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      root_ = other.root_;
      size_ = other.size_;
      other.root_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  iterator push(const Key& k, const Value& v)
  {
    BSTNode<Key, Value>* newNode = new BSTNode<Key, Value>(k, v);
    if (root_ == nullptr)
    {
      root_ = newNode;
      ++size_;
      return iterator(root_);
    }

    BSTNode<Key, Value>* current = root_;
    BSTNode<Key, Value>* parent = nullptr;
    while (current != nullptr)
    {
      parent = current;
      if (Compare()(k, current->key))
      {
        current = current->left;
      }
      else if (Compare()(current->key, k))
      {
        current = current->right;
      }
      else
      {
        delete newNode;
        current->value = v;
        return iterator(current);
      }
    }

    if (Compare()(k, parent->key))
    {
      parent->left = newNode;
    }
    else
    {
      parent->right = newNode;
    }
    newNode->parent = parent;
    ++size_;
    return iterator(newNode);
  }

  Value& get(const Key& k)
  {
    BSTNode<Key, Value>* node = findNode(k);
    if (node == nullptr)
    {
      throw std::runtime_error("Key not found");
    }
    return node->value;
  }

  const Value& get(const Key& k) const
  {
    const BSTNode<Key, Value>* node = findNode(k);
    if (node == nullptr)
    {
      throw std::runtime_error("Key not found");
    }
    return node->value;
  }

  Value drop(const Key& k)
  {
    BSTNode<Key, Value>* node = findNode(k);
    if (node == nullptr)
    {
      throw std::runtime_error("Key not found");
    }
    Value val = node->value;
    removeNode(node);
    --size_;
    return val;
  }

  bool has(const Key& k) const
  {
    return findNode(k) != nullptr;
  }

  iterator find(const Key& k)
  {
    return iterator(findNode(k));
  }

  const_iterator find(const Key& k) const
  {
    return const_iterator(findNode(k));
  }

  iterator begin()
  {
    BSTNode<Key, Value>* node = root_;
    if (node != nullptr)
    {
      while (node->left != nullptr)
      {
        node = node->left;
      }
    }
    return iterator(node);
  }

  iterator end()
  {
    return iterator(nullptr);
  }

  const_iterator begin() const
  {
    const BSTNode<Key, Value>* node = root_;
    if (node != nullptr)
    {
      while (node->left != nullptr)
      {
        node = node->left;
      }
    }
    return const_iterator(node);
  }

  const_iterator end() const
  {
    return const_iterator(nullptr);
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator cend() const
  {
    return end();
  }

  size_t size() const
  {
    return size_;
  }

  bool empty() const
  {
    return size_ == 0;
  }

  void clear()
  {
    deleteTree(root_);
    root_ = nullptr;
    size_ = 0;
  }

  void swap(BSTree& other) noexcept
  {
    BSTNode<Key, Value>* tmpRoot = root_;
    root_ = other.root_;
    other.root_ = tmpRoot;

    size_t tmpSize = size_;
    size_ = other.size_;
    other.size_ = tmpSize;
  }

  const_iterator rotateLeft(const_iterator it)
  {
    BSTNode<Key, Value>* node = const_cast<BSTNode<Key, Value>*>(it.node_);
    if (node == nullptr || node->right == nullptr)
    {
      return it;
    }

    BSTNode<Key, Value>* r = node->right;
    BSTNode<Key, Value>* rl = r->left;
    BSTNode<Key, Value>* p = node->parent;

    r->left = node;
    node->parent = r;
    node->right = rl;
    if (rl != nullptr)
    {
      rl->parent = node;
    }
    r->parent = p;
    if (p == nullptr)
    {
      root_ = r;
    }
    else if (p->left == node)
    {
      p->left = r;
    }
    else
    {
      p->right = r;
    }

    return const_iterator(r);
  }

  const_iterator rotateRight(const_iterator it)
  {
    BSTNode<Key, Value>* node = const_cast<BSTNode<Key, Value>*>(it.node_);
    if (node == nullptr || node->left == nullptr)
    {
      return it;
    }

    BSTNode<Key, Value>* l = node->left;
    BSTNode<Key, Value>* lr = l->right;
    BSTNode<Key, Value>* p = node->parent;

    l->right = node;
    node->parent = l;
    node->left = lr;
    if (lr != nullptr)
    {
      lr->parent = node;
    }
    l->parent = p;
    if (p == nullptr)
    {
      root_ = l;
    }
    else if (p->left == node)
    {
      p->left = l;
    }
    else
    {
      p->right = l;
    }

    return const_iterator(l);
  }

  const_iterator rotateLargeLeft(const_iterator it)
  {
    BSTNode<Key, Value>* node = const_cast<BSTNode<Key, Value>*>(it.node_);
    if (node == nullptr) return it;
    if (node->right != nullptr)
    {
      rotateRight(const_iterator(node->right));
    }
    return rotateLeft(const_iterator(node));
  }

  const_iterator rotateLargeRight(const_iterator it)
  {
    BSTNode<Key, Value>* node = const_cast<BSTNode<Key, Value>*>(it.node_);
    if (node == nullptr) return it;
    if (node->left != nullptr)
    {
      rotateLeft(const_iterator(node->left));
    }
    return rotateRight(const_iterator(node));
  }

  size_t height(const_iterator it) const
  {
    return nodeHeight(it.node_);
  }

  size_t height() const
  {
    return nodeHeight(root_);
  }

private:
  BSTNode<Key, Value>* root_;
  size_t size_;

  size_t nodeHeight(const BSTNode<Key, Value>* node) const
  {
    if (node == nullptr) return 0;
    size_t leftH = nodeHeight(node->left);
    size_t rightH = nodeHeight(node->right);
    return 1 + (leftH > rightH ? leftH : rightH);
  }

  BSTNode<Key, Value>* findNode(const Key& k) const
  {
    BSTNode<Key, Value>* current = root_;
    while (current != nullptr)
    {
      if (Compare()(k, current->key))
      {
        current = current->left;
      }
      else if (Compare()(current->key, k))
      {
        current = current->right;
      }
      else
      {
        return current;
      }
    }
    return nullptr;
  }

  void removeNode(BSTNode<Key, Value>* node)
  {
    if (node->left != nullptr && node->right != nullptr)
    {
      BSTNode<Key, Value>* successor = node->right;
      while (successor->left != nullptr)
      {
        successor = successor->left;
      }
      node->key = successor->key;
      node->value = successor->value;
      removeNode(successor);
      return;
    }

    BSTNode<Key, Value>* child = (node->left != nullptr) ? node->left : node->right;
    if (child != nullptr)
    {
      child->parent = node->parent;
    }

    if (node->parent == nullptr)
    {
      root_ = child;
    }
    else if (node == node->parent->left)
    {
      node->parent->left = child;
    }
    else
    {
      node->parent->right = child;
    }

    delete node;
  }

  BSTNode<Key, Value>* copyTree(BSTNode<Key, Value>* src, BSTNode<Key, Value>* parent)
  {
    if (src == nullptr) return nullptr;
    BSTNode<Key, Value>* newNode = new BSTNode<Key, Value>(src->key, src->value);
    newNode->parent = parent;
    newNode->left = copyTree(src->left, newNode);
    newNode->right = copyTree(src->right, newNode);
    return newNode;
  }

  void deleteTree(BSTNode<Key, Value>* node)
  {
    if (node != nullptr)
    {
      deleteTree(node->left);
      deleteTree(node->right);
      delete node;
    }
  }
};

}

#endif
