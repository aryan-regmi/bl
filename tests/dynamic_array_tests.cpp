#include "bl/ds/dynamic_array.h"
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
  Panic::checkError();

  arr.push(1);
  Panic::checkError();
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  arr.push(2);
  Panic::checkError();
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  arr.push(3);
  Panic::checkError();
  assert(arr.getLen() == 3);
  assert(arr.getCap() == 4);
}

void popTest(void) {
  DynamicArray arr = DynamicArray<int>(2);
  Panic::checkError();

  arr.push(1);
  Panic::checkError();
  arr.push(2);
  Panic::checkError();

  int popped = arr.pop();
  Panic::checkError();
  assert(popped == 2);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  popped = arr.pop();
  Panic::checkError();
  assert(popped == 1);
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

void indexTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  Panic::checkError();

  assert(arr[0] == 1);
  assert(arr[1] == 2);
  assert(arr[2] == 3);
}

void clearTest(void) {
  DynamicArray arr = DynamicArray<String>(2);
  Panic::checkError();

  arr.push(String("Hello"));
  Panic::checkError();
  arr.push(String("GoodBye"));
  Panic::checkError();
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  arr.clear();
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

void insertTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  Panic::checkError();

  arr.insert(1, 4);
  Panic::checkError();
  assert(arr.getLen() == 4);
  assert(arr.getCap() == 6);
  assert(arr[0] == 1);
  assert(arr[1] == 4);
  assert(arr[2] == 2);
  assert(arr[3] == 3);

  arr.insert(3, 4);
  Panic::checkError();
  assert(arr.getLen() == 5);
  assert(arr.getCap() == 6);
  assert(arr[0] == 1);
  assert(arr[1] == 4);
  assert(arr[2] == 2);
  assert(arr[3] == 3);
  assert(arr[4] == 4);
}

void removeTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  Panic::checkError();

  int removed = arr.remove(0);
  Panic::checkError();
  assert(removed == 1);
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 3);
  assert(arr[0] == 2);
  assert(arr[1] == 3);

  removed = arr.remove(1);
  Panic::checkError();
  assert(removed == 3);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 3);
  assert(arr[0] == 2);
}

void swapRemoveTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  Panic::checkError();

  int removed = arr.swapRemove(0);
  Panic::checkError();
  assert(removed == 1);
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 3);
  assert(arr[0] == 3);
  assert(arr[1] == 2);

  removed = arr.remove(1);
  Panic::checkError();
  assert(removed == 2);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 3);
  assert(arr[0] == 3);
}

int main(void) {
  pushTest();
  popTest();
  indexTest();
  clearTest();
  insertTest();
  removeTest();
  swapRemoveTest();
}
