#include "bl/error.h"

#include "bl/primitives.h" // usize

#include <assert.h> // assert
#include <cstdio>   // fprintf
#include <cstdlib>  // malloc

namespace bl {

bool       Error::isError(void) { return ERROR_OCCURED; }

const_cstr Error::getErrorMsg(void) { return ERROR->msg; }

void       Error::printErrorTrace(void) {
  Error* curr_err = ERROR;

  while (curr_err != nullptr) {
    int bytes_written = fprintf(stderr, "%s:%zu -> %s\n", curr_err->filename,
                                      curr_err->line, curr_err->msg);
    if (bytes_written == 0) {
      // Error occured during printing error
      return;
    }

    curr_err = curr_err->prev;
  }
}

void Error::throwError(const_cstr filename, usize line, const_cstr msg) {
  Error* err = (Error*)malloc(sizeof(Error));
  if (err == nullptr) {
    int bytes_written =
        fprintf(stderr, "OutOfMemory: Unable to allocate space for error");
    assert(bytes_written > 0);
    return;
  }

  err->filename = filename;
  err->line     = line;
  err->msg      = msg;

  if (ERROR != nullptr) {
    err->prev = ERROR;
  }
  ERROR         = err;
  ERROR_OCCURED = true;
}

void Error::resetError(void) {
  ERROR_OCCURED = false;
  ERROR         = nullptr;
}

void Error::checkError(void) {
  if (isError()) {
    printErrorTrace();
  }
}

} // namespace bl
