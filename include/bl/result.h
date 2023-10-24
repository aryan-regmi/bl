#ifndef BL_RESULT_H
#define BL_RESULT_H

#include "bl/primitives.h" // const_cstr

#include <algorithm>   // move
#include <type_traits> // is_base_of_v

namespace bl {
using namespace primitives;

// TODO: Add operator bool so it can be used with `?`.

/// An interface for errors, to be used in conjunction with `Result`.
struct Error {
  virtual ~Error() {}
  virtual const_cstr errMsg() = 0;
};

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

/// A type used for returning and propagating errors.
///
/// It can contain either a success value (`Ok`) or an error value `Err`.
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

  /// Returns `true` if the result is `Ok`.
  bool isOk(void) const { return !this->is_err; }

  /// Returns `true` if the result is `Err`.
  bool isErr(void) const { return this->is_err; }

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
