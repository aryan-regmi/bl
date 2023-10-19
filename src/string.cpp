#include "bl/string.h"

#include "bl/error.h"           // BL_THROW, resetError
#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator
#include "bl/primitives.h"      // const_cstr, cstr, usize, u8

#include <cstring> // strncpy, strlen

namespace bl {

namespace {
enum class StringError {
  InvalidAllocator,
  InvalidCString,
  BufferAllocationFailed,
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
  }
}

mem::Allocator DEFAULT_C_ALLOCATOR = mem::CAllocator();

const u8       RESIZE_FACTOR       = 2;
} // namespace

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

  if (capacity != 0) {
    cstr data = (cstr)allocator->allocRaw(capacity + 1);
    if (data == nullptr) {
      BL_THROW(errMsg(StringError::BufferAllocationFailed));
    }

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

String::~String() {
  this->allocator->deallocRaw(this->data);
  this->len       = 0;
  this->cap       = 0;
  this->allocator = nullptr;
}

const_cstr String::getRaw(void) const { return this->data; }

usize      String::getLen(void) const { return this->len; }

usize      String::getCap(void) const { return this->cap; }

bool       String::isEmpty(void) const { return this->len == 0; }

String     String::clone(void) const {
  Error::resetError();

  String copy   = String(this->allocator, this->len + 1);

  cstr   copied = std::strncpy(copy.data, this->data, this->len);
  if (copied == nullptr) {
    BL_THROW(errMsg(StringError::StrncpyFailed));
    return String();
  }
  copied[this->len] = '\0';
  copy.data         = copied;

  return copy;
}

void String::clear(void) {
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
    }
  }

  this->data[this->len]  = chr;
  this->data[new_len]    = '\0';
  this->len             += 1;
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

  this->data        = copied;
  this->cap         = new_cap;
}

} // namespace bl
