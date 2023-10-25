#ifndef BL_DYNAMIC_ARRAY_H
#define BL_DYNAMIC_ARRAY_H

#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator
#include "bl/panic.h"           // BL_PANIC, panic
#include "bl/primitives.h"      // const_cstr, usize, u8

#include <cstdlib>          // abort
#include <cstring>          // memcpy
#include <initializer_list> // initializer_list

namespace bl::ds {
using namespace primitives;

// TODO: Add error section on doc comments (see String)
//
// TODO: Replace raw casts with static_casts

namespace dynamic_array_internal {
enum class DynamicArrayError {
  InvalidAllocator,
  InvalidArray,
  BufferAllocationFailed,
  BufferDeallocationFailed,
  BufferResizeFailed,
  MemcpyFailed,
  ResizeFailed,
  IndexOutOfBounds,
  InvalidPop,
};

const_cstr            errMsg(DynamicArrayError err);
extern mem::Allocator DEFAULT_C_ALLOCATOR;
extern const u8       RESIZE_FACTOR;
} // namespace dynamic_array_internal

template <typename T> struct DynamicArray {
public:
  /// Creates an empty dynamic array with `mem::CAllocator` as its backing
  /// allocator.
  ///
  /// ## Note
  /// Nothing is allocated until the first push.
  DynamicArray() {
    this->allocator = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;
  }

  /// Creates an empty dynamic array backed by the given allocator.
  ///
  /// ## Note
  /// Nothing is allocated until the first push.
  DynamicArray(mem::Allocator* allocator) {
    // Input validation
    {
      Panic::resetError();

      if (allocator == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::InvalidAllocator));
        return;
      }
    }

