#ifndef BL_ALLOCATOR_H
#define BL_ALLOCATOR_H

#include "bl/primitives.h" // usize

namespace bl::mem {

namespace {
using namespace primitives;

typedef void* (*AllocFn)(usize);
typedef void (*DeallocFn)(void*);
typedef void* (*ResizeFn)(void*, usize);
} // namespace

typedef struct Allocator {
public:
  void* allocRaw(usize nbytes) { return this->alloc(nbytes); }
  void  deallocRaw(void* ptr) { this->dealloc(ptr); }
  void* resizeRaw(void* ptr, usize nbytes) { return this->resize(ptr, nbytes); }

protected:
  void*     ctx;
  AllocFn   alloc;
  DeallocFn dealloc;
  ResizeFn  resize;
} Allocator;

} // namespace bl::mem

#endif // !BL_ALLOCATOR_H
