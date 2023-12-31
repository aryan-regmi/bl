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
  assert(str.isSame("Hello "));
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  str.push("World!");
  Error::checkError();
  assert(str.isSame("Hello World!"));
  assert(str.getLen() == 12);
  assert(str.getCap() == 20);
}

void popTest(void) {
  String str = String("Hello");
  Error::checkError();

  char popped = str.pop();
  Error::checkError();

  assert(str.isSame("Hell"));
  assert(popped == 'o');
  assert(str.getLen() == 4);
}

void insertTest(void) {
  String str = String("Hello");
  Error::checkError();

  str.insert(0, 'O');
  Error::checkError();
  assert(str.isSame("OHello"));
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  str.insert(5, '!');
  Error::checkError();
  assert(str.isSame("OHello!"));
  assert(str.getLen() == 7);
  assert(str.getCap() == 10);

  str.insert(1, 'y');
  Error::checkError();
  assert(str.isSame("OyHello!"));
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);
}

void insertStrTest(void) {
  String str = String("Hello");
  Error::checkError();

  str.insert(0, "Oy ");
  Error::checkError();
  assert(str.isSame("Oy Hello"));
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);

  str.insert(7, " You!");
  Error::checkError();
  assert(str.isSame("Oy Hello You!"));
  assert(str.getLen() == 13);
  assert(str.getCap() == 20);

  str.insert(9, "There ");
  Error::checkError();
  assert(str.isSame("Oy Hello There You!"));
  assert(str.getLen() == 19);
  assert(str.getCap() == 20);
}

void removeTest(void) {
  String str = String("Hello");
  Error::checkError();

  char removed = str.remove(0);
  Error::checkError();
  assert(removed == 'H');
  assert(str.isSame("ello"));
  assert(str.getLen() == 4);

  removed = str.remove(3);
  Error::checkError();
  assert(removed == 'o');
  assert(str.isSame("ell"));
  assert(str.getLen() == 3);

  removed = str.remove(1);
  Error::checkError();
  assert(removed == 'l');
  assert(str.isSame("el"));
  assert(str.getLen() == 2);
}

void containsTest(void) {
  String str = String("Hello");
  Error::checkError();

  i32 found = str.find("Hell");
  Error::checkError();
  assert(found == 0);

  found = str.find("lo");
  Error::checkError();
  assert(found == 3);

  found = str.find("Bye");
  Error::checkError();
  assert(found == -1);
}

void shrinkTest(void) {
  String str = String("Hello");
  Error::checkError();
  assert(str.getCap() == 5);

  str.pop();
  str.pop();
  Error::checkError();
  assert(str.getCap() == 5);

  str.shrinkToFit();
  Error::checkError();
  assert(str.getCap() == 3);
  assert(str.isSame("Hel"));
}

void splitTest(void) {
  String str = String("Hello");
  Error::checkError();

  String split = str.split(3);
  Error::checkError();

  assert(str.getLen() == 3);
  assert(str.getCap() == 5);
  assert(str.isSame("Hel"));

  assert(split.getLen() == 2);
  assert(split.getCap() == 2);
  assert(split.isSame("lo"));
}

void indexTest(void) {
  String str = String("Hello");
  Error::checkError();

  assert(str[0] == 'H');
  assert(str[1] == 'e');
  assert(str[2] == 'l');
  assert(str[3] == 'l');
  assert(str[4] == 'o');
}

int main(void) {
  pushTest();
  popTest();
  insertTest();
  insertStrTest();
  removeTest();
  containsTest();
  shrinkTest();
  splitTest();
  indexTest();
}
