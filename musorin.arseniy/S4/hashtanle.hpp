#ifndef MUSORIN_HASHTABLE_HPP
#define MUSORIN_HASHTABLE_HPP

#include <cstddef>
#include <stdexcept>
#include <functional>
#include <utility>
#include <string>

namespace musorin
{

struct SimpleStringHash
{
  size_t operator()(const std::string& s) const
  {
    size_t h = 0;
    for (unsigned char c : s)
    {
      h = h * 31 + c;
    }
    return h;
  }
};

template< class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key> >
class HashTable
{
public:
  enum State { EMPTY, OCCUPIED, TOMBSTONE };

  class HTIter;
  class HTCIter;

  using iterator = HTIter;
  using const_iterator = HTCIter;

  friend class HTIter;
  friend class HTCIter;

  explicit HashTable(size_t capacity = 64):
    entries_(new Entry[capacity]),
    capacity_(capacity),
    size_(0),
    tombstone_count_(0)
  {
    for (size_t i = 0; i < capacity_; ++i)
    {
      entries_[i].state = EMPTY;
    }
  }

  ~HashTable()
  {
    delete[] entries_;
  }

  HashTable(const HashTable& other):
    entries_(new Entry[other.capacity_]),
    capacity_(other.capacity_),
    size_(other.size_),
    tombstone_count_(other.tombstone_count_)
  {
    for (size_t i = 0; i < capacity_; ++i)
    {
      entries_[i] = other.entries_[i];
    }
  }

  HashTable(HashTable&& other) noexcept:
    entries_(other.entries_),
    capacity_(other.capacity_),
    size_(other.size_),
    tombstone_count_(other.tombstone_count_)
  {
    other.entries_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
    other.tombstone_count_ = 0;
  }

  HashTable& operator=(const HashTable& other)
  {
    if (this != &other)
    {
      HashTable tmp(other);
      swap(tmp);
    }
    return *this;
  }

  HashTable& operator=(HashTable&& other) noexcept
  {
    if (this != &other)
    {
      delete[] entries_;
      entries_ = other.entries_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      tombstone_count_ = other.tombstone_count_;
      other.entries_ = nullptr;
      other.capacity_ = 0;
      other.size_ = 0;
      other.tombstone_count_ = 0;
    }
    return *this;
  }

  void add(const Key& k, const Value& v)
  {
    size_t idx = findInsertIndex(k);
    if (idx == capacity_)
    {
      throw std::runtime_error("Hash table is full");
    }
    Entry& entry = entries_[idx];
    if (entry.state == OCCUPIED && Equal()(entry.pair_.first, k))
    {
      entry.pair_.second = v;
    }
    else
    {
      if (entry.state == TOMBSTONE)
      {
        --tombstone_count_;
      }
      new (&entry.pair_) std::pair<const Key, Value>(k, v);
      entry.state = OCCUPIED;
      ++size_;
    }
  }

  Value drop(const Key& k)
  {
    size_t idx = findIndex(k);
    if (idx == capacity_)
    {
      throw std::runtime_error("Key not found");
    }
    Entry& entry = entries_[idx];
    Value val = entry.pair_.second;
    entry.pair_.~pair();
    entry.state = TOMBSTONE;
    ++tombstone_count_;
    --size_;
    return val;
  }

  bool has(const Key& k) const
  {
    return findIndex(k) != capacity_;
  }

  Value& at(const Key& k)
  {
    size_t idx = findIndex(k);
    if (idx == capacity_)
    {
      throw std::runtime_error("Key not found");
    }
    return entries_[idx].pair_.second;
  }

  const Value& at(const Key& k) const
  {
    size_t idx = findIndex(k);
    if (idx == capacity_)
    {
      throw std::runtime_error("Key not found");
    }
    return entries_[idx].pair_.second;
  }

  iterator find(const Key& k)
  {
    size_t idx = findIndex(k);
    if (idx != capacity_)
    {
      return iterator(this, idx);
    }
    return end();
  }

  const_iterator find(const Key& k) const
  {
    size_t idx = findIndex(k);
    if (idx != capacity_)
    {
      return const_iterator(this, idx);
    }
    return end();
  }

  void rehash(size_t slots)
  {
    if (slots == 0)
    {
      throw std::invalid_argument("Slots must be > 0");
    }
    size_t old_size = size_;
    size_t old_tombstone = tombstone_count_;
    Entry* old_entries = entries_;
    size_t old_capacity = capacity_;
    entries_ = new Entry[slots];
    capacity_ = slots;
    size_ = 0;
    tombstone_count_ = 0;
    for (size_t i = 0; i < capacity_; ++i)
    {
      entries_[i].state = EMPTY;
    }
    for (size_t i = 0; i < old_capacity; ++i)
    {
      if (old_entries[i].state == OCCUPIED)
      {
        const auto& p = old_entries[i].pair_;
        size_t idx = findInsertIndex(p.first);
        if (idx == capacity_)
        {
          delete[] entries_;
          entries_ = old_entries;
          capacity_ = old_capacity;
          size_ = old_size;
          tombstone_count_ = old_tombstone;
          throw std::runtime_error("Rehash failed: not enough space");
        }
        new (&entries_[idx].pair_) std::pair<const Key, Value>(p);
        entries_[idx].state = OCCUPIED;
        ++size_;
      }
    }
    
    for (size_t i = 0; i < old_capacity; ++i)
    {
      if (old_entries[i].state == OCCUPIED)
      {
        old_entries[i].pair_.~pair();
        old_entries[i].state = TOMBSTONE;
      }
    }
    delete[] old_entries;
  }

