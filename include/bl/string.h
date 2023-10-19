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

  /// Clones the string.
  ///
  /// @note The capacity of the cloned string will not be the same as the
  /// original string, but the length and contents will be the same.
  String(const String&);

  /// Deallocates memory used by the string.
  ~String();

  /// Returns the underyling string buffer.
  const_cstr getRaw(void) const;

  /// Returns the length of the string (doesn't count the null-terminator).
  usize      getLen(void) const;

  /// Returns the capacity of the string (doesn't count the null-terminator).
  usize      getCap(void) const;

  /// Checks if the string is empty.
  bool       isEmpty(void) const;

  /// Removes all of the string's contents, but leaves the capacity unchanged.
  void       clear(void);

  /// Appends the given character to the end of the string.
  ///
  /// @note This can cause a resize if the string does not have enough capacity.
  void       push(char chr);

  /// Appends the given C-string to the end of the string.
  ///
  /// @note This can cause a resize if the string does not have enough capacity.
  void       push(const_cstr str);

  /// Removes and returns the last character in the string.
  ///
  /// @note If the string is empty, this will return the null-terminator
  /// (`'\0'`) and throw an error.
  char       pop(void);

  /// Inserts the given character at the specified index in the string.
  ///
  /// @note This is an **O(n)** operation since it requires copying every
  /// character in the buffer.
  void       insert(usize idx, char chr);

  /// Inserts the given C-string at the specified index in the string.
  ///
  /// @note This is an **O(n)** operation since it requires copying every
  /// character in the buffer.
  void       insert(usize idx, const_cstr str);

  /// Removes and returns the character at the specified index from the string.
  ///
  /// @note This is an **O(n)** operation since it requires copying every
  /// character in the buffer.
  char       remove(usize idx);

  /// Checks if the string contains the specified sub-string.
  ///
  /// This will return the index where the sub-string was found, or `-1` if it
  /// wasn't found.
  i32        find(const_cstr substr) const;

  /// Shrinks the capacity of the string to match its length.
  void       shrinkToFit(void);

  /// Splits the string into two at the given index.
  ///
  /// The original string contains bytes in the range `[0, idx)`, and the
  /// returned string contains the bytes in the range `[idx, len)`.
  String     split(usize idx);

  /// Checks if the two strings are the same.
  ///
  /// @note This only checks for the string contents; the capacity and allocator
  /// of the strings may differ.
  bool       isSame(String* other) const;

  /// Checks if the two strings are the same.
  ///
  /// This overload check a raw C-string against the string.
  bool       isSame(const_cstr other) const;

  // Operator overload for index operator.
  char       operator[](usize idx);

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
