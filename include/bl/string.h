#ifndef BL_STRING_H
#define BL_STRING_H

#include "bl/mem/allocator.h" // Allocator
#include "bl/primitives.h"    // cstr, usize

#include <cstring>

namespace bl {
using namespace primitives;

// TODO: Add error handling (error free reporting)!

struct String {
public:
  /// Creates an empty string with the `mem::CAllocator` as its backing
  /// allocator.
  String();

  String(mem::Allocator* allocator);

  String(mem::Allocator* allocator, usize capacity);

  String(const String&) = delete;
  String(String&&)      = default;

  usize      getLen(void) const { return this->len; }

  usize      getCap(void) const { return this->cap; }

  const_cstr getRaw(void) const { return this->data; }

  String     clone(void) const {
    String copy = String(this->allocator, this->len + 1);

    cstr   copied =
        std::strncpy(copy.data, this->data, this->len); // TODO: Handle error
    copied[this->len] = '\0';
    copy.data         = copied;

    return copy;
  }

private:
  mem::Allocator* allocator;
  cstr            data          = nullptr;
  usize           cap           = 0;
  usize           len           = 0;

  static const u8 RESIZE_FACTOR = 2;
  void            resize(void) {
    // TODO: Implement
  }
};

} // namespace bl

#endif // !BL_STRING_H
