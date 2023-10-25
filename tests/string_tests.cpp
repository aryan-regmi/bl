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

  assert(str.push(' '));
  assert(str.isSame("Hello "));
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  assert(str.push("World!"));
  assert(str.isSame("Hello World!"));
  assert(str.getLen() == 12);
  assert(str.getCap() == 20);
}

void popTest(void) {
  String str    = String("Hello");

  char   popped = str.pop().unwrapUnchecked();
  assert(str.isSame("Hell"));
  assert(popped == 'o');
  assert(str.getLen() == 4);
}

void insertTest(void) {
  String str = String("Hello");

  assert(str.insert(0, 'O'));
  assert(str.isSame("OHello"));
  assert(str.getLen() == 6);
  assert(str.getCap() == 10);

  assert(str.insert(5, '!'));
  assert(str.isSame("OHello!"));
  assert(str.getLen() == 7);
  assert(str.getCap() == 10);

  assert(str.insert(1, 'y'));
  assert(str.isSame("OyHello!"));
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);
}

void insertStrTest(void) {
  String str = String("Hello");

  assert(str.insert(0, "Oy "));
  assert(str.isSame("Oy Hello"));
  assert(str.getLen() == 8);
  assert(str.getCap() == 10);

  assert(str.insert(7, " You!"));
  assert(str.isSame("Oy Hello You!"));
  assert(str.getLen() == 13);
  assert(str.getCap() == 20);

  assert(str.insert(9, "There "));
  assert(str.isSame("Oy Hello There You!"));
  assert(str.getLen() == 19);
  assert(str.getCap() == 20);
}

void removeTest(void) {
  String str     = String("Hello");

  char   removed = str.remove(0).unwrapUnchecked();
  assert(removed == 'H');
  assert(str.isSame("ello"));
  assert(str.getLen() == 4);

  removed = str.remove(3).unwrapUnchecked();
  assert(removed == 'o');
  assert(str.isSame("ell"));
  assert(str.getLen() == 3);

  removed = str.remove(1).unwrapUnchecked();
  assert(removed == 'l');
  assert(str.isSame("el"));
  assert(str.getLen() == 2);
}

void containsTest(void) {
  String str   = String("Hello");

  i32    found = str.find("Hell").unwrapUnchecked();
  assert(found == 0);

  found = str.find("lo").unwrapUnchecked();
  assert(found == 3);

  assert(str.find("Bye").isNone());
}

void shrinkTest(void) {
  String str = String("Hello");
  assert(str.getCap() == 5);

  assert(str.pop());
  assert(str.pop());
  assert(str.getCap() == 5);

  assert(str.shrinkToFit());
  assert(str.getCap() == 3);
  assert(str.isSame("Hel"));
}

void splitTest(void) {
  String str   = String("Hello");

  String split = str.split(3).unwrapUnchecked();
  assert(str.getLen() == 3);
  assert(str.getCap() == 5);
  assert(str.isSame("Hel"));

  assert(split.getLen() == 2);
  assert(split.getCap() == 2);
  assert(split.isSame("lo"));
}

void indexTest(void) {
  String str = String("Hello");

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
