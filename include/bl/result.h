#ifndef BL_RESULT_H
#define BL_RESULT_H

#include "bl/primitives.h" // const_cstr

#include <algorithm>   // move
#include <type_traits> // is_base_of_v

namespace bl {
using namespace primitives;

struct Error {
  virtual const_cstr errMsg() = 0;
};

template <typename T, typename E> struct Result;

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
  Result(Ok<T> ok) {
    this->val.ok = std::move(ok.val);
    this->is_err = false;
    ok.val.~T();
  }

  Result(Err<E> err) {
    this->val.error = std::move(err.val);
    this->is_err    = true;
    err.val.~E();
  }

private:
  bool is_err;

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
