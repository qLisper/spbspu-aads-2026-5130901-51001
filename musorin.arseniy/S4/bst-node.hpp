#ifndef MUSORIN_BST_NODE_HPP
#define MUSORIN_BST_NODE_HPP

namespace musorin
{

template< class Key, class Value >
struct BSTNode
{
  Key key;
  Value value;
  BSTNode* left;
  BSTNode* right;
  BSTNode* parent;

  BSTNode():
    key(),
    value(),
    left(nullptr),
    right(nullptr),
    parent(nullptr)
  {}

  BSTNode(const Key& k, const Value& v):
    key(k),
    value(v),
    left(nullptr),
    right(nullptr),
    parent(nullptr)
  {}
};

} 

#endif
