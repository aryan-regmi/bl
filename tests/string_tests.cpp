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

void insertTest(void) {
  String str = String("Hello");
  Error::checkError();

  str.insert(0, 'O');
  Error::checkError();
  assert(strcmp(str.getRaw(), "OHello") == 0);
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  str.insert(5, '!');
  Error::checkError();
  assert(strcmp(str.getRaw(), "OHello!") == 0);
  assert(str.getLen() == 7);
  assert(str.getCap() == 10);

  str.insert(1, 'y');
  Error::checkError();
  assert(strcmp(str.getRaw(), "OyHello!") == 0);
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);
}

void insertStrTest(void) {
  String str = String("Hello");
  Error::checkError();

  str.insert(0, "Oy ");
  Error::checkError();
  assert(strcmp(str.getRaw(), "Oy Hello") == 0);
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);

  str.insert(7, " You!");
  Error::checkError();
  assert(strcmp(str.getRaw(), "Oy Hello You!") == 0);
  assert(str.getLen() == 13);
  assert(str.getCap() == 20);

  str.insert(9, "There ");
  Error::checkError();
  assert(strcmp(str.getRaw(), "Oy Hello There You!") == 0);
  assert(str.getLen() == 19);
  assert(str.getCap() == 20);
}

int main(void) {
  pushTest();
  popTest();
  insertTest();
  insertStrTest();
}
