#ifndef BL_ALLOCATOR_H
#define BL_ALLOCATOR_H

#include "bl/primitives.h" // usize

namespace bl::mem {
using namespace primitives;

typedef void* (*AllocFn)(usize);
typedef void (*DeallocFn)(void*);
typedef void* (*ResizeFn)(void*, usize);

struct Allocator {
public:
  inline void* allocRaw(usize nbytes) { return this->alloc(nbytes); }
  inline void  deallocRaw(void* ptr) { this->dealloc(ptr); }
  inline void* resizeRaw(void* ptr, usize nbytes) {
    return this->resize(ptr, nbytes);
  }

protected:
  void*     ctx;
  AllocFn   alloc;
  DeallocFn dealloc;
  ResizeFn  resize;
};

} // namespace bl::mem

#endif // !BL_ALLOCATOR_H
