#ifndef BL_PRIMITIVES_H
#define BL_PRIMITIVES_H

#include <cstddef> // size_t
#include <cstdint> // uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t,
                   // int_least8_t, int_least16_t, int_least32_t, int_least64_t

namespace bl::primitives {

typedef size_t         usize;
typedef uint_least8_t  u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;
typedef uint_least64_t u64;

typedef int_least8_t   i8;
typedef int_least16_t  i16;
typedef int_least32_t  i32;
typedef int_least64_t  i64;

typedef float          f32;
typedef double         f64;

typedef char*          cstr;
typedef const char*    const_cstr;

} // namespace bl::primitives

#endif // !BL_PRIMITIVES_H
