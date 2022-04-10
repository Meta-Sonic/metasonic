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
#include "mts/math.h"
#include <type_traits>

MTS_BEGIN_NAMESPACE

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T ms_to_seconds(T ms) {
  return ms * (T)0.001;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T seconds_to_ms(T s) {
  return s * (T)1000.0;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr int time_to_samples(T sampling_rate, T seconds) {
  return std::ceil(sampling_rate * seconds);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T seconds_to_samples(T sampling_rate, T s) {
  return sampling_rate * s;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T ms_to_samples(T sampling_rate, T ms) {
  return sampling_rate * ms_to_seconds(ms);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T samples_to_time(T sampling_rate, T count) {
  return count / sampling_rate;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T samples_to_ms(T sampling_rate, T count) {
  return count / sampling_rate * (T)0.001;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T degree_to_radian(T value) {
  return value / (T)360.0 * math::two_pi<T>;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T radian_to_degree(T value) {
  return value * math::one_over_two_pi<T> * (T)360.0;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline T to_dB(T value, T offset = 0) {
  return offset + 20.0 * std::log10(value < 1e-23 ? 1e-23 : value);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline T from_dB(T value, T offset = 0) {
  return std::pow((T)10.0, (value - offset) / (T)20.0);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline T power_to_dB(T value, T delta = 0) {
  constexpr T denormal = 1.0e-23;
  return (T)10.0 * std::log10(value < denormal ? denormal : value) - delta;
}

namespace cexpr {
template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T to_dB(T value, T offset = 0) noexcept {
  return offset + 20.0 * math::log10(value < 1e-23 ? 1e-23 : value);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline T power_to_dB(T value, T delta = 0) {
  constexpr T denormal = 1.0e-23;
  return (T)10.0 * math::log10(value < denormal ? denormal : value) - delta;
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
inline constexpr T from_dB(T value, T offset = 0) noexcept {
  return math::pow((T)10.0, (value - offset) / (T)20.0);
}
} // namespace cexpr.

MTS_END_NAMESPACE
