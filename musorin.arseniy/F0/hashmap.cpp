#ifndef HASHMAP_HPP
#define HASHMAP_HPP

#include <cstddef>
#include <string>
#include <stdexcept>
#include "list.hpp"

template< class Key, class Value >
class HashMap
{
public:
  HashMap():
    buckets_(new List<Pair>*[INITIAL_BUCKETS]),
    bucketCount_(INITIAL_BUCKETS),
    itemCount_(0)
  {
    for (std::size_t i = 0; i < bucketCount_; ++i)
    {
      buckets_[i] = nullptr;
    }
  }

  ~HashMap()
  {
    clear();
    delete[] buckets_;
  }

  void insert(const Key& key, const Value& value)
  {
    std::size_t idx = hash(key) % bucketCount_;
    if (!buckets_[idx])
    {
      buckets_[idx] = new List<Pair>;
    }
    List<Pair>* chain = buckets_[idx];
    auto* curr = chain->head();
    while (curr)
    {
      if (curr->data.key == key)
      {
        throw std::runtime_error("Key already exists");
      }
      curr = curr->next_;
    }
    Pair p{key, value};
    chain->pushFront(p);
    ++itemCount_;
  }

  Value* find(const Key& key)
  {
    std::size_t idx = hash(key) % bucketCount_;
    if (!buckets_[idx])
    {
      return nullptr;
    }
    List<Pair>* chain = buckets_[idx];
    auto* curr = chain->head();
    while (curr)
    {
      if (curr->data.key == key)
      {
        return &curr->data.value;
      }
      curr = curr->next_;
    }
    return nullptr;
  }

  bool contains(const Key& key)
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

  std::size_t size() const
  {
    return itemCount_;
  }

private:
  struct Pair
  {
    Key key;
    Value value;
  };

  std::size_t hash(const std::string& s) const
  {
    unsigned long h = 5381;
    for (char c : s)
    {
      h = ((h << 5) + h) + static_cast<unsigned long>(c);
    }
    return static_cast<std::size_t>(h);
  }

  static constexpr std::size_t INITIAL_BUCKETS = 64;
  List<Pair>** buckets_;
  std::size_t bucketCount_;
  std::size_t itemCount_;
};

#endif
