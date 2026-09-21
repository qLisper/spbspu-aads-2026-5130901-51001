#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <cstddef>
#include <string>
#include <stdexcept>
#include <functional>
#include <utility>
#include "list.hpp"

template< class Key, class Value >
class HashMap
{
public:
  struct Pair
  {
    Key key;
    Value value;
  };

  class Iterator
  {
    friend class HashMap;
  public:
    Iterator() : map_(nullptr), bucket_(0), node_(nullptr) {}

    bool operator==(const Iterator& other) const
    {
      return map_ == other.map_
          && bucket_ == other.bucket_
          && node_ == other.node_;
    }
    bool operator!=(const Iterator& other) const { return !(*this == other); }

    Iterator& operator++()
    {
      if (node_)
        node_ = node_->next_;

      while (!node_ && bucket_ < map_->bucketCount_)
      {
        ++bucket_;
        if (bucket_ >= map_->bucketCount_)
          break;
        if (map_->buckets_[bucket_])
          node_ = map_->buckets_[bucket_]->head();
      }
      return *this;
    }

    Pair& operator*()  const { return node_->data; }
    Pair* operator->() const { return &(node_->data); }

  private:
    Iterator(const HashMap* map, std::size_t bucket,
             typename List<Pair>::Node* node)
      : map_(map), bucket_(bucket), node_(node) {}

    const HashMap* map_;
    std::size_t bucket_;
    typename List<Pair>::Node* node_;
  };

  HashMap()
    : buckets_(new List<Pair>*[INITIAL_BUCKETS]),
      bucketCount_(INITIAL_BUCKETS),
      itemCount_(0)
  {
    for (std::size_t i = 0; i < bucketCount_; ++i)
      buckets_[i] = nullptr;
  }

  HashMap(const HashMap& other)
    : buckets_(new List<Pair>*[other.bucketCount_]),
      bucketCount_(other.bucketCount_),
      itemCount_(other.itemCount_)
  {
    for (std::size_t i = 0; i < bucketCount_; ++i)
    {
      if (other.buckets_[i])
        buckets_[i] = new List<Pair>(*other.buckets_[i]);
      else
        buckets_[i] = nullptr;
    }
  }

  HashMap& operator=(const HashMap& other)
  {
    if (this != &other)
    {
      HashMap tmp(other);
      std::swap(buckets_, tmp.buckets_);
      std::swap(bucketCount_, tmp.bucketCount_);
      std::swap(itemCount_, tmp.itemCount_);
    }
    return *this;
  }

  ~HashMap()
  {
    clear();
    delete[] buckets_;
  }

  bool remove(const Key& key)
  {
    std::size_t idx = hash(key) % bucketCount_;
    if (!buckets_[idx])
      return false;

    List<Pair>* chain = buckets_[idx];
    auto* curr = chain->head();
    typename List<Pair>::Node* prev = nullptr;
    while (curr)
    {
      if (curr->data.key == key)
      {
        if (prev)
          prev->next_ = curr->next_;
        else
          chain->setHead(curr->next_);
        delete curr;
        --chain->size_;
        --itemCount_;
        return true;
      }
      prev = curr;
      curr = curr->next_;
    }
    return false;
  }

  void insert(const Key& key, const Value& value)
  {
    std::size_t idx = hash(key) % bucketCount_;
    if (!buckets_[idx])
      buckets_[idx] = new List<Pair>;

    List<Pair>* chain = buckets_[idx];
    auto* curr = chain->head();
    while (curr)
    {
      if (curr->data.key == key)
        throw std::runtime_error("Key already exists");
      curr = curr->next_;
    }
    Pair p{key, value};
    chain->pushFront(p);
    ++itemCount_;
  }

  Value* find(const Key& key)
  {
    return const_cast<Value*>(
      static_cast<const HashMap*>(this)->find(key));
  }

  const Value* find(const Key& key) const
  {
    std::size_t idx = hash(key) % bucketCount_;
    if (!buckets_[idx])
      return nullptr;

    List<Pair>* chain = buckets_[idx];
    auto* curr = chain->head();
    while (curr)
    {
      if (curr->data.key == key)
        return &curr->data.value;
      curr = curr->next_;
    }
    return nullptr;
  }

  bool contains(const Key& key) const
  {
    return find(key) != nullptr;
  }

  void clear()
  {
    for (std::size_t i = 0; i < bucketCount_; ++i)
    {
      if (buckets_[i])
      {
        delete buckets_[i];
        buckets_[i] = nullptr;
      }
    }
    itemCount_ = 0;
  }

  std::size_t size() const { return itemCount_; }

  Iterator begin() const
  {
    for (std::size_t i = 0; i < bucketCount_; ++i)
    {
      if (buckets_[i] && buckets_[i]->head())
        return Iterator(this, i, buckets_[i]->head());
    }
    return end();
  }

  Iterator end() const
  {
    return Iterator(this, bucketCount_, nullptr);
  }

private:
  std::size_t hash(const Key& k) const
  {
    return std::hash<Key>{}(k);
  }

  static constexpr std::size_t INITIAL_BUCKETS = 64;
  List<Pair>** buckets_;
  std::size_t bucketCount_;
  std::size_t itemCount_;
};

#endif
