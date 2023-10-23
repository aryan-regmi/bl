#include "bl/option.h"

#include "bl/primitives.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace bl;

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

void createTest(void) {
  Option<int> opt = Some(4);
  assert(opt.isSome());
  assert(!opt.isNone());
  assert(opt.someOr(Some(3)).unwrap() == 4);
  assert(opt.unwrap() == 4);

  Option<int> opt2 = None();
  assert(!opt2.isSome());
  assert(opt2.isNone());
  assert(opt2.someOr(Some(3)).unwrap() == 3);
  assert(opt2.unwrapOr(2) == 2);
  // assert(opt2.unwrap(__FILE__, __LINE__) == 4); // Panics!

  Option<NonTrivial> opt3 = Some(NonTrivial(1));
  assert(opt3.isSome());
  assert(!opt3.isNone());
  assert(*opt3.someOr(Some(NonTrivial(3))).unwrap().x == 1);
  assert(*opt3.unwrap().x == 1);

  Option<NonTrivial> opt4 = None();
  assert(!opt4.isSome());
  assert(opt4.isNone());
  assert(*opt4.someOr(Some(NonTrivial(3))).unwrap().x == 3);
  assert(*opt4.unwrapOr(2).x == 2);
  // assert(*opt4.unwrap(__FILE__, __LINE__).x == 4); // Panics!
}

void mapTest(void) {
  Option<int>        opt    = Some(3);

  Option<const_cstr> mapped = opt.map(+[](int x) {
    if (x == 3) {
      return "Three";
    }
    return "Other";
  });
  assert(mapped.isSome());
  assert(strcmp(mapped.unwrap(), "Three") == 0);
}

void copyTest(void) {
  Option<NonTrivial> opt = Some(NonTrivial(1));
  assert(opt.isSome());
  assert(*opt.unwrap().x == 1);

  Option<NonTrivial> copied = opt;
  assert(copied.isSome());
  assert(*copied.unwrap().x == 1);
}

void replaceTest(void) {
  Option<NonTrivial> opt      = Some(NonTrivial(1));
  Option<NonTrivial> replaced = opt.replace(NonTrivial(2));
  assert(replaced.isSome());
  assert(*replaced.unwrap().x == 1);
  assert(opt.isSome());
  assert(*opt.unwrap().x == 2);

  Option<NonTrivial> opt2      = None();
  Option<NonTrivial> replaced2 = opt2.replace(NonTrivial(3));
  assert(replaced2.isNone());
  assert(opt2.isSome());
  assert(*opt2.unwrap().x == 3);
}

void takeTest(void) {
  Option<NonTrivial> opt   = Some(NonTrivial(1));

  Option<NonTrivial> taken = opt.take();
  assert(taken.isSome());
  assert(*taken.unwrap().x == 1);
  assert(opt.isNone());
}

int main(void) {
  createTest();
  mapTest();
  copyTest();
  replaceTest();
  takeTest();
}
