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

  assert(arr.push(1));
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  assert(arr.push(2));
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  assert(arr.push(3));
  assert(arr.getLen() == 3);
  assert(arr.getCap() == 4);
}

void popTest(void) {
  DynamicArray arr = DynamicArray<int>(2);

  assert(arr.push(1));
  assert(arr.push(2));

  int popped = arr.pop().unwrapUnchecked();
  assert(popped == 2);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 2);

  popped = arr.pop().unwrapUnchecked();
  assert(popped == 1);
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

void indexTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});
  assert(arr[0] == 1);
  assert(arr[1] == 2);
  assert(arr[2] == 3);
}

void clearTest(void) {
  DynamicArray arr = DynamicArray<String>(2);

  assert(arr.push(String("Hello")));
  assert(arr.push(String("GoodBye")));
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 2);

  arr.clear();
  assert(arr.getLen() == 0);
  assert(arr.getCap() == 2);
}

void insertTest(void) {
  DynamicArray arr = DynamicArray<int>({1, 2, 3});

  assert(arr.insert(1, 4));
  assert(arr.getLen() == 4);
  assert(arr.getCap() == 6);
  assert(arr[0] == 1);
  assert(arr[1] == 4);
  assert(arr[2] == 2);
  assert(arr[3] == 3);

  assert(arr.insert(3, 4));
  assert(arr.getLen() == 5);
  assert(arr.getCap() == 6);
  assert(arr[0] == 1);
  assert(arr[1] == 4);
  assert(arr[2] == 2);
  assert(arr[3] == 3);
  assert(arr[4] == 4);
}

void removeTest(void) {
  DynamicArray arr     = DynamicArray<int>({1, 2, 3});

  int          removed = arr.remove(0).unwrapUnchecked();
  assert(removed == 1);
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 3);
  assert(arr[0] == 2);
  assert(arr[1] == 3);

  removed = arr.remove(1).unwrapUnchecked();
  assert(removed == 3);
  assert(arr.getLen() == 1);
  assert(arr.getCap() == 3);
  assert(arr[0] == 2);
}

void swapRemoveTest(void) {
  DynamicArray arr     = DynamicArray<int>({1, 2, 3});

  int          removed = arr.swapRemove(0).unwrapUnchecked();
  assert(removed == 1);
  assert(arr.getLen() == 2);
  assert(arr.getCap() == 3);
  assert(arr[0] == 3);
  assert(arr[1] == 2);

  removed = arr.remove(1).unwrapUnchecked();
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
