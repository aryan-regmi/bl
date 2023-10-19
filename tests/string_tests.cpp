#include "bl/error.h"
#include "bl/mem/allocator.h"
#include "bl/mem/c_allocator.h"
#include "bl/primitives.h"
#include "bl/string.h"

#include <cassert>
#include <cstdio>
#include <cstring>

using namespace bl;

void pushTest(void) {
  String str = String("Hello");
  Error::checkError();

  str.push(' ');
  Error::checkError();
  assert(strcmp(str.getRaw(), "Hello ") == 0);
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  str.push("World!");
  Error::checkError();
  assert(strcmp(str.getRaw(), "Hello World!") == 0);
  assert(str.getLen() == 12);
  assert(str.getCap() == 20);
}

void popTest(void) {
  String str = String("Hello");
  Error::checkError();

  char popped = str.pop();
  Error::checkError();

  assert(strcmp(str.getRaw(), "Hell") == 0);
  assert(popped == 'o');
  assert(str.getLen() == 4);
}

int main(void) {
  pushTest();
  popTest();
}
