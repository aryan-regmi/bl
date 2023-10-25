#ifndef BL_RESULT_H
#define BL_RESULT_H

#include "bl/option.h"     // Option
#include "bl/panic.h"      // BL_PANIC, panic
#include "bl/primitives.h" // const_cstr

#include <algorithm>   // move
#include <cstdlib>     // abort
#include <type_traits> // is_base_of_v

namespace bl {
using namespace primitives;

// TODO: Add template specialization for reference/pointer types?

/// An interface for errors, to be used in conjunction with `Result`.
struct Error {
  virtual ~Error() {}
  virtual const_cstr errMsg() = 0;
};

/// A type used for returning and propagating errors.
///
/// It can contain either a success value (`Ok`) or an error value `Err`.
///
/// ## Note
/// This should be used with moveable structs/classes for the lowest memory
/// footprint (and to reduce the number of copies that occur). Use of this with
/// non-moveable classes can result in more copies than expected and use up
/// extra memory.
template <typename T, typename E> struct Result;

/// Represents a success value.
template <typename T> struct Ok {
public:
  Ok(T val) : val(std::move(val)) {}

  Ok(const Ok& other) { this->val = other.val; }

  Ok(Ok&& other) { this->val = std::move(other.val); }

  ~Ok()                          = default;

  Ok& operator=(const Ok& other) = default;

  Ok& operator=(Ok&& other)      = default;

private:
  T val;
  template <typename U, typename E> friend struct Result;
};

/// Represents a failure/error value.
template <typename E> struct Err {
public:
  Err(E val) : val(std::move(val)) {}

  Err(const Err& other) { this->val = other.val; }

  Err(Err&& other) { this->val = std::move(other.val); }

  ~Err()                           = default;

  Err& operator=(const Err& other) = default;

  Err& operator=(Err&& other)      = default;

private:
  E val;
  template <typename T, typename F> friend struct Result;
};

template <typename T, typename E> struct Result {
  static_assert(std::is_base_of_v<Error, E>,
                "The error type of `Result` must be an `Error`");

public:
  /// Creates a result containing a success value (`Ok`).
  Result(Ok<T> ok) {
    this->val.ok = std::move(ok.val);
    this->is_err = false;
    ok.val.~T();
  }

  /// Creates a result containing a failure value (`Err`).
  Result(Err<E> err) {
    this->val.error = std::move(err.val);
    this->is_err    = true;
    err.val.~E();
  }

  /// Clones the `other` optional.
  Result(const Result& other) {
    if (other.is_err) {
      this->is_err    = true;
      this->val.error = other.val.error;
      return;
    }

    this->is_err = false;
    this->val.ok = other.val.ok;
  };

  /// Moves the `other` optional.
  Result(Result&& other) {
    if (other.is_err) {
      this->is_err    = true;
      this->val.error = std::move(other.val.error);
      return;
    }

    this->is_err   = false;
    this->val.ok   = std::move(other.val.ok);
    other.is_valid = false;
  };

  /// Calls the contained values destructor (if `Ok`).
  ~Result() {
    if (!this->is_valid) {
      return;
    }

    switch (this->is_err) {
    case true:
      this->val.error.~E();
      return;
    case false:
      this->val.ok.~T();
      return;
    }
  }

  /// Copy assignment operator.
  Result& operator=(const Result& other) {
    if (this == other) {
      return *this;
    }

    if (this->is_err) {
      this->val.error.~E();
      switch (other.is_err) {
      case true:
        this->val.error = other.val.error;
        return *this;
      case false:
        this->val.ok = other.val.ok;
        this->is_err = false;
        return *this;
      }
    }

    if (!this->is_err) {
      this->val.ok.~T();
      switch (other.is_err) {
      case true:
        this->val.error = other.val.error;
        this->is_err    = true;
        return *this;
      case false:
        this->val.ok = other.val.ok;
        return *this;
      }
    }
  }

  /// Move assignment operator.
  Result& operator=(Result&& other) {
    if (this == other) {
      return *this;
    }

    if (this->is_err) {
      this->val.error.~E();
      switch (other.is_err) {
      case true:
        this->val.error = std::move(other.val.error);
        break;
      case false:
        this->val.ok = std::move(other.val.ok);
        this->is_err = false;
        break;
      }
      other.is_valid = false;
      return *this;
    }

    if (!this->is_err) {
      this->val.ok.~T();
      switch (other.is_err) {
      case true:
        this->val.error = std::move(other.val.error);
        this->is_err    = true;
        break;
      case false:
        this->val.ok = std::move(other.val.ok);
        break;
      }
      other.is_valid = false;
      return *this;
    }
  }

  /// Bool operator to use result with `?` for easy chaining.
  operator bool() const { return !this->is_err; }

  /// Returns `true` if the result is `Ok`.
  bool                               isOk(void) const { return !this->is_err; }

  /// Returns `true` if the result is `Err`.
  bool                               isErr(void) const { return this->is_err; }

  /// Maps a `Result<T,E>` to `Result<U, E>` by applying the given function to a
  /// contained `Ok` value, leaving an `Err` value untouched.
  template <typename U> Result<U, E> map(U (*mapfn)(T)) const {
    if (this->is_err) {
      return Err(this->val.error);
    }

    return Some(mapfn(this->val.ok));
  }

  /// Maps a `Result<T,E>` to `Result<T, F>` by applying the given function to a
  /// contained `Err` value, leaving an `Ok` value untouched.
  template <typename F> Result<T, F> mapErr(F (*mapfn)(E)) const {
    if (!this->is_err) {
      return Ok<T>(this->val.ok);
    }

    return Err<F>(mapfn(this->val.error));
  }

  /// Returns the contained `Ok` value.
  ///
  /// ## Panics
  /// - Panics if the result is `Err`.
  T& unwrap(void) {
    if (!this->is_err) {
      return this->val.some;
    }

    panic(__FILE__, __LINE__, "Unwrapped a `Err` value:");
    BL_PANIC(this->val.error.errMsg());
  }

  /// Returns the contained `Ok` value.
  ///
  /// This overload takes the filename and line it's called at for more a
  /// accurate panic message.
  ///
  /// ## Panics
  /// - Panics if the result is `Err`.
  T& unwrap(const_cstr filename, usize line) {
    if (!this->is_err) {
      return this->val.some;
    }

    panic(filename, line, "Unwrapped a `Err` value:");
    panic(filename, line, this->val.error.errMsg());
    BL_PANIC(this->val.error.errMsg());
    abort();
  }

  /// Returns the contained `Ok` value, or the provided default.
  T& unwrapOr(T&& default_val) {
    if (!this->is_err) {
      return this->val.ok;
    }

    return default_val;
  }

  /// Returns the contained `Ok` value without checking that the value is not an
  /// `Err`.
  ///
  /// Calling this on an `Err` value/result is undefined behavior.
  T& unwrapUnchecked(void) { return this->val.ok; }

  /// Returns the contained `Err` value.
  ///
  /// ## Panics
  /// - Panics if the result is `Ok`.
  E& unwrapErr(void) {
    if (!this->is_err) {
      panic(__FILE__, __LINE__, "Unwrapped an `Ok` value:");
      BL_PANIC(this->val.error.errMsg());
    }

    return this->val.error;
  }

  /// Returns the contained `Err` value.
  ///
  /// This overload takes the filename and line it's called at for more a
  /// accurate panic message.
  ///
  /// ## Panics
  /// - Panics if the result is `Ok`.
  E& unwrapErr(const_cstr filename, usize line) {
    if (!this->is_err) {
      panic(filename, line, "Unwrapped an `Ok` value:");
      panic(filename, line, this->val.error.errMsg());
      BL_PANIC(this->val.error.errMsg());
      abort();
    }

    return this->val.error;
  }

  /// Returns the contained `Ok` value.
  ///
  /// ## Panics
  /// - Panics with the given message if the result is `Err`.
  T& expect(const_cstr msg) {
    if (!this->is_err) {
      return this->val.some;
    }

    BL_PANIC(msg);
  }

  /// Returns the contained `Ok` value.
  ///
  /// This overload takes the filename and line it's called at for more a
  /// accurate panic message.
  ///
  /// ## Panics
  /// - Panics with the given message if the result is `Err`.
  T& expect(const_cstr filename, usize line, const_cstr msg) {
    if (!this->is_err) {
      return this->val.some;
    }

    panic(filename, line, msg);
    abort();
  }

  /// Returns the contained `Err` value.
  ///
  /// ## Panics
  /// - Panics with the given message if the result is `Ok`.
  E& expectErr(const_cstr msg) {
    if (!this->is_err) {
      BL_PANIC(msg);
    }

    return this->val.error;
  }

  /// Returns the contained `Err` value.
  ///
  /// This overload takes the filename and line it's called at for more a
  /// accurate panic message.
  ///
  /// ## Panics
  /// - Panics with the given message if the result is `Ok`.
  E& expectErr(const_cstr filename, usize line, const_cstr msg) {
    if (!this->is_err) {
      panic(filename, line, msg);
      abort();
    }

    return this->val.error;
  }

  /// Converts the `Result<T,E>` to `Option<T>`.
  ///
  /// If the contained value is an `Err`, then it will be discarded and replaced
  /// with `None`.
  Option<T> ok(void) {
    if (this->is_err) {
      return None();
    }

    return Some<T>(this->val.ok);
  }

  /// Converts the `Result<T,E>` to `Option<E>`.
  ///
  /// If the contained value is an `Ok`, then it will be discarded and replaced
  /// with `None`.
  Option<E> err(void) {
    if (!this->is_err) {
      return None();
    }

    return Some<E>(this->val.error);
  }

private:
  bool is_err;
  bool is_valid = true;

  union ResVal {
    bool marker;
    T    ok;
    E    error;

    constexpr ResVal() : marker(false) {}
    ResVal(ResVal&& other)            = default;
    ResVal& operator=(ResVal&& other) = default;
    ~ResVal() {}
  } val;
};

} // namespace bl

#endif // !BL_RESULT_H
