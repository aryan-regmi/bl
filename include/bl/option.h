#ifndef BL_OPTION_H
#define BL_OPTION_H

#include "bl/panic.h"      // BL_PANIC, panic
#include "bl/primitives.h" // usize, const_cstr
#include <algorithm>       // move

namespace bl {

// TODO: Add operator bool so it can be used with `?`.
//
// TODO: Add funcs to convert option to result
//
// TODO: Add template specialization for reference/pointer types?

template <typename T> struct Option;

/// Represents an optional containing a value.
template <typename T> struct Some {
public:
  /// Creates a new `Option<T>` containing the given value.
  Some(T val) : val(std::move(val)) {}

  /// Copy constructor.
  Some(const Some& other) { this->val = other.val; }

  /// Move constructor.
  Some(Some&& other) { this->val = std::move(other.val); }

  /// Default destructor.
  ~Some()                            = default;

  /// Default copy assignment operator.
  Some& operator=(const Some& other) = default;

  /// Default move assignment operator.
  Some& operator=(Some&& other)      = default;

private:
  T val;
  friend Option<T>;
};

/// Represents an optional with no value.
struct None {};

/// Represents an optional type.
///
/// The value can be either `Some` (contains a value) or `None` (contains no
/// value).
template <typename T> struct Option {
public:
  /// Creates an optional containing a value (`Some`).
  Option(Some<T> some) {
    this->val.some = std::move(some.val);
    this->is_none  = false;
    some.val.~T();
  }

  /// Creates an optional with no value (`None`).
  Option(None /* none */) { this->is_none = true; }

  /// Clones the `other` optional.
  Option(const Option& other) {
    if (other.is_none) {
      this->is_none = true;
      return;
    }

    this->val.some = other.val.some;
    this->is_none  = false;
  };

  /// Moves the `other` optional.
  Option(Option&& other) {
    if (other.is_none) {
      // this->val.some.~T();
      this->is_none = true;
      return;
    }

    this->val.some = std::move(other.val.some);
    this->is_none  = false;
    other.is_none  = true;
  };

  /// Calls the contained values destructor (if `Some`).
  ~Option() {
    if (this->is_none) {
      return;
    }

    this->val.some.~T();
  }

  /// Copy assignment operator.
  Option& operator=(const Option& other) {
    if (this == other) {
      return *this;
    }

    if (!this->is_none) {
      this->val.some.~T();
    }

    if (other.is_none) {
      this->is_none = true;
      return *this;
    }

    this->val.some = other.val.some;
    this->is_none  = false;
    return *this;
  }

  /// Move assignment operator.
  Option& operator=(Option&& other) {
    if (this == other) {
      return *this;
    }

    if (!this->is_none) {
      this->val.some.~T();
    }

    if (other.is_none) {
      this->is_none = true;
      return *this;
    }

    this->val.some = std::move(other.val.some);
    this->is_none  = false;
    other.is_none  = true;
    return *this;
  }

  /// Returns `true` if the option contains a value.
  bool   isSome(void) const { return !this->is_none; }

  /// Returns `true` if the option doesn't contain a value.
  bool   isNone(void) const { return this->is_none; }

  /// Returns the option if it contains a value, otherwise returns `optb`.
  Option someOr(Option optb) {
    if (!this->is_none) {
      return *this;
    }

    return std::move(optb);
  }

  /// Returns the contained value.
  ///
  /// ## Panics
  /// - Panics if the option is  `None`.
  T& unwrap(void) {
    if (!this->is_none) {
      return this->val.some;
    }

    BL_PANIC("Unwrapped a `None` value");
  }

  /// Returns the contained value.
  ///
  /// This overload takes the filename and line it's called at for more a
  /// accurate panic message.
  ///
  /// ## Panics
  /// - Panics if the option is  `None`.
  T& unwrap(const_cstr filename, usize line) {
    if (!this->is_none) {
      return this->val.some;
      // return std::move(this->val.some);
    }

    panic(filename, line, "Unwrapped a `None` value");
    abort();
  }

  /// Returns the contained value (if `Some`), or the provided default.
  T& unwrapOr(T&& default_val) {
    if (!this->is_none) {
      return this->val.some;
    }

    return default_val;
  }

  /// Returns the contained value without checking if it is `None`.
  ///
  /// ## Note
  /// Calling this on a `None` is undefined behavior.
  T& unwrapUnchecked(void) { return this->val.some; }

  /// Maps an `Option<T>` to `Option<U>` by applying the given function to the
  /// contained value (if `Some`).
  ///
  /// If the value is `None`, nothing is done to it and `None` is returned.
  ///
  /// ## Note
  /// The `func` must take a value of type `T` and return a value of type `U`.
  template <typename U> Option<U> map(U (*func)(T)) const {
    if (this->is_none) {
      return None();
    }

    U mapped = func(this->val.some);

    return Some<U>(mapped);
  }

  /// Replaces the value in the option with the given one, returning the old
  /// value.
  Option replace(T value) {
    if (this->is_none) {
      this->val.some = std::move(value);
      this->is_none  = false;
      return None();
    }

    T old_value = std::move(this->val.some);
    this->val.some.~T();
    this->val.some = std::move(value);
    return Some(old_value);
  }

  /// Takes the value out of the option and leaves a `None` in its place.
  Option take(void) {
    if (this->is_none) {
      return None();
    }

    T old_value = std::move(this->val.some);
    this->val.some.~T();
    this->is_none = true;
    return Some(old_value);
  }

private:
  /// Determines the type of the optional.
  bool is_none;

  /// The value stored in the optional (if `Some`).
  union OptVal {
    /// The actual value stored in the optional.
    T    some;

    /// Marker to cirumvent compiler bugs.
    bool marker;

    constexpr OptVal() : marker(false) {}

    OptVal(OptVal&& other)            = default;
    OptVal& operator=(OptVal&& other) = default;

    ~OptVal() {}
  } val;
};

} // namespace bl

#endif // !BL_OPTION_H
