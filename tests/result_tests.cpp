#include "bl/primitives.h"
#include "bl/result.h"
#include <algorithm>
#include <cassert>
#include <cstdio>

using namespace bl;

struct StringError : public Error {
  const_cstr errMsg() { return "Test Error!"; }
};

struct NonTrivial {
  int* x = nullptr;

  NonTrivial(int x) {
    int* data = (int*)malloc(sizeof(int));
    *data     = x;
    this->x   = data;
  }

  NonTrivial(const NonTrivial& other) {
    // fprintf(stderr, "\nCopied!\n");
    // fprintf(stderr, "This: %p\n", this->x);
    // fprintf(stderr, "Other: %p\n", other.x);

    if (this->x == nullptr) {
      int* data = (int*)malloc(sizeof(int));
      *data     = *other.x;
      this->x   = data;
      return;
    }

    *this->x = *other.x;
  }

  NonTrivial(NonTrivial&& other) {
    // fprintf(stderr, "\nMoved!\n");
    // fprintf(stderr, "This: %p\n", this->x);
    // fprintf(stderr, "Other: %p\n", other.x);
    this->x = std::move(other.x);
    other.x = nullptr;
  }

  ~NonTrivial() {
    if (x != nullptr) {
      // fprintf(stderr, "Deleted: %p\n", this->x);
      free(this->x);
      this->x = nullptr;
    }
  }

  NonTrivial& operator=(const NonTrivial& other) {
    // fprintf(stderr, "\nCopy Assigned!\n");
    // fprintf(stderr, "Other: %p\n", other.x);

    this->x  = (int*)malloc(sizeof(int));
    *this->x = *other.x;

    // fprintf(stderr, "This: %p\n", this->x);
    return *this;
  }

  NonTrivial& operator=(NonTrivial&& other) {
    // fprintf(stderr, "\nMove Assigned!\n");
    // fprintf(stderr, "Other: %p\n", other.x);

    this->x = other.x;
    other.x = nullptr;
    // fprintf(stderr, "This: %p\n", this->x);
    return *this;
  }
};

template <typename T> using Res = Result<T, StringError>;

int main(void) {
  Res<NonTrivial> res  = Ok(NonTrivial(1));
  Res<int>        res2 = Err(StringError());

  Res<NonTrivial> copy = res;
  Res<int>        move = std::move(res2);
}
