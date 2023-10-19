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
  str.push('!');
  assert(strcmp(str.getRaw(), "Hello!") == 0);
}

int main(void) { pushTest(); }
