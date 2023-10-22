#ifndef BL_OPTION_H
#define BL_OPTION_H

#include "bl/panic.h"      // BL_PANIC
#include "bl/primitives.h" // usize, const_cstr
#include <algorithm>

namespace bl {

// TODO: Add doc comments for struct
//
// TODO: Add funcs to convert option to result

template <typename T> struct Option;

template <typename T> struct Some {
public:
  Some(T val) : val(std::move(val)) {}

  Some(const Some& other) { this->val = other.val; }

  /// Default move constructor.
  Some(Some&& other) { this->val = std::move(other.val); }

  /// Default destructor.
  ~Some() = default;

  /// Default copy assignment operator.
  Some& operator=(const Some& other) {
    if (this == other) {
      return *this;
    }

    this->val = other.val;
    return *this;
  }

  /// Default move assignment operator.
  Some& operator=(Some&& other) {
    if (this == other) {
      return *this;
    }

    this->val = std::move(other.val);
    return *this;
  }

private:
  T val;

  friend Option<T>;
};

struct None {};

template <typename T> struct Option {
public:
  /// Creates an optional containing a value (`Some`).
  Option(Some<T> val) {
    // this->val.some = val.val;
    this->val.some = std::move(val.val);
    this->is_none  = false;
  }

  /// Creates an optional with no value (`None`).
  Option(None /* none */) { this->is_none = true; }

  /// Clones the `other` value.
  Option(const Option& other) {
    if (other.is_none) {
      this->is_none = true;
      return;
    }

    this->val.some = other.val.some;
    this->is_none  = false;
  };

  /// Moves the `other` value.
  Option(Option&& other) {
    if (other.is_none) {
      this->val.some.~T();
      this->is_none = true;
      return;
    }

    this->val.some = std::move(other.val.some);
    this->is_none  = false;
    other.is_none  = true;
  };

  ~Option() {
    if (this->is_none) {
      return;
    }

    this->val.some.~T();
  }

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

    return optb;
  }

  /// Returns the contained value.
  ///
  /// ## Panics
  /// - Panics if the option is  `None`.
  T& unwrap(void) {
    if (!this->is_none) {
      return this->val.some;
      // return std::move(this->val.some);
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

private:
  union OptVal {
    T    some;
    bool marker;

    constexpr OptVal() : marker(false) {}

    OptVal(OptVal&& other)            = default;
    OptVal& operator=(OptVal&& other) = default;

    ~OptVal() {}
  } val;

  bool is_none;
};

} // namespace bl

#endif // !BL_OPTION_H