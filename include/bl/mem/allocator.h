#ifndef BL_ALLOCATOR_H
#define BL_ALLOCATOR_H

#include "bl/primitives.h" // usize
#include <cstdio>

namespace bl::mem {
using namespace primitives;

typedef void* (*AllocFn)(usize);
typedef void (*DeallocFn)(void*);
typedef void* (*ResizeFn)(void*, usize);

/// An interface for allocators.
struct Allocator {
public:
  /// Allocates the specified number of bytes.
  inline void* allocRaw(usize nbytes) { return this->alloc(nbytes); }

  /// Deallocates the given pointer, freeing any allocated memory at that
  /// location.
  inline void  deallocRaw(void* ptr) { this->dealloc(ptr); }

  /// Resizes the given pointer to the specified size (in bytes).
  inline void* resizeRaw(void* ptr, usize nbytes) {
    return this->resize(ptr, nbytes);
  }

protected:
  /// Allocator specific context (useful for non-global allocators).
  void*     ctx;

  /// Function used to make allocations.
  AllocFn   alloc;

  /// Function used to free allocations.
  DeallocFn dealloc;

  /// Function used to resize allocations.
  ResizeFn  resize;
};

} // namespace bl::mem

#endif // !BL_ALLOCATOR_H
