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

void removeTest(void) {
  String str = String("Hello");
  Error::checkError();

  char removed = str.remove(0);
  Error::checkError();
  assert(removed == 'H');
  assert(strcmp(str.getRaw(), "ello") == 0);
  assert(str.getLen() == 4);

  removed = str.remove(3);
  Error::checkError();
  assert(removed == 'o');
  assert(strcmp(str.getRaw(), "ell") == 0);
  assert(str.getLen() == 3);

  removed = str.remove(1);
  Error::checkError();
  assert(removed == 'l');
  assert(strcmp(str.getRaw(), "el") == 0);
  assert(str.getLen() == 2);
}

void containsTest(void) {
  String str   = String("Hello");

  i32    found = str.find("Hell");
  assert(found == 0);

  found = str.find("lo");
  assert(found == 3);

  found = str.find("Bye");
  assert(found == -1);
}

void shrinkTest(void) {
  String str = String("Hello");
  assert(str.getCap() == 5);

  str.pop();
  str.pop();
  assert(str.getCap() == 5);

  str.shrinkToFit();
  assert(str.getCap() == 3);
  assert(strcmp(str.getRaw(), "Hel") == 0);
}

void splitTest(void) {
  String str = String("Hello");
  Error::checkError();
  String st2 = str;
  //
  // assert(st2.getLen() == 5);
  //
  // assert(str.getLen() == 5);
  // String split = str.splitOff(3);

  // assert(str.getLen() == 3);
  // assert(str.getCap() == 5);
  // assert(strcmp(str.getRaw(), "Hel") == 0);
  //
  // assert(split.getLen() == 2);
  // assert(split.getCap() == 2);
  // assert(strcmp(split.getRaw(), "lo") == 0);
}

int main(void) {
  // pushTest();
  // popTest();
  // insertTest();
  // insertStrTest();
  // removeTest();
  // containsTest();
  // shrinkTest();
  splitTest();
}
