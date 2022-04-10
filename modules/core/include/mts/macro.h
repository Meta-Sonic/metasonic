#pragma once

/// @def __MTS_CONCAT1
#define __MTS_CONCAT1(_X, _Y) _X##_Y

/// @def MTS_CONCAT
#define MTS_CONCAT(_X, _Y) __MTS_CONCAT1(_X, _Y)

/// @def MTS_STRINGIFY
#define MTS_STRINGIFY(X) MTS_STR(X)

/// @def MTS_STR
#define MTS_STR(X) #X

/// @def MTS_DEFER
#define MTS_DEFER(M, ...) M(__VA_ARGS__)

#define MTS_DETAIL_EXPAND(x) x

#define MTS_FIRST_(a, ...) a
#define MTS_SECOND_(a, b, ...) b

#define MTS_FIRST(...) MTS_FIRST_(__VA_ARGS__, )
#define MTS_SECOND(...) MTS_SECOND_(__VA_ARGS__, )

#define MTS_IS_PROBE(...) MTS_SECOND(__VA_ARGS__, 0)
#define MTS_PROBE() ~, 1

#define MTS_CAT(a, b) a##b

#define MTS_NOT(x) MTS_IS_PROBE(MTS_CAT(MTS__NOT_, x))
#define MTS__NOT_0 PROBE()

#define MTS_BOOL(x) MTS_NOT(MTS_NOT(x))

#define MTS_HAS_ARGS(...) MTS_BOOL(MTS_FIRST(MTS__END_OF_ARGUMENTS_ __VA_ARGS__)())
#define MTS__END_OF_ARGUMENTS_() 0

#define MTS_VA_REST_(A, ...) __VA_ARGS__
#define MTS_VA_REST(...) MTS_VA_REST_(__VA_ARGS__)

// Accept any number of args >= N, but expand to just the Nth one.
#define MTS_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,     \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, \
    _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, \
    _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, N, ...)                                                          \
  N

// Get the number of VAARGS.
#define MTS_SIZEOF_VAARGS(...)                                                                                         \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83,   \
      82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55,  \
      54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27,  \
      26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
