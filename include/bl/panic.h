#ifndef BL_ERROR_H
#define BL_ERROR_H

#include "bl/primitives.h" // cstr, usize

namespace bl {
using namespace primitives;

/// Panics with the given message.
void panic(const_cstr filename, usize line, const_cstr msg) noexcept;

/// Convenience macro for `panic`.
#define BL_PANIC(msg)                                                          \
  do {                                                                         \
    panic(__FILE__, __LINE__, msg);                                            \
    abort();                                                                   \
  } while (0);

} // namespace bl

#endif // !BL_ERROR_H
