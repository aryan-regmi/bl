#include "bl/ds/dynamic_array.h"
#include "bl/error.h"
#include "bl/mem/allocator.h"
#include "bl/mem/c_allocator.h"
#include "bl/primitives.h"
#include "bl/string.h"

#include <cassert>
#include <cstdio>
#include <cstring>

using namespace bl;
using namespace bl::ds;

void pushTest(void) {
  DynamicArray arr = DynamicArray<int>(2);
  Error::checkError();

  arr.push(1);
  Error::checkError();
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  arr.push(2);
  Error::checkError();
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  arr.push(3);
  Error::checkError();
  assert(arr.getLen() == 3);
  assert(arr.getCap() == 4);
}

void popTest(void) {
  DynamicArray arr = DynamicArray<int>(2);
  Error::checkError();

  arr.push(1);
  Error::checkError();
  arr.push(2);
  Error::checkError();

  int popped = arr.pop();
  Error::checkError();
  assert(popped == 2);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  popped = arr.pop();
  Error::checkError();
  assert(popped == 1);
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

void indexTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  Error::checkError();

  assert(arr[0] == 1);
  assert(arr[1] == 2);
  assert(arr[2] == 3);
}

void clearTest(void) {
  DynamicArray arr = DynamicArray<String>(2);
  Error::checkError();

  arr.push(String("Hello"));
  Error::checkError();
  arr.push(String("GoodBye"));
  Error::checkError();
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  arr.clear();
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

int main(void) {
  pushTest();
  popTest();
  indexTest();
  clearTest();
}
