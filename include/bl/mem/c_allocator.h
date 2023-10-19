#ifndef BL_C_ALLOCATOR_H
#define BL_C_ALLOCATOR_H

#include "bl/mem/allocator.h" // Allocator

#include <cstdlib> // malloc, realloc, free

namespace bl::mem {

struct CAllocator : Allocator {
public:
  CAllocator() {
    this->ctx     = nullptr;
    this->alloc   = std::malloc;
    this->dealloc = std::free;
    this->resize  = std::realloc;
  }
};

} // namespace bl::mem

#endif // !BL_C_ALLOCATOR_H
