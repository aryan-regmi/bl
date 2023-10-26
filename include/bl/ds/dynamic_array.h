#ifndef BL_DYNAMIC_ARRAY_H
#define BL_DYNAMIC_ARRAY_H

#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator
#include "bl/option.h"          // Some, None, Option
#include "bl/panic.h"           // BL_PANIC, panic
#include "bl/primitives.h"      // const_cstr, usize, u8
#include "bl/result.h"          // Error, Result, Ok, Err

#include <cstdio>
#include <cstdlib>          // abort
#include <initializer_list> // initializer_list

namespace bl::ds {
using namespace primitives;

// TODO: Replace raw casts with static_casts
//
// TODO: Add `filter` function

// Possible error types returned by `DynamicArray`'s methods.
struct DynamicArrayError : public Error {
public:
  enum ErrorType {
    InvalidAllocator,
    ResizeFailed,
    BufferAllocationFailed,
    BufferDeallocationFailed,
    BufferResizeFailed,
    IndexOutOfBounds,
    InvalidPop,
    InvalidArray,
  };

  DynamicArrayError(ErrorType type);

  /// Returns the error message for the current error type.
  const_cstr errMsg(void) const;

private:
  ErrorType type;
};

namespace dynamic_array_internal {
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
  ///
  /// ## Panics
  /// - Panics if the allocator is null.
  DynamicArray(mem::Allocator* allocator) {
    // Input validation
    {
      if (allocator == nullptr) {
        BL_PANIC(
            DynamicArrayError(DynamicArrayError::InvalidAllocator).errMsg())
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
  ///
  /// ## Panics
  /// - Panics if the allocator is null.
  /// - Panics if the allocator is unable to allocate space for the array's
  /// buffer.
  DynamicArray(mem::Allocator* allocator, usize capacity) {
    // Input validation
    {
      if (allocator == nullptr) {
        BL_PANIC(
            DynamicArrayError(DynamicArrayError::InvalidAllocator).errMsg());
      }
    }

    if (capacity != 0) {
      T* data = (T*)allocator->allocRaw(capacity * sizeof(T));
      if (data == nullptr) {
        BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                     .errMsg());
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
  ///
  /// ## Panics
  /// - Panics if the allocator is unable to allocate space for the array's
  /// buffer.
  DynamicArray(usize capacity) {

    this->allocator = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;

    if (capacity != 0) {
      T* data = (T*)this->allocator->allocRaw(capacity * sizeof(T));
      if (data == nullptr) {
        BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                     .errMsg());
      }
      this->data = data;
      this->cap  = capacity;
    }
  }

  /// Creates a dynamic array backed by the given allocator with data from the
  // BL_THROW(dynamic_array_internal::errMsg(
  //     dynamic_array_internal::DynamicArrayError::BufferAllocationFailed));
  // return;
  /// initializer list.
  ///
  /// ## Panics
  /// - Panics if the allocator is null.
  /// - Panics if the allocator is unable to allocate space for the array's
  /// buffer.
  DynamicArray<T>(mem::Allocator* allocator, std::initializer_list<T> list) {
    // Input validation
    {
      if (allocator == nullptr) {
        BL_PANIC(
            DynamicArrayError(DynamicArrayError::InvalidAllocator).errMsg());
      }
    }

    this->allocator       = allocator;

    // Allocate buffer for dynamic array
    const usize list_size = list.size();
    T*          data = (T*)this->allocator->allocRaw(sizeof(T) * list_size);
    if (data == nullptr) {
      BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                   .errMsg());
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
  ///
  /// ## Panics
  /// - Panics if the allocator is unable to allocate space for the array's
  /// buffer.
  DynamicArray<T>(std::initializer_list<T> list) {
    this->allocator       = &dynamic_array_internal::DEFAULT_C_ALLOCATOR;

    // Allocate buffer for dynamic array
    const usize list_size = list.size();
    T*          data = (T*)this->allocator->allocRaw(sizeof(T) * list_size);
    if (data == nullptr) {
      BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                   .errMsg());
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
  ///
  /// ## Panics
  /// - Panics if the allocator is unable to allocate space for the array's
  /// buffer.
  DynamicArray(const DynamicArray& other) {
    this->allocator = other.allocator;
    this->len       = other.len;
    this->cap       = other.len;

    T* data         = (T*)this->allocator->allocRaw(this->len * sizeof(T));
    if (data == nullptr) {
      BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                   .errMsg());
    }
    this->data = data;

    for (usize i = 0; i < this->len; i++) {
      this->data[i] = other.data[i];
    }
  }

  /// Moves the dynamic array.
  DynamicArray(DynamicArray&& other) {
    this->allocator = other.allocator;
    this->len       = other.len;
    this->cap       = other.cap;
    this->data      = other.data;

    other.data      = nullptr;
    other.len       = 0;
    other.cap       = 0;
  }

  /// Deallocates memory used by the array.
  ~DynamicArray() {
    for (usize i = 0; i < this->len; i++) {
      this->data[i].~T();
    }

    if (this->cap != 0) {
      this->allocator->deallocRaw(this->data);
    }
  }

  /// Copy assigns an array.
  DynamicArray& operator=(const DynamicArray& other) {
    if (this == other) {
      return *this;
    }

    // Free old buffer if it exists
    if (this->data != nullptr) {
      this->allocator->deallocRaw(this->data);
    }
    this->allocator = other.allocator;

    // Allocate new buffer
    T* data         = (T*)this->allocator->allocRaw(other.len * sizeof(T));
    if (this->data == nullptr) {
      BL_PANIC(DynamicArrayError(DynamicArrayError::BufferAllocationFailed)
                   .errMsg());
    }

    // Copy data to new buffer
    for (usize i = 0; i < other.len; i++) {
      data[i] = other.data[i];
    }

    this->data = data;
    this->len  = other.len;
    this->cap  = other.len;
  }

  /// Move assigns an array.
  DynamicArray& operator=(DynamicArray&& other) {
    if (this->data == other.data) {
      return *this;
    }

    // Free old buffer if it exists
    if (this->data != nullptr) {
      this->allocator->deallocRaw(this->data);
    }

    this->allocator = other.allocator;
    this->data      = other.data;
    this->len       = other.len;
    this->cap       = other.cap;

    other.data      = nullptr;
    other.len       = 0;
    other.cap       = 0;

    return *this;
  }

  /// Operator overload for index operator.
  ///
  /// ## Panics
  /// - Panics if the index is out of the array's bounds.
  T& operator[](usize idx) {
    // Input validation
    {
      if (idx > this->len - 1) {
        BL_PANIC(
            DynamicArrayError(DynamicArrayError::IndexOutOfBounds).errMsg());
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
    for (usize i = 0; i < this->len; i++) {
      this->data[i].~T();
    }
    this->len = 0;
  }

  /// Appends the given value to the end of the array.
  Result<Void, DynamicArrayError> push(T val) {
    // Allocate on first push
    if (this->cap == 0) {
      T* data = (T*)this->allocator->allocRaw(sizeof(T));
      if (data == nullptr) {
        return Err(
            DynamicArrayError(DynamicArrayError::BufferAllocationFailed));
      }
      this->data = data;
      this->cap  = 1;
    }

    // Resize if necessary
    usize new_len = this->len + 1;
    if (new_len > this->cap) {
      Result<Void, DynamicArrayError> resized = this->resize();
      if (!resized) {
        return resized;
      }
    }

    this->data[this->len]  = val;
    this->len             += 1;

    return Ok(Void());
  }

  /// Removes and returns the last element in the array.
  ///
  /// Returns `None` if the array is empty.
  Option<T> pop(void) {
    if (this->len == 0) {
      return None();
    }

    T popped   = this->data[this->len - 1];
    this->len -= 1;
    return Some(popped);
  }

  // TODO: Update `String`'s `insert` and `remove` functions to shift like these
  // instead of extra allocations

  /// Inserts the given value at the specified index, shifting all elements
  /// after it to the right.
  ///
  /// ## Note
  /// This is **O(n)** in the worst case due to the shifting of elements.
  Result<Void, DynamicArrayError> insert(usize idx, T val) {
    // Input validation
    {
      if (idx > this->len - 1) {
        return Err(DynamicArrayError(DynamicArrayError::IndexOutOfBounds));
      }
    }

    // Just call `push` if index is the last element
    if (idx == this->len - 1) {
      return this->push(val);
    }

    // Resize original buffer if necessary
    if (this->len + 1 > this->cap) {
      Result<Void, DynamicArrayError> resized = this->resize();
      if (!resized) {
        return resized;
      }
    }

    // Shift all elements after `idx` to the right
    for (usize i = this->len; i > idx; i--) {
      this->data[i] = this->data[i - 1];
    }

    // Insert value to specified index
    this->data[idx]  = val;
    this->len       += 1;

    return Ok(Void());
  }

  /// Removes and returns the element at the specified index, shifting all
  /// elements after it to the left.
  ///
  /// ## Note
  /// This is **O(n)** in the worst case due to the shifting of elements; if the
  /// order does not need to be preserved, then `DynamicArray::swapRemove`
  /// should be used instead.
  Option<T> remove(usize idx) {
    // Input validation
    {
      if (idx > this->len - 1) {
        return None();
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

    return Some(removed);
  }

  /// Removes and returns the element at the specified index.
  ///
  /// The removed element is replaced by the last element of the array.
  ///
  /// ## Note
  /// This does not preserve ordering, but is **O(1)**; if order needs to be
  /// preserved, use `DynamicArray::remove` instead;
  Option<T> swapRemove(usize idx) {
    // Input validation
    {
      if (idx > this->len - 1) {
        return None();
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

    return Some(removed);
  }

  /// Checks if the array contains the specified value.
  bool contains(T to_find) const {
    for (usize i = 0; i < this->len; i++) {
      if (this->data[i] == to_find) {
        return true;
      }
    }

    return false;
  }

  /// Shrinks the capacity of the vector to its length.
  ///
  /// ## Note
  /// Some allocators may keep more than just the length allocated.
  Result<Void, DynamicArrayError> shrinkToFit(void) {
    if (this->cap > this->len) {
      T* resized =
          this->allocator->resizeRaw(this->data, this->len * sizeof(T));
      if (resized == nullptr) {
        return Err(DynamicArrayError(DynamicArrayError::BufferResizeFailed));
      }

      this->data = resized;
      this->cap  = this->len;
    }

    return Ok(Void());
  }

private:
  /// Backing allocator used for internal allocations.
  mem::Allocator*                 allocator;

  /// The actual element buffer.
  T*                              data = nullptr;

  /// The length of the array.
  usize                           len  = 0;

  /// The capacity of the array.
  usize                           cap  = 0;

  /// Function to resize the array.
  Result<Void, DynamicArrayError> resize(void) {
    // Resize the buffer to new capacity
    usize new_cap = this->cap * dynamic_array_internal::RESIZE_FACTOR;
    T*    resized =
        (T*)this->allocator->resizeRaw(this->data, new_cap * sizeof(T));
    if (resized == nullptr) {
      return Err(DynamicArrayError(DynamicArrayError::BufferResizeFailed));
    }

    // Set the buffer to the resized one
    this->data = resized;
    this->cap  = new_cap;

    return Ok(Void());
  }
};

} // namespace bl::ds

#endif // !BL_DYNAMIC_ARRAY_H
