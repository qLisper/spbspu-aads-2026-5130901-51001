#ifndef MUSORIN_HASHTABLE_HPP
#define MUSORIN_HASHTABLE_HPP

#include <cstddef>
#include <utility>

namespace musorin {

template< class Key, class Value, class Hash, class Equal >
class HashTable;

template< class Key, class Value, class Hash, class Equal >
class HTIter {
  friend class HashTable<Key, Value, Hash, Equal>;
public:
  HTIter():
    table_(nullptr),
    index_(0)
  {}

  std::pair< const Key&, Value& > operator*() const
  {
    return { table_->entries_[index_].key, table_->entries_[index_].value };
  }

  std::pair< const Key&, Value& >* operator->() const
  {
    return nullptr;
  }

  HTIter& operator++()
  {
    do {
      ++index_;
    } while (index_ < table_->capacity_ &&
             table_->entries_[index_].state != HashTable<Key, Value, Hash, Equal>::OCCUPIED);
    return *this;
  }

  HTIter operator++(int)
  {
    HTIter tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const HTIter& rhs) const
  {
    return table_ == rhs.table_ && index_ == rhs.index_;
  }

  bool operator!=(const HTIter& rhs) const
  {
    return !(*this == rhs);
  }

private:
  explicit HTIter(HashTable<Key, Value, Hash, Equal>* table, std::size_t idx):
    table_(table),
    index_(idx)
  {}

  HashTable<Key, Value, Hash, Equal>* table_;
  std::size_t index_;
};

template< class Key, class Value, class Hash, class Equal >
class HTCIter {
  friend class HashTable<Key, Value, Hash, Equal>;
public:
  HTCIter():
    table_(nullptr),
    index_(0)
  {}

  HTCIter(const HTIter<Key, Value, Hash, Equal>& it):
    table_(it.table_),
    index_(it.index_)
  {}

  std::pair< const Key&, const Value& > operator*() const
  {
    return { table_->entries_[index_].key, table_->entries_[index_].value };
  }

  HTCIter& operator++()
  {
    do {
      ++index_;
    } while (index_ < table_->capacity_ &&
             table_->entries_[index_].state != HashTable<Key, Value, Hash, Equal>::OCCUPIED);
    return *this;
  }

  HTCIter operator++(int)
  {
    HTCIter tmp(*this);
    ++(*this);
    return tmp;
  }

  bool operator==(const HTCIter& rhs) const
  {
    return table_ == rhs.table_ && index_ == rhs.index_;
  }

  bool operator!=(const HTCIter& rhs) const
  {
    return !(*this == rhs);
  }

private:
  explicit HTCIter(const HashTable<Key, Value, Hash, Equal>* table, std::size_t idx):
    table_(table),
    index_(idx)
  {}

  const HashTable<Key, Value, Hash, Equal>* table_;
  std::size_t index_;
};

}

#endif
