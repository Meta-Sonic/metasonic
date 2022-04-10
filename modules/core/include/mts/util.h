///
/// BSD 3-Clause License
///
/// Copyright (c) 2022, Alexandre Arsenault
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// * Redistributions of source code must retain the above copyright notice, this
///   list of conditions and the following disclaimer.
///
/// * Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// * Neither the name of the copyright holder nor the names of its
///   contributors may be used to endorse or promote products derived from
///   this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#pragma once
#include "mts/config.h"
#include "mts/assert.h"
#include "mts/traits.h"
#include <type_traits>
#include <array>

MTS_BEGIN_NAMESPACE

/// Get the minimum of n values.
/// @code
///   int a = mts::minimum(1, 2, 3, 4, 5);
/// @endcode
template <typename T0, typename T1, typename... Ts>
inline constexpr typename std::common_type<T0, T1, Ts...>::type minimum(T0 v1, T1 v2, Ts... vs) {
  if constexpr (sizeof...(Ts) == 0) {
    return v2 < v1 ? v2 : v1;
  }
  else {

    return v2 < v1 ? minimum(v2, vs...) : minimum(v1, vs...);
  }
}

/// Get the maximum of n values.
/// @code
///   int a = mts::maximum(1, 2, 3, 4, 5);
/// @endcode
template <typename T0, typename T1, typename... Ts>
inline constexpr typename std::common_type<T0, T1, Ts...>::type maximum(T0 v1, T1 v2, Ts... vs) {
  if constexpr (sizeof...(Ts) == 0) {
    return v2 > v1 ? v2 : v1;
  }
  else {
    return v2 > v1 ? maximum(v2, vs...) : maximum(v1, vs...);
  }
}

/// Returns true if t1 < t2.
template <typename T1, typename T2>
inline constexpr bool is_less(const T1& t1, const T2& t2) {
  using type = std::common_type_t<T1, T2>;
  return static_cast<type>(t1) < static_cast<type>(t2);
}

/// Returns true if t1 <= t2.
template <typename T1, typename T2>
inline constexpr bool is_less_or_equal(const T1& t1, const T2& t2) {
  using type = std::common_type_t<T1, T2>;
  return static_cast<type>(t1) <= static_cast<type>(t2);
}

/// Returns true if t1 > t2.
template <typename T1, typename T2>
inline constexpr bool is_greater(const T1& t1, const T2& t2) {
  using type = std::common_type_t<T1, T2>;
  return static_cast<type>(t1) > static_cast<type>(t2);
}

/// Returns true if t1 >= t2.
template <typename T1, typename T2>
inline constexpr bool is_greater_or_equal(const T1& t1, const T2& t2) {
  using type = std::common_type_t<T1, T2>;
  return static_cast<type>(t1) >= static_cast<type>(t2);
}

/// Check if the first value is the same as one of the other ones.
///
/// These two conditions are equivalent:
/// @code
///   if(mts::is_one_of(a, 0, 1, 2, 3)) {
///     ...
///   }
///
///   if(a == 0 || a == 1 || a == 2 || a == 3) {
///     ...
///   }
/// @endcode
template <typename T, typename T1, typename... Ts>
inline constexpr bool is_one_of(T t, T1 t1, Ts... ts) {
  if constexpr (sizeof...(Ts) == 0) {
    return t == t1;
  }
  else {
    return (t == t1) || is_one_of(t, ts...);
  }
}

/// Check if all values are equals.
///
/// These two conditions are equivalent:
/// @code
///   if(mts::all_equals(a, b, c, d)) {
///      ...
///   }
///
///   if(a == b && b == c && c == d) {
///      ...
///   }
/// @endcode
template <typename T, typename T1, typename... Ts>
inline constexpr bool all_equals(T t, T1 t1, Ts... ts) {
  if constexpr (sizeof...(Ts) == 0) {
    return t == t1;
  }
  else {
    return (t == t1) && all_equals(t, ts...);
  }
}

/// Check if only one boolean is true.
template <typename... Ts>
inline constexpr bool is_only_one_true(Ts... ts) {
  return (ts ^ ...);
}

/// Same as is_only_one_true but with template arguments.
template <bool... Bs>
struct is_only_one_true_t {
  static constexpr bool value = (Bs ^ ...);
};

/// Check if more than one boolean are true.
template <typename... Ts>
inline constexpr bool is_more_than_one_true(Ts... ts) {
  return (int(ts) + ...) > 1;
}

/// Same as is_more_than_one_true but with template arguments.
template <bool... Bs>
struct is_more_than_one_true_t {
  static constexpr bool value = is_more_than_one_true(Bs...);
};

/// Check if only one boolean is false.
template <typename... Ts>
inline constexpr bool is_only_one_false(Ts... ts) {
  return (!ts ^ ...);
}

/// Same as is_only_one_false but with template arguments.
template <bool... Bs>
struct is_only_one_false_t {
  static constexpr bool value = (!Bs ^ ...);
};

/// Check if x is inside the range [left, right].
/// @warning Ill-formed if left > right.
template <typename T>
inline constexpr bool is_in_range(T x, T left, T right) {
  return x >= left && x <= right;
}

/// Check if x is outside the range [left, right].
/// @warning Ill-formed if left > right.
template <typename T>
inline constexpr bool is_out_of_range(T x, T left, T right) {
  return !is_in_range(x, left, right);
}

/// Use to silence unused variables warning.
template <typename... Types>
inline constexpr void unused(Types&&...) noexcept {}

/// Assign src value to dst value if they differs.
/// @returns true is assignment took place.
template <typename T, typename P>
MTS_NODISCARD inline constexpr bool assign(T& dst, const P& src) {
  if (dst == src) {
    return false;
  }

  dst = src;
  return true;
}

template <typename _Type, std::size_t _Size>
constexpr std::size_t array_size(_Type (&)[_Size]) noexcept {
  return _Size;
}
MTS_END_NAMESPACE
