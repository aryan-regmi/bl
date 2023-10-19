#include "bl/string.h"

#include "bl/error.h"           // BL_THROW
#include "bl/mem/allocator.h"   // Allocator
#include "bl/mem/c_allocator.h" // CAllocator

namespace bl {

String::String() {
  mem::Allocator allocator = mem::CAllocator();
  this->allocator          = &allocator;
}

String::String(mem::Allocator* allocator) {
  if (allocator == nullptr) {
    BL_THROW("Invalid Allocator");
  }
}

String::String(mem::Allocator* allocator, usize capacity)
    : allocator(allocator), cap(capacity) {
  if (capacity != 0) {
    this->data = (cstr)allocator->allocRaw(capacity + 1); // TODO: Handle error
    this->data[capacity] = '\0';
  }
}

} // namespace bl
