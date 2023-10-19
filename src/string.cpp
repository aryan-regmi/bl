#include "bl/string.h"

#include "bl/error.h"           // BL_THROW, resetError
#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator
#include "bl/primitives.h"      // const_cstr, cstr, usize, u8

#include <algorithm>
#include <cstdio>
#include <cstring> // strncpy, strlen, memmove

namespace bl {

namespace {
enum class StringError {
  InvalidAllocator,
  InvalidCString,
  BufferAllocationFailed,
  BufferDeallocationFailed,
  BufferResizeFailed,
  StrncpyFailed,
  ResizeFailed,
};

const_cstr errMsg(StringError err) {
  switch (err) {
  case StringError::InvalidAllocator:
    return "StringError: Invalid Allocator (the allocator was null)";
  case StringError::BufferAllocationFailed:
    return "StringError: Unable to allocate space for the string buffer";
  case StringError::StrncpyFailed:
    return "StringError: `strncpy` failed (returned null)";
  case StringError::ResizeFailed:
    return "StringError: Unable to resize the string";
  case StringError::InvalidCString:
    return "StringError: Invalid C-string (the provided C-string was null)";
  case StringError::BufferDeallocationFailed:
    return "StringError: Unable to deallocate the string buffer";
  case StringError::BufferResizeFailed:
    return "StringError: Unable to resize for the string buffer";
  }

  return nullptr;
}

mem::Allocator DEFAULT_C_ALLOCATOR = mem::CAllocator();

const u8       RESIZE_FACTOR       = 2;
} // namespace

// TODO: Handle indicies that are out of bounds!

String::String() { this->allocator = &DEFAULT_C_ALLOCATOR; }

String::String(mem::Allocator* allocator) {
  // Input validation
  {
    Error::resetError();
    if (allocator == nullptr) {
      BL_THROW(errMsg(StringError::InvalidAllocator));
      return;
    }
  }

  this->allocator = allocator;
}

String::String(mem::Allocator* allocator, usize capacity) : cap(capacity) {
  // Input validation
  {
    Error::resetError();
    if (allocator == nullptr) {
      BL_THROW(errMsg(StringError::InvalidAllocator));
      this->cap = 0;
      return;
    }
  }

  this->allocator = allocator;

  if (capacity != 0) {
    cstr data = (cstr)allocator->allocRaw(capacity + 1);
    if (data == nullptr) {
      BL_THROW(errMsg(StringError::BufferAllocationFailed));
      return;
    }
    this->data           = data;
    this->data[capacity] = '\0';
  }
}

String::String(mem::Allocator* allocator, const_cstr str) {
  // Input validation
  {
    Error::resetError();
    if (allocator == nullptr) {
      BL_THROW(errMsg(StringError::InvalidAllocator));
      return;
    }

    if (str == nullptr) {
      BL_THROW(errMsg(StringError::InvalidCString));
      return;
    }
  }

  // Create new string with `len` capacity
  usize len       = strlen(str);
  this->allocator = allocator;
  this->cap       = len;
  this->len       = len;
  cstr data       = (cstr)allocator->allocRaw(len + 1);
  if (data == nullptr) {
    BL_THROW(errMsg(StringError::BufferAllocationFailed));
    this->cap = 0;
    this->len = 0;
    return;
  }
  this->data  = data;

  // Copy `str` to string
  cstr copied = strncpy(this->data, str, len);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    this->allocator->deallocRaw(this->data);
    this->data = nullptr;
    this->len  = 0;
    this->cap  = 0;
    return;
  }
  copied[len] = '\0';
  this->data  = copied;
}

String::String(const_cstr str) {
  Error::resetError();
  if (str == nullptr) {
    BL_THROW(errMsg(StringError::InvalidCString));
    return;
  }

  // Create new string with `len` capacity
  usize len       = strlen(str);
  this->allocator = &DEFAULT_C_ALLOCATOR;
  this->cap       = len;
  this->len       = len;
  cstr data       = (cstr)allocator->allocRaw(len + 1);
  if (data == nullptr) {
    BL_THROW(errMsg(StringError::BufferAllocationFailed));
    this->cap = 0;
    this->len = 0;
    return;
  }
  this->data  = data;

  // Copy `str` to string
  cstr copied = strncpy(this->data, str, len);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    this->allocator->deallocRaw(this->data);
    this->data = nullptr;
    this->len  = 0;
    this->cap  = 0;
    return;
  }
  copied[len] = '\0';
  this->data  = copied;
}

String::String(const String& other) {
  Error::resetError();

  this->allocator = other.allocator;
  this->len       = other.len;
  this->cap       = other.len;

  cstr data       = (cstr)this->allocator->allocRaw(this->len + 1);
  if (data == nullptr) {
    BL_THROW(errMsg(StringError::BufferAllocationFailed));
    return;
  }
  this->data  = data;

  cstr copied = strncpy(this->data, other.data, this->len);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  copied[this->len] = '\0';
  this->data        = copied;
}

