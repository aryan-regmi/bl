#ifndef BL_ERROR_H
#define BL_ERROR_H

#include "bl/primitives.h" // cstr, usize

namespace bl {

using namespace primitives;

/// Represents an error.
struct Error {
public:
  Error() = delete;

  /// Checks if an error occured.
  ///
  /// ## Note
  /// Use `Error::getErrorMsg` to get the error that occured.
  static bool       isError(void);

  /// Gets the error message for the current/latest error.
  ///
  /// ## Note
  /// This should be used in conjuction with `Error::isError`; it will return
  /// `nullptr` if `Error::isError` returns false.
  static const_cstr getErrorMsg(void);

  /// Prints the error stack trace to `stderr`.
  static void       printErrorTrace(void);

  /// Throws an error with the given message.
  ///
  /// This will add to the existing error trace unless `Error::resetError` is
  /// called first.
  static void       throwError(const_cstr filename, usize line, const_cstr msg);

  /// Resets the error (`ERROR`) to its inital state, signifying no error.
  static void       resetError(void);

  /// Prints the error trace to `stderr` if an error occured.
  static void       checkError(void);

private:
  const_cstr filename = 0;
  usize      line     = 0;
  const_cstr msg      = nullptr;
  Error*     prev     = nullptr;
};

static thread_local bool   ERROR_OCCURED = false;
static thread_local Error* ERROR;

/// Convinence macro for throwing errors.
#define BL_THROW(msg) Error::throwError(__FILE__, __LINE__, msg)

} // namespace bl

#endif // !BL_ERROR_H
