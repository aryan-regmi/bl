#include "bl/string.h"

#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator
#include "bl/option.h"          // Option, Some, None
#include "bl/panic.h"           // BL_PANIC, panic
#include "bl/primitives.h"      // const_cstr, cstr, usize, u8, Void
#include "bl/result.h"          // Result, Ok, Err

#include <algorithm>
#include <cstdio>
#include <cstdlib> // abort
#include <cstring> // strncpy, strlen, memmove

// TODO: Replace raw casts with static_casts

namespace bl {
namespace {
mem::Allocator DEFAULT_C_ALLOCATOR = mem::CAllocator();
const u8       RESIZE_FACTOR       = 2;
} // namespace

StringBufferError::StringBufferError(ErrorType type) : type(type) {}

const_cstr StringBufferError::errMsg(void) const {
  switch (this->type) {
  case ErrorType::BufferAllocationFailed:
    return "StringError: Unable to allocate space for the string buffer";
  case ErrorType::StrncpyFailed:
    return "StringError: `strncpy` failed (returned null)";
  case ErrorType::ResizeFailed:
    return "StringError: Unable to resize the string";
  case ErrorType::InvalidCString:
    return "StringError: Invalid C-string (the provided C-string was null)";
  case ErrorType::InvalidString:
    return "StringError: Invalid string (the provided string was null)";
  case ErrorType::BufferDeallocationFailed:
    return "StringError: Unable to deallocate the string buffer";
  case ErrorType::BufferResizeFailed:
    return "StringError: Unable to resize for the string buffer";
  case ErrorType::IndexOutOfBounds:
    return "StringError: The specified index was out of the string's bounds";
  case ErrorType::InvalidPop:
    return "StringError: Tried `popping` from an empty string";
  }

  BL_PANIC("Unreachable");
}

String::String() { this->allocator = &DEFAULT_C_ALLOCATOR; }

String::String(mem::Allocator* allocator) {
  // Input validation
  {
    if (allocator == nullptr) {
      BL_PANIC("Invalid Allocator (the allocator was null)");
    }
  }

  this->allocator = allocator;
}

String::String(mem::Allocator* allocator, usize capacity) : cap(capacity) {
  // Input validation
  {
    if (allocator == nullptr) {
      this->cap = 0;
      BL_PANIC("Invalid Allocator (the allocator was null)");
    }
  }

  this->allocator = allocator;

  if (capacity != 0) {
    cstr data = (cstr)allocator->allocRaw(capacity + 1);
    if (data == nullptr) {
      BL_PANIC("Unable to allocate space for the string buffer");
    }
    this->data           = data;
    this->data[capacity] = '\0';
  }
}

String::String(mem::Allocator* allocator, const_cstr str) {
  // Input validation
  {
    if (allocator == nullptr) {
      BL_PANIC("Invalid Allocator (the allocator was null)");
    }

    if (str == nullptr) {
      BL_PANIC("Invalid C-String (the provided C-string was null)");
    }
  }

  // Create new string with `len` capacity
  usize len       = strlen(str);
  this->allocator = allocator;
  this->cap       = len;
  this->len       = len;
  cstr data       = (cstr)allocator->allocRaw(len + 1);
  if (data == nullptr) {
    this->cap = 0;
    this->len = 0;
    BL_PANIC("Unable to allocate space for the string buffer");
  }
  this->data  = data;

  // Copy `str` to string
  cstr copied = strncpy(this->data, str, len);
  if (copied == nullptr) {
    this->allocator->deallocRaw(this->data);
    this->data = nullptr;
    this->len  = 0;
    this->cap  = 0;
    BL_PANIC("Unable to copy string (`strncpy` returned NULL)");
  }
  copied[len] = '\0';
  this->data  = copied;
}

String::String(const_cstr str) {
  if (str == nullptr) {
    BL_PANIC("Invalid C-String (the provided C-string was null)");
  }

  // Create new string with `len` capacity
  usize len       = strlen(str);
  this->allocator = &DEFAULT_C_ALLOCATOR;
  this->cap       = len;
  this->len       = len;
  cstr data       = (cstr)allocator->allocRaw(len + 1);
  if (data == nullptr) {
    this->cap = 0;
    this->len = 0;
    BL_PANIC("Unable to allocate space for the string buffer");
  }
  this->data  = data;

  // Copy `str` to string
  cstr copied = strncpy(this->data, str, len);
  if (copied == nullptr) {
    this->allocator->deallocRaw(this->data);
    this->data = nullptr;
    this->len  = 0;
    this->cap  = 0;
    BL_PANIC("Unable to copy string (`strncpy` returned NULL)");
  }
  copied[len] = '\0';
  this->data  = copied;
}

String::String(const String& other) {
  this->len = other.len;
  this->cap = other.len;

  if (other.allocator != nullptr) {
    this->allocator = other.allocator;
  }

  if (this->cap != 0) {
    cstr data = (cstr)this->allocator->allocRaw(this->len + 1);
    if (data == nullptr) {
      this->len = 0;
      this->cap = 0;
      BL_PANIC("Unable to allocate space for the string buffer");
    }
    this->data  = data;

    cstr copied = strncpy(this->data, other.data, this->len);
    if (copied == nullptr) {
      this->len = 0;
      this->cap = 0;
      this->allocator->deallocRaw(this->data);
      this->data = nullptr;
      BL_PANIC("Unable to copy string (`strncpy` returned NULL)");
    }
    copied[this->len] = '\0';
    this->data        = copied;
  }
}

String::String(String&& other) {
  this->data      = other.data;
  this->len       = other.len;
  this->cap       = other.cap;
  this->allocator = other.allocator;

  other.data      = nullptr;
  other.len       = 0;
  other.cap       = 0;
  other.allocator = nullptr;
}

String::~String() {
  if (this->cap != 0 && this->data != nullptr) {
    this->allocator->deallocRaw(this->data);
    this->data = nullptr;
    this->len  = 0;
    this->cap  = 0;
  }
}

String& String::operator=(const String& other) {
  this->allocator = other.allocator;
  this->len       = other.len;
  this->cap       = other.len;

  cstr data       = (cstr)this->allocator->allocRaw(this->len + 1);
  if (data == nullptr) {
    BL_PANIC("Unable to allocate space for the string buffer");
  }

  cstr copied = strncpy(data, other.data, this->len);
  if (copied == nullptr) {
    BL_PANIC("Unable to copy string (`strncpy` returned NULL)");
  }
  copied[this->len] = '\0';

  this->data        = copied;
  return *this;
}

String& String::operator=(String&& other) {
  // Copy other string's info
  this->allocator = other.allocator;
  this->len       = other.len;
  this->cap       = other.len;
  this->data      = std::move(other.data);

  // Reset other string
  other.allocator = nullptr;
  other.len       = 0;
  other.cap       = 0;
  other.data      = nullptr;

  return *this;
}

const_cstr String::getRaw(void) const { return this->data; }

usize      String::getLen(void) const { return this->len; }

usize      String::getCap(void) const { return this->cap; }

bool       String::isEmpty(void) const { return this->len == 0; }

void       String::clear(void) {
  if (this->len != 0) {
    this->data[0] = '\0';
    this->len     = 0;
  }
}

Result<Void, StringBufferError> String::push(char chr) {
  // Allocate on first push
  if (this->cap == 0) {
    this->cap = 1;
    cstr data = (cstr)this->allocator->allocRaw(this->cap + 1);
    if (data == nullptr) {
      this->cap = 0;
      return Err(StringBufferError(StringBufferError::BufferAllocationFailed));
    }
    this->data = data;
  }

  // Resize if necessary
  usize new_len = this->len + 1;
  if (new_len > this->cap) {
    Result<Void, StringBufferError> resized = this->resize();
    if (!resized) {
      return Err(StringBufferError(StringBufferError::ResizeFailed));
    }
  }

  this->data[this->len]  = chr;
  this->data[new_len]    = '\0';
  this->len             += 1;
  return Ok(Void());
}

Result<Void, StringBufferError> String::push(const_cstr str) {
  // Input validation
  {
    if (str == nullptr) {
      return Err(StringBufferError(StringBufferError::InvalidCString));
    }
  }

  const usize len = strlen(str);

  // Allocate on first push
  if (this->cap == 0) {
    this->cap = len;
    cstr data = (cstr)this->allocator->allocRaw(len + 1);
    if (data == nullptr) {
      this->cap = 0;
      return Err(StringBufferError(StringBufferError::BufferAllocationFailed));
    }
    this->data = data;
  }

  // Push all the given string's characters `this` buffer
  for (usize i = 0; i < len; i++) {
    Result<Void, StringBufferError> push_res = this->push(str[i]);
    if (!push_res) {
      return push_res;
    }
  }

  return Ok(Void());
}

Option<char> String::pop(void) {
  if (this->len == 0) {
    return None();
  }

  char popped            = this->data[this->len - 1];
  this->len             -= 1;
  this->data[this->len]  = '\0';
  return Some(popped);
}

Result<Void, StringBufferError> String::insert(usize idx, char chr) {
  // Input validation
  {
    if (idx > this->len - 1) {
      return Err(StringBufferError(StringBufferError::IndexOutOfBounds));
    }
  }

  if (idx == this->len - 1) {
    return this->push(chr);
  }

  // Copy second half of the string
  const usize split_size = this->len - idx;
  cstr        split      = (cstr)this->allocator->allocRaw(split_size + 1);
  cstr        copied     = strncpy(split, this->data + idx, split_size);
  if (copied == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  copied[split_size] = '\0';
  split              = copied;

  // Resize if necessary
  usize new_len      = this->len + 1;
  if (new_len > this->cap) {
    Result<Void, StringBufferError> resized = this->resize();
    if (!resized) {
      return resized;
    }
  }

  // Append `chr` then append the rest of the string
  this->data[idx]   = chr;
  cstr copied_split = strncpy(this->data + idx + 1, split, split_size);
  if (copied_split == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  this->allocator->deallocRaw(split);
  this->len             += 1;
  this->data[this->len]  = '\0';

  return Ok(Void());
}

Result<Void, StringBufferError> String::insert(usize idx, const_cstr str) {
  // Input validation
  {
    if (str == nullptr) {
      return Err(StringBufferError(StringBufferError::InvalidCString));
    }

    if (idx > this->len - 1) {
      return Err(StringBufferError(StringBufferError::IndexOutOfBounds));
    }
  }

  if (idx == this->len - 1) {
    return this->push(str);
  }

  // Copy second half of the string
  const usize split_size = this->len - idx;
  cstr        split      = (cstr)this->allocator->allocRaw(split_size + 1);
  cstr        copied     = strncpy(split, this->data + idx, split_size);
  if (copied == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  copied[split_size] = '\0';
  split              = copied;

  // Resize if necessary
  usize len          = strlen(str);
  usize new_len      = this->len + len;
  while (new_len > this->cap) {
    Result<Void, StringBufferError> resized = this->resize();
    if (!resized) {
      return resized;
    }
  }

  // Append `str` then append the rest of the string
  cstr copied_str = strncpy(this->data + idx, str, len);
  if (copied_str == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
    // BL_THROW(errMsg(StringError::StrncpyFailed));
    // return;
  }
  cstr copied_split = strncpy(this->data + idx + len, split, split_size);
  if (copied_split == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  this->allocator->deallocRaw(split);
  this->len             += len;
  this->data[this->len]  = '\0';

  return Ok(Void());
}

Result<char, StringBufferError> String::remove(usize idx) {
  // Input validation
  {
    if (idx > this->len - 1) {
      return Err(StringBufferError(StringBufferError::IndexOutOfBounds));
    }
  }

  if (idx == this->len - 1) {
    Option<char> popped = this->pop();
    if (!popped) {
      return Err(StringBufferError(StringBufferError::InvalidPop));
    }

    return Ok(popped.unwrap());
  }

  char  removed   = this->data[idx];

  usize move_size = this->len - idx;
  cstr moved = (cstr)memmove(this->data + idx, this->data + idx + 1, move_size);
  if (moved == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  this->len -= 1;

  return Ok(removed);
}

Option<i32> String::find(const_cstr substr) const {
  // Input validation
  {
    if (substr == nullptr) {
      return None();
    }
  }

  cstr found = strstr(this->data, substr);
  if (found == NULL) {
    return None();
  }

  return Some((i32)(found - this->data));
}

Result<Void, StringBufferError> String::shrinkToFit(void) {
  if (this->cap > this->len) {
    cstr resized = (cstr)this->allocator->resizeRaw(this->data, this->len + 1);
    if (resized == nullptr) {
      return Err(StringBufferError(StringBufferError::BufferResizeFailed));
    }

    this->data = resized;
    this->cap  = this->len;
  }

  return Ok(Void());
}

Result<String, StringBufferError> String::split(usize idx) {
  // Input validation
  {
    if (idx > this->len - 1) {
      return Err(StringBufferError(StringBufferError::IndexOutOfBounds));
    }
  }

  if (idx == 0) {
    String split = *this;
    this->clear();
    return Ok(split);
  }

  if (idx == this->len - 1) {
    return Ok(String(this->allocator));
  }

  // Create new string with enough capacity to hold the split
  usize  split_size = this->len - idx;
  String split      = String(this->allocator, split_size);

  // Copy from original into split
  cstr   copied     = strncpy(split.data, this->data + idx, split_size);
  if (copied == nullptr) {
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  copied[split_size] = '\0';
  split.data         = copied;
  split.len          = split_size;

  // Remove split contents from original string
  this->data[idx]    = '\0';
  this->len          = idx;

  return Ok(split);
}

bool String::isSame(String* other) const {
  // Input validation
  {
    if (other == nullptr) {
      BL_PANIC(StringBufferError(StringBufferError::InvalidString).errMsg());
    }
  }

  if (this->len != other->len) {
    return false;
  }

  return strncmp(this->data, other->data, this->len) == 0;
}

bool String::isSame(const_cstr other) const {
  // Input validation
  {
    if (other == nullptr) {
      BL_PANIC(StringBufferError(StringBufferError::InvalidString).errMsg());
    }
  }

  return strncmp(this->data, other, this->len) == 0;
}

char& String::operator[](usize idx) {
  // Input validation
  {
    if (idx > this->len - 1) {
      BL_PANIC(StringBufferError(StringBufferError::InvalidString).errMsg());
    }
  }

  return this->data[idx];
}

Result<Void, StringBufferError> String::resize(void) {
  // Create new buffer with increased capacity
  usize new_cap = this->cap * RESIZE_FACTOR;
  cstr  new_buf = (cstr)this->allocator->allocRaw(new_cap + 1);
  if (new_buf == nullptr) {
    return Err(StringBufferError(StringBufferError::BufferAllocationFailed));
  }

  // Copy original data to new buffer
  cstr copied = strncpy(new_buf, this->data, this->len);
  if (copied == nullptr) {
    this->allocator->deallocRaw(new_buf);
    return Err(StringBufferError(StringBufferError::StrncpyFailed));
  }
  copied[this->len] = '\0';

  // Free old buffer
  this->allocator->deallocRaw(this->data);

  this->data = copied;
  this->cap  = new_cap;

  return Ok(Void());
}

} // namespace bl
