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
	
	private:
	  BSTNode<Key, Value>* root_;
	  size_t size_;
	
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
