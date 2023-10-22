#include "bl/panic.h"

#include "bl/primitives.h" // usize, const_cstr

#include <cassert> // assert
#include <cstdio>  // fprintf, stderr
#include <cstdlib> // abort

namespace bl {

// TODO: Make this thread safe (`fflush` after locking mutex?)

void panic(const_cstr filename, usize line, const_cstr msg) noexcept {
  int printed = fprintf(stderr, "[PANIC] %s:%zu -> %s\n", filename, line, msg);
  assert(printed > 0);
}

} // namespace bl
