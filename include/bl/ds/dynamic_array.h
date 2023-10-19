#ifndef BL_DYNAMIC_ARRAY_H
#define BL_DYNAMIC_ARRAY_H

#include "bl/mem/allocator.h"
#include "bl/primitives.h" // usize

namespace bl::ds {
using namespace primitives;

template <typename T> struct DynamicArray {
public:
  DynamicArray();

  DynamicArray(mem::Allocator* allocator);

  DynamicArray(mem::Allocator* allocator, usize capacity);

  // Disable copy-constructor
  DynamicArray(const DynamicArray&) = delete;
  DynamicArray(DynamicArray&&)      = default;

  T*    getRaw(void) const;

  usize getLen(void) const;

  usize getCap(void) const;

  bool  isEmpty(void) const;

private:
  mem::Allocator* allocator;
  T*              data;
  usize           len;
  usize           cap;
};

} // namespace bl::ds

#endif // !BL_DYNAMIC_ARRAY_H
