#ifndef MUSORIN_HASHTABLE_HPP
#define MUSORIN_HASHTABLE_HPP

#include <cstddef>
#include <stdexcept>
#include <functional>
#include <utility>

namespace musorin {

template< class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
class HashTable {
public:
  enum State { EMPTY, OCCUPIED, TOMBSTONE };

  class HTIter;
  class HTCIter;

  using iterator = HTIter;
  using const_iterator = HTCIter;

  friend class HTIter;
  friend class HTCIter;

  explicit HashTable(std::size_t capacity = 10):
    entries_(new Entry[capacity]),
    capacity_(capacity),
    size_(0),
    tombstone_count_(0)
  {
    for (std::size_t i = 0; i < capacity_; ++i) {
      entries_[i].state = EMPTY;
    }
  }

  ~HashTable()
  {
    delete[] entries_;
  }

  void add(const Key& k, const Value& v)
  {
    std::size_t idx = findInsertIndex(k);
    if (idx == capacity_) {
      throw std::runtime_error("Hash table is full");
    }
    Entry& entry = entries_[idx];
    if (entry.state == OCCUPIED && Equal()(entry.key, k)) {
      entry.value = v;
    } else {
      if (entry.state == TOMBSTONE) {
        --tombstone_count_;
      }
      entry.key = k;
      entry.value = v;
      entry.state = OCCUPIED;
      ++size_;
    }
  }

  Value drop(const Key& k)
  {
    std::size_t idx = findIndex(k);
    if (idx == capacity_) {
      throw std::runtime_error("Key not found");
    }
    Entry& entry = entries_[idx];
    entry.state = TOMBSTONE;
    ++tombstone_count_;
    --size_;
    return entry.value;
  }

  bool has(const Key& k) const
  {
    return findIndex(k) != capacity_;
  }

  Value& at(const Key& k)
  {
    std::size_t idx = findIndex(k);
    if (idx == capacity_) {
      throw std::runtime_error("Key not found");
    }
    return entries_[idx].value;
  }

  const Value& at(const Key& k) const
  {
    std::size_t idx = findIndex(k);
    if (idx == capacity_) {
      throw std::runtime_error("Key not found");
    }
    return entries_[idx].value;
  }

  iterator find(const Key& k)
  {
    std::size_t idx = findIndex(k);
    if (idx != capacity_) {
      return iterator(this, idx);
    }
    return end();
  }

  const_iterator find(const Key& k) const
  {
    std::size_t idx = findIndex(k);
    if (idx != capacity_) {
      return const_iterator(this, idx);
    }
    return end();
  }

  void rehash(std::size_t slots)
  {
    if (slots == 0) {
      throw std::invalid_argument("Slots must be > 0");
    }
    Entry* old_entries = entries_;
    std::size_t old_capacity = capacity_;
    entries_ = new Entry[slots];
    capacity_ = slots;
    size_ = 0;
    tombstone_count_ = 0;
    for (std::size_t i = 0; i < capacity_; ++i) {
      entries_[i].state = EMPTY;
    }
    for (std::size_t i = 0; i < old_capacity; ++i) {
      if (old_entries[i].state == OCCUPIED) {
        std::size_t idx = findInsertIndex(old_entries[i].key);
        if (idx == capacity_) {
          // откат изменений
          delete[] entries_;
          entries_ = old_entries;
          capacity_ = old_capacity;
          throw std::runtime_error("Rehash failed: not enough space");
        }
        entries_[idx].key = old_entries[i].key;
        entries_[idx].value = old_entries[i].value;
        entries_[idx].state = OCCUPIED;
        ++size_;
      }
    }
    delete[] old_entries;
  }

  iterator begin()
  {
    std::size_t idx = 0;
    while (idx < capacity_ && entries_[idx].state != OCCUPIED) {
      ++idx;
    }
    return iterator(this, idx);
  }

  iterator end()
  {
    return iterator(this, capacity_);
  }

  const_iterator begin() const
  {
    std::size_t idx = 0;
    while (idx < capacity_ && entries_[idx].state != OCCUPIED) {
      ++idx;
    }
    return const_iterator(this, idx);
  }

  const_iterator end() const
  {
    return const_iterator(this, capacity_);
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator cend() const
  {
    return end();
  }

  std::size_t size() const
  {
    return size_;
  }

  bool empty() const
  {
    return size_ == 0;
  }

private:
  struct Entry {
    Key key;
    Value value;
    State state;
    Entry(): state(EMPTY) {}
  };

  Entry* entries_;
  std::size_t capacity_;
  std::size_t size_;
  std::size_t tombstone_count_;

  std::size_t hash(const Key& k) const
  {
    return Hash()(k) % capacity_;
  }

  std::size_t findIndex(const Key& k) const
  {
    std::size_t h = hash(k);
    for (std::size_t i = 0; i < capacity_; ++i) {
      std::size_t idx = (h + i) % capacity_;
      if (entries_[idx].state == EMPTY) {
        return capacity_;
      } else if (entries_[idx].state == OCCUPIED && Equal()(entries_[idx].key, k)) {
        return idx;
      }
    }
    return capacity_;
  }

  std::size_t findInsertIndex(const Key& k) const
  {
    std::size_t h = hash(k);
    std::size_t first_tombstone = capacity_;
    for (std::size_t i = 0; i < capacity_; ++i) {
      std::size_t idx = (h + i) % capacity_;
      if (entries_[idx].state == EMPTY) {
        return (first_tombstone != capacity_) ? first_tombstone : idx;
      } else if (entries_[idx].state == TOMBSTONE) {
        if (first_tombstone == capacity_) {
          first_tombstone = idx;
        }
      } else if (entries_[idx].state == OCCUPIED && Equal()(entries_[idx].key, k)) {
        return idx;
      }
    }
    return (first_tombstone != capacity_) ? first_tombstone : capacity_;
  }
};


template< class Key, class Value, class Hash, class Equal >
class HashTable<Key, Value, Hash, Equal>::HTIter {
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
  explicit HTIter(HashTable* table, std::size_t idx):
    table_(table),
    index_(idx)
  {}

  HashTable* table_;
  std::size_t index_;
};

template< class Key, class Value, class Hash, class Equal >
class HashTable<Key, Value, Hash, Equal>::HTCIter {
  friend class HashTable<Key, Value, Hash, Equal>;
public:
  HTCIter():
    table_(nullptr),
    index_(0)
  {}

  HTCIter(const HTIter& it):
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
  explicit HTCIter(const HashTable* table, std::size_t idx):
    table_(table),
    index_(idx)
  {}

  const HashTable* table_;
  std::size_t index_;
};

}

#endif
