#ifndef BL_STRING_H
#define BL_STRING_H

#include "bl/mem/allocator.h" // Allocator
#include "bl/option.h"        // Option
#include "bl/primitives.h"    // cstr, const_cstr, usize, Void
#include "bl/result.h"        // Error, Result

namespace bl {
using namespace primitives;

// Possible error types returned by `String`'s methods.
struct StringBufferError : public Error {
public:
  enum ErrorType {
    ResizeFailed,
    BufferAllocationFailed,
    BufferDeallocationFailed,
    BufferResizeFailed,
    IndexOutOfBounds,
    StrncpyFailed,
    InvalidCString,
    InvalidString,
    InvalidPop,
  };

  StringBufferError(ErrorType type);

  /// Returns the error message for the current error type.
  const_cstr errMsg(void) const;

private:
  ErrorType type;
};

/// A dynamic string buffer.
struct String {
public:
  /// Creates an empty string with the `mem::CAllocator` as its backing
  /// allocator.
  ///
  /// ## Note
  /// Nothing is allocated until the first push.
  String();

  /// Creates an empty string backed by the given allocator.
  ///
  /// ## Note
  /// Nothing is allocated until the first push.
  ///
  /// ## Panics
  /// - Panics if the provided allocator is null.
  String(mem::Allocator* allocator);

  /// Creates an empty string with the specified capacity, backed by the given
  /// allocator.
  ///
  /// ## Note
  /// If the capacity is `0`, then this just calls the `String(mem::Allocator*)`
  /// constructor (nothing gets allocated).
  ///
  /// ## Panics
  /// - Panics if the provided allocator is null.
  /// - Panics if allocator is unable to allocate space for the buffer.
  String(mem::Allocator* allocator, usize capacity);

  /// Creates a string containing the given C-string, with the given allocator
  /// as its backing allocator.
  ///
  /// ## Panics
  /// - Panics if the provided allocator or the C-string is null.
  /// - Panics if allocator is unable to allocate space for the buffer.
  String(mem::Allocator* allocator, const_cstr str);

  /// Creates a string containing the given C-string.
  ///
  /// ## Note
  /// This overload uses `mem::CAllocator` as its backing allocator.
  ///
  /// ## Panics
  /// - Panics if the C-string is null.
  /// - Panics if allocator is unable to allocate space for the buffer.
  String(const_cstr str);

  /// Clones the string.
  ///
  /// ## Note
  /// The capacity of the cloned string will not be the same as the
  /// original string, but the length and contents will be the same.
  ///
  /// ## Panics
  /// - Panics if allocator is unable to allocate space for the buffer.
  String(const String&);

  /// Deallocates memory used by the string.
  ~String();

  String&                           operator=(const String&) = default;

  /// Operator overload for index operator.
  ///
  /// ## Panics
  /// - Panics if the index is out of the string's bounds.
  char&                             operator[](usize idx);

  /// Returns the underyling string buffer.
  const_cstr                        getRaw(void) const;

  /// Returns the length of the string (doesn't count the null-terminator).
  usize                             getLen(void) const;

  /// Returns the capacity of the string (doesn't count the null-terminator).
  usize                             getCap(void) const;

  /// Checks if the string is empty.
  bool                              isEmpty(void) const;

  /// Removes all of the string's contents, but leaves the capacity unchanged.
  void                              clear(void);

  /// Appends the given character to the end of the string.
  ///
  /// ## Note
  /// This can cause a resize if the string does not have enough capacity.
  Result<Void, StringBufferError>   push(char chr);

  /// Appends the given C-string to the end of the string.
  ///
  /// ## Note
  /// This can cause a resize if the string does not have enough capacity.
  Result<Void, StringBufferError>   push(const_cstr str);

  /// Removes and returns the last character in the string.
  ///
  /// ## Note
  /// If the string is empty, this will return `None`.
  Option<char>                      pop(void);

  /// Inserts the given character at the specified index in the string.
  ///
  /// ## Note
  /// This is an **O(n)** operation since it requires copying every character in
  /// the buffer.
  Result<Void, StringBufferError>   insert(usize idx, char chr);

  /// Inserts the given C-string at the specified index in the string.
  ///
  /// ##Note
  /// This is an **O(n)** operation since it requires copying every character in
  /// the buffer.
  ///
  Result<Void, StringBufferError>   insert(usize idx, const_cstr str);

  /// Removes and returns the character at the specified index from the string.
  ///
  /// ##Note
  /// This is an **O(n)** operation since it requires copying every character in
  /// the buffer.
  Result<char, StringBufferError>   remove(usize idx);

  /// Checks if the string contains the specified sub-string.
  ///
  /// This will return the index where the sub-string was found, or `None` if it
  /// wasn't found.
  Option<i32>                       find(const_cstr substr) const;

  /// Shrinks the capacity of the string to match its length.
  Result<Void, StringBufferError>   shrinkToFit(void);

  /// Splits the string into two at the given index.
  ///
  /// The original string contains bytes in the range `[0, idx)`, and the
  /// returned string contains the bytes in the range `[idx, len)`.
  Result<String, StringBufferError> split(usize idx);

  /// Checks if the two strings are the same.
  ///
  /// ## Note
  /// This only checks for the string contents; the capacity and allocator of
  /// the strings may differ.
  ///
  /// ## Panics
  /// - Panics if the `other` string is null.
  bool                              isSame(String* other) const;

  /// Checks if the two strings are the same.
  ///
  /// This overload check a raw C-string against the string.
  ///
  /// ## Panics
  /// - Panics if the `other` C-string is null.
  bool                              isSame(const_cstr other) const;

private:
  /// Backing allocator used for internal allocations.
  mem::Allocator*                 allocator;

  /// The actual string buffer.
  cstr                            data = nullptr;

  /// The number of bytes allocated by/for the string.
  usize                           cap  = 0;

  /// The length of the string.
  usize                           len  = 0;

  /// Function to resize the string.
  Result<Void, StringBufferError> resize(void);
};

} // namespace bl

#endif // !BL_STRING_H
