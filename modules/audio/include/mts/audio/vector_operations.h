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
#include "mts/audio/detail/vector_operations_vdsp.h"

MTS_BEGIN_SUB_NAMESPACE(vec)
struct optimized_op {};
struct default_op {};

namespace detail {
template <typename O>
using op = std::conditional_t<std::is_same_v<O, optimized_op>,
    std::conditional_t<mts::is_defined_v<ops_type>, ops_type, base_ops>, base_ops>;
} // namespace detail.

template <typename O = optimized_op, typename T>
inline void clear(T* sd, stride_t s_sd, length_t length) {
  detail::op<O>::clear(sd, s_sd, length);
}

template <typename O = optimized_op, typename T>
inline void fill(T* sd, stride_t s_sd, T value, length_t length) {
  detail::op<O>::fill(sd, s_sd, value, length);
}

template <typename O = optimized_op, typename T>
inline void copy(const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::copy(s1, s_s1, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void lshift(T* sd, length_t delta, length_t length) {
  detail::op<O>::lshift(sd, delta, length);
}

template <typename O = optimized_op, typename T>
inline void clip(const T* s1, stride_t s_s1, T min, T max, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::clip(s1, s_s1, min, max, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void abs(const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::abs(s1, s_s1, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void add(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::add(s1, s_s1, value, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void mul(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::mul(s1, s_s1, value, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void div(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::div(s1, s_s1, value, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void vdiv(T value, const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
  detail::op<O>::vdiv(value, s1, s_s1, d1, s_d1, length);
}

template <typename O = optimized_op, typename T>
inline void sqrt(const T* s1, T* d1, length_t length) {
  detail::op<O>::sqrt(s1, d1, length);
}

template <typename O = optimized_op, typename T>
inline void sin(const T* s1, T* d1, length_t length) {
  detail::op<O>::sin(s1, d1, length);
}

template <typename O = optimized_op, typename T>
inline void cos(const T* s1, T* d1, length_t length) {
  detail::op<O>::cos(s1, d1, length);
}

template <typename O = optimized_op, typename T>
inline void tan(const T* s1, T* d1, length_t length) {
  detail::op<O>::tan(s1, d1, length);
}

template <typename O = optimized_op, typename T>
inline void tanh(const T* s1, T* d1, length_t length) {
  detail::op<O>::tanh(s1, d1, length);
}

template <typename O = optimized_op, typename T>
inline void convert_from_int8(const std::int8_t* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_int8(input, input_stride, output, size);
}

template <typename O = optimized_op, typename T>
inline void convert_from_int16(const std::int16_t* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_int16(input, input_stride, output, size);
}

template <typename O = optimized_op, typename T>
inline void convert_from_int24(const mts::int24_t* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_int24(input, input_stride, output, size);
}

template <typename O = optimized_op, typename T>
inline void convert_from_int32(const std::int32_t* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_int32(input, input_stride, output, size);
}

template <typename O = optimized_op, typename T>
inline void convert_from_float(const float* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_float(input, input_stride, output, size);
}

template <typename O = optimized_op, typename T>
inline void convert_from_double(const double* input, stride_t input_stride, T* output, length_t size) {
  detail::op<O>::convert_from_double(input, input_stride, output, size);
}
MTS_END_SUB_NAMESPACE(vec)