String::~String() {
  if (this->cap != 0) {
    this->allocator->deallocRaw(this->data);
  }
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

void String::push(char chr) {
  Error::resetError();

  // Resize if necessary
  usize new_len = this->len + 1;
  if (new_len > this->cap) {
    this->resize();
    if (Error::isError()) {
      BL_THROW(errMsg(StringError::ResizeFailed));
      return;
    }
  }

  this->data[this->len]  = chr;
  this->data[new_len]    = '\0';
  this->len             += 1;
}

void String::push(const_cstr str) {
  // Input validation
  {
    Error::resetError();

    if (str == nullptr) {
      BL_THROW(errMsg(StringError::InvalidCString));
      return;
    }
  }

  usize len = strlen(str);
  for (usize i = 0; i < len; i++) {
    this->push(str[i]);
    if (Error::isError()) {
      return;
    }
  }
}

char String::pop(void) {
  char popped            = this->data[this->len - 1];
  this->len             -= 1;
  this->data[this->len]  = '\0';
  return popped;
}

void String::insert(usize idx, char chr) {
  Error::resetError();

  if (idx == this->len - 1) {
    this->push(chr);
    return;
  }

  // Copy second half of the string
  const usize split_size = this->len - idx;
  cstr        split      = (cstr)this->allocator->allocRaw(split_size + 1);
  cstr        copied     = strncpy(split, this->data + idx, split_size);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  copied[split_size] = '\0';
  split              = copied;

  // Resize if necessary
  usize new_len      = this->len + 1;
  if (new_len > this->cap) {
    this->resize();
    if (Error::isError()) {
      BL_THROW(errMsg(StringError::ResizeFailed));
      return;
    }
  }

  // Append `chr` then append the rest of the string
  this->data[idx]   = chr;
  cstr copied_split = strncpy(this->data + idx + 1, split, split_size);
  if (copied_split == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  this->allocator->deallocRaw(split);
  this->len             += 1;
  this->data[this->len]  = '\0';
}

void String::insert(usize idx, const_cstr str) {
  // Input validation
  {
    Error::resetError();

    if (str == nullptr) {
      BL_THROW(errMsg(StringError::InvalidCString));
      return;
    }
  }

  if (idx == this->len - 1) {
    this->push(str);
    return;
  }

  // Copy second half of the string
  const usize split_size = this->len - idx;
  cstr        split      = (cstr)this->allocator->allocRaw(split_size + 1);
  cstr        copied     = strncpy(split, this->data + idx, split_size);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  copied[split_size] = '\0';
  split              = copied;

  // FIXME: The resize might not be enough (resize in a while loop)
  //
  // Resize if necessary
  usize len          = strlen(str);
  usize new_len      = this->len + len;
  if (new_len > this->cap) {
    this->resize();
    if (Error::isError()) {
      BL_THROW(errMsg(StringError::ResizeFailed));
      return;
    }
  }

  // Append `str` then append the rest of the string
  cstr copied_str = strncpy(this->data + idx, str, len);
  if (copied_str == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  cstr copied_split = strncpy(this->data + idx + len, split, split_size);
  if (copied_split == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return;
  }
  this->allocator->deallocRaw(split);
  this->len             += len;
  this->data[this->len]  = '\0';
}

char String::remove(usize idx) {
  if (idx == this->len - 1) {
    return this->pop();
  }

  char  removed   = this->data[idx];

  usize move_size = this->len - idx;
  cstr moved = (cstr)memmove(this->data + idx, this->data + idx + 1, move_size);
  if (moved == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return '\0';
  }
  this->len -= 1;

  return removed;
}

void String::resize(void) {
  // Create new buffer with increased capacity
  usize new_cap = this->cap * RESIZE_FACTOR;
  cstr  new_buf = (cstr)this->allocator->allocRaw(new_cap + 1);
  if (new_buf == nullptr) {
    BL_THROW(errMsg(StringError::BufferAllocationFailed));
    return;
  }

  // Copy original data to new buffer
  cstr copied = strncpy(new_buf, this->data, this->len);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    this->allocator->deallocRaw(new_buf);
    return;
  }
  copied[this->len] = '\0';

  // Free old buffer
  this->allocator->deallocRaw(this->data);
  if (Error::isError()) {
    BL_THROW(errMsg(StringError::BufferDeallocationFailed));
    return;
  }

  this->data = copied;
  this->cap  = new_cap;
}

i32 String::find(const_cstr substr) const {
  // Input validation
  {
    Error::resetError();

    if (substr == nullptr) {
      BL_THROW(errMsg(StringError::InvalidCString));
      return -1;
    }
  }

  cstr found = strstr(this->data, substr);
  if (found == NULL) {
    return -1;
  }

  return (i32)(found - this->data);
}

void String::shrinkToFit(void) {
  if (this->cap > this->len) {
    cstr resized = (cstr)this->allocator->resizeRaw(this->data, this->len + 1);
    if (resized == nullptr) {
      BL_THROW(errMsg(StringError::InvalidCString));
      return;
    }

    this->data = resized;
    this->cap  = this->len;
  }
}

String String::splitOff(usize idx) {
  if (idx == 0) {
    String* split = this;
    this->clear();
    return *split;
  }

  if (idx == this->len - 1) {
    return String(this->allocator);
  }

  // TODO: Finish impl

  // Create new string with enough capacity to hold the split
  // String split = String(&DEFAULT_C_ALLOCATOR, this->len - idx);
  // usize split_size = this->len - idx;
  // cstr  split_data = (cstr)this->allocator->allocRaw(split_size + 1);
  // if (split_data == nullptr) {
  //   BL_THROW(errMsg(StringError::BufferAllocationFailed));
  //   return String();
  // }
  // split_data[split_size] = '\0';
  // String split           = String();
  // split.allocator        = this->allocator;
  // split.cap              = split_size;
  // split.len              = split_size;
  // split.data             = split_data;

  // Copy from original into split

  // return split;
}

} // namespace bl
