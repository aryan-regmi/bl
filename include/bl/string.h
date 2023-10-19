#ifndef BL_STRING_H
#define BL_STRING_H

#include "bl/mem/allocator.h" // Allocator
#include "bl/primitives.h"    // cstr, const_cstr, usize

namespace bl {
using namespace primitives;

struct String {
public:
  /// Creates an empty string with the `mem::CAllocator` as its backing
  /// allocator.
  ///
  /// @note Nothing is allocated until the first push.
  String();

  /// Creates an empty string backed by the given allocator.
  ///
  /// @note Nothing is allocated until the first push.
  String(mem::Allocator* allocator);

  /// Creates an empty string with the specified capacity, backed by the given
  /// allocator.
  String(mem::Allocator* allocator, usize capacity);

  /// Creates a string containing the given C-string, with the given allocator
  /// as its backing allocator.
  String(mem::Allocator* allocator, const_cstr str);

  /// Creates a string containing the given C-string.
  ///
  /// @note This overload uses `mem::CAllocator` as its backing allocator.
  String(const_cstr str);

  /// Deallocates memory used by the string.
  ~String();

  // Disable copy-constructor
  String(const String&) = delete;
  String(String&&)      = default;

  /// Returns the underyling string buffer.
  const_cstr getRaw(void) const;

  /// Returns the length of the string (doesn't count the null-terminator).
  usize      getLen(void) const;

  /// Returns the capacity of the string (doesn't count the null-terminator).
  usize      getCap(void) const;

  /// Checks if the string is empty.
  bool       isEmpty(void) const;

  /// Clones the string.
  ///
  /// @note The capacity of the cloned string will not be the same as the
  /// original string, but the length and contents will be the same.
  String     clone(void) const;

  /// Removes all of the string's contents, but leaves the capacity unchanged.
  void       clear(void);

  /// Appends the given character to the end of the string.
  void       push(char chr);

private:
  /// Backing allocator used for internal allocations.
  mem::Allocator* allocator;

  /// The actual string buffer.
  cstr            data = nullptr;

  /// The number of bytes allocated by/for the string.
  usize           cap  = 0;

  /// The length of the string.
  usize           len  = 0;

  /// Function to resize the string.
  void            resize(void);
};

} // namespace bl

#endif // !BL_STRING_H