    this->allocator = allocator;
  }

  /// Creates an empty dynamic array with the specified capacity, backed by the
  /// given allocator.
  ///
  /// ## Note
  /// If the capacity is `0`, then this just calls the
  /// `DynamicArray(mem::Allocator*)` constructor (nothing gets allocated).
  DynamicArray(mem::Allocator* allocator, usize capacity) {
    // Input validation
    {
      Panic::resetError();

      if (allocator == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::InvalidAllocator));
        return;
      }
    }

    if (capacity != 0) {
      T* data = (T*)allocator->allocRaw(capacity * sizeof(T));
      if (data == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
        return;
      }
      this->data = data;
      this->cap  = capacity;
    }
  }

  /// Creates an empty dynamic array with the specified capacity, backed by the
  /// `mem::CAllocator`.
  ///
  /// ## Note
  /// If the capacity is `0`, then this just calls the
  /// `DynamicArray(mem::Allocator*)` constructor (nothing gets allocated).
  DynamicArray(usize capacity) {
    Panic::resetError();

    this->allocator = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;

    if (capacity != 0) {
      T* data = (T*)this->allocator->allocRaw(capacity * sizeof(T));
      if (data == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
        return;
      }
      this->data = data;
      this->cap  = capacity;
    }
  }

  /// Creates a dynamic array backed by the given allocator with data from the
  /// initializer list.
  DynamicArray<T>(mem::Allocator* allocator, std::initializer_list<T> list) {
    // Input validation
    {
      Panic::resetError();

      if (allocator == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::InvalidAllocator));
        return;
      }
    }

    this->allocator       = allocator;

    // Allocate buffer for dynamic array
    const usize list_size = list.size();
    T*          data = (T*)this->allocator->allocRaw(sizeof(T) * list_size);
    if (data == nullptr) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
      return;
    }

    // Copy data from given array
    usize idx = 0;
    for (auto& val : list) {
      data[idx] = val;
      ++idx;
    }

    this->data = data;
    this->len  = list_size;
    this->cap  = list_size;
  }

  /// Creates a dynamic array backed by the `mem::CAllocator` with data from the
  /// initializer list.
  DynamicArray<T>(std::initializer_list<T> list) {
    // Input validation
    Panic::resetError();

    this->allocator       = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;

    // Allocate buffer for dynamic array
    const usize list_size = list.size();
    T*          data = (T*)this->allocator->allocRaw(sizeof(T) * list_size);
    if (data == nullptr) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
      return;
    }

    // Copy data from given array
    usize idx = 0;
    for (auto& val : list) {
      data[idx] = val;
      ++idx;
    }

    this->data = data;
    this->len  = list_size;
    this->cap  = list_size;
  }

  /// Clones the dynamic array.
  ///
  /// ## Note
  /// The capacity of the cloned array will not be the same as the orininal's,
  /// but the length and elements will be the same.
  DynamicArray(const DynamicArray& other) {
    Panic::resetError();

    this->allocator = other.allocator;
    this->len       = other.len;
    this->cap       = other.len;

    T* data         = (T*)this->allocator->allocRaw(this->len * sizeof(T));
    if (data == nullptr) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
      return;
    }
    this->data = data;

    T* copied  = memcpy(this->data, other.data, this->len * sizeof(T));
    if (copied == nullptr) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::MemcpyFailed));
      return;
    }
    this->data = copied;
  }

  /// Deallocates memory used by the array.
  ~DynamicArray() {
    if (this->cap != 0) {
      this->allocator->deallocRaw(this->data);
    }
  }

  DynamicArray& operator=(const DynamicArray&) = default;

  /// Operator overload for index operator.
  ///
  /// ## Error
  /// - Throws an error if the index is out of the array's bounds.
  T&            operator[](usize idx) {
    // Input validation
    {
      Panic::resetError();
      if (idx > this->len - 1) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::IndexOutOfBounds));
        Panic::printErrorTrace();
        abort();
      }
    }

    return this->data[idx];
  }

  /// Returns the underlying element buffer.
  const T* getRaw(void) const { return this->data; }

  /// Returns the length of the array.
  usize    getLen(void) const { return this->len; }

  /// Returns the capacity of the array.
  usize    getCap(void) const { return this->cap; }

  /// Checks if the array is empty.
  bool     isEmpty(void) const { return this->len == 0; }

  /// Removes all of the array's contents, but leaves the capacity unchanged.
  void     clear(void) {
    // TODO: Call destructor for elements?
    this->len = 0;
  }

  /// Appends the given value to the end of the array.
  void push(T val) {
    Panic::resetError();

    // Allocate on first push
    if (this->cap == 0) {
      T* data = (T*)this->allocator->allocRaw(sizeof(T));
      if (data == nullptr) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
        return;
      }
      this->data = data;
      this->cap  = 1;
    }

    // Resize if necessary
    usize new_len = this->len + 1;
    if (new_len > this->cap) {
      this->resize();
      if (Panic::isError()) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::ResizeFailed));
        return;
      }
    }

    this->data[this->len]  = val;
    this->len             += 1;
  }

  /// Removes and returns the last element in the array.
  T pop(void) {
    Panic::resetError();

    if (this->len == 0) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::InvalidPop));
      return T();
    }

    T popped   = this->data[this->len - 1];
    this->len -= 1;
    return popped;
  }

  // TODO: Update `String`'s `insert` and `remove` functions to shift like these
  // instead of extra allocations

  /// Inserts the given value at the specified index, shifting all elements
  /// after it to the right.
  ///
  /// ## Note
  /// This is **O(n)** in the worst case due to the shifting of elements.
  void insert(usize idx, T val) {
    // Input validation
    {
      Panic::resetError();
      if (idx > this->len - 1) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::IndexOutOfBounds));
        return;
      }
    }

    // Just call `push` if index is the last element
    if (idx == this->len - 1) {
      this->push(val);
      return;
    }

    // Resize original buffer if necessary
    if (this->len + 1 > this->cap) {
      this->resize();
      if (Panic::isError()) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::ResizeFailed));
        return;
      }
    }

    // Shift all elements after `idx` to the right
    for (usize i = this->len; i > idx; i--) {
      this->data[i] = this->data[i - 1];
    }

    // Insert value to specified index
    this->data[idx]  = val;
    this->len       += 1;
  }

  /// Removes and returns the element at the specified index, shifting all
  /// elements after it to the left.
  ///
  /// ## Note
  /// This is **O(n)** in the worst case due to the shifting of elements; if the
  /// order does not need to be preserved, then `DynamicArray::swapRemove`
  /// should be used instead.
  T remove(usize idx) {
    // Input validation
    {
      Panic::resetError();
      if (idx > this->len - 1) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::IndexOutOfBounds));
        abort();
      }
    }

    // Just call `pop` if index is the last element
    if (idx == this->len - 1) {
      return this->pop();
    }

    T removed = this->data[idx];

    // Shift all elements after `idx` to the left
    for (usize i = idx; i < this->len - 1; i++) {
      this->data[i] = this->data[i + 1];
    }
    this->len -= 1;

    return removed;
  }

  /// Removes and returns the element at the specified index.
  ///
  /// The removed element is replaced by the last element of the array.
  ///
  /// ## Note
  /// This does not preserve ordering, but is **O(1)**; if order needs to be
  /// preserved, use `DynamicArray::remove` instead;
  T swapRemove(usize idx) {
    // Input validation
    {
      Panic::resetError();
      if (idx > this->len - 1) {
        BL_THROW(dynamic_array_internal::errMsg(
            dynamic_array_internal::DynamicArrayError::IndexOutOfBounds));
        abort();
      }
    }

    // Just call `pop` if index is the last element
    if (idx == this->len - 1) {
      return this->pop();
    }

    T removed        = this->data[idx];

    // Replace the indexc
    this->data[idx]  = this->data[this->len - 1];
    this->len       -= 1;

    return removed;
  }

private:
  /// Backing allocator used for internal allocations.
  mem::Allocator* allocator;

  /// The actual element buffer.
  T*              data = nullptr;

  /// The length of the array.
  usize           len  = 0;

  /// The capacity of the array.
  usize           cap  = 0;

  /// Function to resize the array.
  void            resize(void) {
    // Resize the buffer to new capacity
    usize new_cap = this->cap * dynamic_array_internal::RESIZE_FACTOR;
    T*    resized =
        (T*)this->allocator->resizeRaw(this->data, new_cap * sizeof(T));
    if (resized == nullptr) {
      BL_THROW(dynamic_array_internal::errMsg(
          dynamic_array_internal::DynamicArrayError::BufferResizeFailed));
      return;
    }

    // Set the buffer to the resized one
    this->data = resized;
    this->cap  = new_cap;
  }
};

} // namespace bl::ds

#endif // !BL_DYNAMIC_ARRAY_H
