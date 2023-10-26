#ifndef BL_ARRAY_HASHMAP_H
#define BL_ARRAY_HASHMAP_H

#include "bl/ds/dynamic_array.h" // DynamicArray
#include "bl/mem/allocator.h"    // Allocator
#include "bl/option.h"           // Option
#include "bl/primitives.h"       // usize, u8, f64
#include <functional>            // hash

namespace bl::ds {

namespace array_hashmap_internal {
extern mem::Allocator DEFAULT_C_ALLOCATOR;
extern const u8       RESIZE_FACTOR;
} // namespace array_hashmap_internal

// TODO: Add random seed/insertable seed

template <typename K> using HashFn = usize (*)(K);

template <typename K, typename V> struct ArrayHashMap {
public:
  ArrayHashMap() {
    this->allocator = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;
    this->keys      = DynamicArray<Option<K>>(this->allocator);
    this->values    = DynamicArray<Option<V>>(this->allocator);
    this->capacity  = 0;
    this->size      = 0;
  }

  ArrayHashMap(usize capacity) {
    this->allocator = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;
    this->keys      = DynamicArray<Option<K>>(this->allocator, capacity);
    this->values    = DynamicArray<Option<V>>(this->allocator, capacity);
    this->capacity  = capacity;
    this->size      = 0;
  }

  ArrayHashMap(mem::Allocator* allocator, HashFn<K> hashFn, usize capacity) {
    this->allocator = allocator;
    this->keys      = DynamicArray<Option<K>>(this->allocator, capacity);
    this->values    = DynamicArray<Option<V>>(this->allocator, capacity);
    this->capacity  = capacity;
    this->size      = 0;
    this->hashFn    = Some(hashFn);
  }
  // std::hash<K>{}(this->keys[0].unwrapOr(1));

  ~ArrayHashMap() {
    if (this->capacity != 0) {
      this->keys.~DynamicArray();
      this->values.~DynamicArray();
      this->capacity = 0;
      this->size     = 0;
    }
  }

private:
  mem::Allocator*         allocator;
  DynamicArray<Option<K>> keys;
  DynamicArray<Option<V>> values;
  Option<HashFn<K>>       hashFn   = None();
  usize                   capacity = 0;
  usize                   size     = 0;
};

} // namespace bl::ds

#endif // !BL_ARRAY_HASHMAP_H
