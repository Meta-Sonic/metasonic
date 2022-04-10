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
#include "mts/traits.h"
#include "mts/util.h"
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

#ifdef abs
  #undef abs
#endif

#ifdef round
  #undef round
#endif

#ifdef signbit
  #undef signbit
#endif

MTS_BEGIN_SUB_NAMESPACE(math)
template <typename T>
inline constexpr T zero = T(0.0);

template <typename T>
inline constexpr T one = T(1.0);

template <typename T>
inline constexpr T e = T(2.7182818284590452353602874713526624977572L);

template <typename T>
inline constexpr T pi = T(3.1415926535897932384626433832795028841972L);

template <typename T>
inline constexpr T half_pi = T(1.5707963267948966192313216916397514420986L);

template <typename T>
inline constexpr T sqrt_pi = T(1.7724538509055160272981674833411451827975L);

template <typename T>
inline constexpr T sqrt_half_pi = T(1.2533141373155002512078826424055226265035L);

template <typename T>
inline constexpr T two_pi = 2.0 * pi<T>;

template <typename T>
inline constexpr T two_pi_2 = two_pi<T>* two_pi<T>;

template <typename T>
inline constexpr T one_over_pi = 1.0 / pi<T>;

template <typename T>
inline constexpr T two_over_pi = 2.0 / pi<T>;

template <typename T>
inline constexpr T one_over_pi_2 = 1.0 / (pi<T> * pi<T>);

template <typename T>
inline constexpr T one_over_two_pi = 1.0 / two_pi<T>;

template <typename T>
inline constexpr T pi_over_two = pi<T> * 0.5;

template <typename T>
inline constexpr T pi_over_four = pi<T> * 0.25;

template <typename T>
inline constexpr T sqrt_2 = T(1.4142135623730950488016887242096980785697L);

template <typename T>
inline constexpr T sqrt_2_over_2 = T(0.7071067811865476);

template <typename T>
inline constexpr T log_2 = T(0.6931471805599453094172321214581765680755L);

template <typename T>
inline constexpr T log_10 = T(2.3025850929940456840179914546843642076011L);

template <typename T>
inline constexpr T log_pi = T(1.1447298858494001741434273513530587116473L);

template <typename T>
inline constexpr T log_two_pi = T(1.8378770664093454835606594728112352797228L);

template <typename T>
inline constexpr T log_sqrt_two_pi = T(0.9189385332046727417803297364056176398614L);

#include "mts/detail/cmath.h"

template <typename T>
inline constexpr _VMTS::floating_point_return_t<T> exp2(T value) {
  return _VMTS::math::pow((T)2, value);
}

template <typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
inline constexpr T is_power_of_two(T v) {
  return v && !(v & (v - 1));
}

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T round_to_power_of_two(T value) {
  // Compute the next highest power of 2 of 32-bit v.
  unsigned int v = static_cast<unsigned int>(value);
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
inline constexpr int log2_of_power_of_two(std::uint32_t v) {
  constexpr int multiply_debruijn_bit_position_2[32] = { 0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 31,
    27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };

  return multiply_debruijn_bit_position_2[(uint32_t)(v * 0x077CB531U) >> 27];
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T square(T x) {
  return x * x;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T cube(T x) {
  return x * x * x;
}

template <typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
inline constexpr bool is_approximately_equal(T a, T b, T tolerance = std::numeric_limits<T>::epsilon()) {
  const T diff = abs(a - b);
  return (diff <= tolerance) || (diff < maximum(abs(a), abs(b)) * tolerance);
}

template <typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
inline constexpr bool is_approximately_zero(T a, T tolerance = std::numeric_limits<T>::epsilon()) {
  return abs(a) <= tolerance;
}

template <typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
inline constexpr bool is_definitely_less_than(T a, T b, T tolerance = std::numeric_limits<T>::epsilon()) {
  const T diff = a - b;
  return (diff < tolerance) || (diff < maximum(abs(a), abs(b)) * tolerance);
}

template <typename T, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
inline constexpr bool is_definitely_greater_than(T a, T b, T tolerance = std::numeric_limits<T>::epsilon()) {
  const T diff = a - b;
  return (diff > tolerance) || (diff > maximum(abs(a), abs(b)) * tolerance);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T degree_to_radian(T value) {
  return value / (T)360.0 * two_pi<T>;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T radian_to_degree(T value) {
  return value * one_over_two_pi<T> * (T)360.0;
}
MTS_END_SUB_NAMESPACE(math)