  iterator begin()
  {
    size_t idx = 0;
    while (idx < capacity_ && entries_[idx].state != OCCUPIED)
    {
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
    size_t idx = 0;
    while (idx < capacity_ && entries_[idx].state != OCCUPIED)
    {
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

  size_t size() const
  {
    return size_;
  }

  bool empty() const
  {
    return size_ == 0;
  }

private:
  struct Entry
  {
    State state;
    union {
      std::pair<const Key, Value> pair_;
    };

    Entry() : state(EMPTY) {}

    ~Entry()
    {
      if (state == OCCUPIED)
        pair_.~pair();
    }

    Entry(const Entry& other) : state(other.state)
    {
      if (state == OCCUPIED)
        new (&pair_) std::pair<const Key, Value>(other.pair_);
    }

    Entry& operator=(const Entry& other)
    {
      if (this != &other)
      {
        if (state == OCCUPIED)
          pair_.~pair();
        state = other.state;
        if (state == OCCUPIED)
          new (&pair_) std::pair<const Key, Value>(other.pair_);
      }
      return *this;
    }
  };

  Entry* entries_;
  size_t capacity_;
  size_t size_;
  size_t tombstone_count_;

  size_t hash(const Key& k) const
  {
    return Hash()(k) % capacity_;
  }

  size_t findIndex(const Key& k) const
  {
    size_t h = hash(k);
    bool seen_tombstone = false;
    for (size_t i = 0; i < capacity_; ++i)
    {
      size_t idx = (h + i) % capacity_;
      if (entries_[idx].state == EMPTY)
      {
        if (!seen_tombstone) return capacity_;
      }
      else if (entries_[idx].state == TOMBSTONE)
      {
        seen_tombstone = true;
      }
      else if (entries_[idx].state == OCCUPIED && Equal()(entries_[idx].pair_.first, k))
      {
        return idx;
      }
    }
    return capacity_;
  }

  size_t findInsertIndex(const Key& k) const
  {
    size_t h = hash(k);
    size_t first_tombstone = capacity_;
    for (size_t i = 0; i < capacity_; ++i)
    {
      size_t idx = (h + i) % capacity_;
      if (entries_[idx].state == EMPTY)
      {
        return (first_tombstone != capacity_) ? first_tombstone : idx;
      }
      else if (entries_[idx].state == TOMBSTONE)
      {
        if (first_tombstone == capacity_)
        {
          first_tombstone = idx;
        }
      }
      else if (entries_[idx].state == OCCUPIED && Equal()(entries_[idx].pair_.first, k))
      {
        return idx;
      }
    }
    return (first_tombstone != capacity_) ? first_tombstone : capacity_;
  }

  void swap(HashTable& other) noexcept
  {
    std::swap(entries_, other.entries_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
    std::swap(tombstone_count_, other.tombstone_count_);
  }
};


template< class Key, class Value, class Hash, class Equal >
class HashTable<Key, Value, Hash, Equal>::HTIter
{
  friend class HashTable<Key, Value, Hash, Equal>;
public:
  HTIter():
    table_(nullptr),
    index_(0)
  {}

  std::pair<const Key&, Value&> operator*() const
  {
    return { table_->entries_[index_].pair_.first, table_->entries_[index_].pair_.second };
  }

  std::pair<const Key, Value>* operator->() const
  {
    return reinterpret_cast<std::pair<const Key, Value>*>(&table_->entries_[index_].pair_);
  }

  HTIter& operator++()
  {
    do
    {
      ++index_;
    }
    while (index_ < table_->capacity_ &&
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
  explicit HTIter(HashTable* table, size_t idx):
    table_(table),
    index_(idx)
  {}

  HashTable* table_;
  size_t index_;
};

template< class Key, class Value, class Hash, class Equal >
class HashTable<Key, Value, Hash, Equal>::HTCIter
{
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

  std::pair<const Key&, const Value&> operator*() const
  {
    return { table_->entries_[index_].pair_.first, table_->entries_[index_].pair_.second };
  }

  const std::pair<const Key, Value>* operator->() const
  {
    return reinterpret_cast<const std::pair<const Key, Value>*>(&table_->entries_[index_].pair_);
  }

  HTCIter& operator++()
  {
    do
    {
      ++index_;
    }
    while (index_ < table_->capacity_ &&
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
  explicit HTCIter(const HashTable* table, size_t idx):
    table_(table),
    index_(idx)
  {}

  const HashTable* table_;
  size_t index_;
};

} 

#endif
