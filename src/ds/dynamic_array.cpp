#include "bl/ds/dynamic_array.h"

#include "bl/mem/c_allocator.h" // CAllocator

namespace bl::ds {

namespace dynamic_array_internal {

const_cstr errMsg(DynamicArrayError err) {
  switch (err) {
  case DynamicArrayError::InvalidAllocator:
    return "DynamicArrayError: Invalid Allocator (the allocator was null)";
  case DynamicArrayError::BufferAllocationFailed:
    return "DynamicArrayError: Unable to allocate space for the array buffer";
  case DynamicArrayError::ResizeFailed:
    return "DynamicArrayError: Unable to resize the array";
  case DynamicArrayError::BufferDeallocationFailed:
    return "DynamicArrayError: Unable to deallocate the array buffer";
  case DynamicArrayError::BufferResizeFailed:
    return "DynamicArrayError: Unable to resize for the array buffer";
  case DynamicArrayError::IndexOutOfBounds:
    return "DynamicArrayError: The specified index was out of the array's "
           "bounds";
  case DynamicArrayError::InvalidPop:
    return "DynamicArrayError: Tried `popping` from an empty array";
  case DynamicArrayError::MemcpyFailed:
    return "DynamicArrayError: `memcpy` failed (returned null)";
  case DynamicArrayError::InvalidArray:
    return "DynamicArrayError: The array used for initalization was invalid "
           "(must be non-null array type)";
  }

  return nullptr;
}

mem::Allocator DEFAULT_C_ALLOCATOR = mem::CAllocator();

const u8       RESIZE_FACTOR       = 2;
} // namespace dynamic_array_internal

} // namespace bl::ds
