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
#include "mts/audio/detail/vector_operations_base.h"

#define __MTS_AUDIO_OPS_F(_) static _(float)
#define __MTS_AUDIO_OPS_D(_) static _(double)
#define __MTS_AUDIO_OPS_I(_) static _(int)

#define __MTS_AUDIO_OPS_FD(_)                                                                                          \
  __MTS_AUDIO_OPS_F(_);                                                                                                \
  __MTS_AUDIO_OPS_D(_)

#define __MTS_AUDIO_OPS_FI(_)                                                                                          \
  __MTS_AUDIO_OPS_F(_);                                                                                                \
  __MTS_AUDIO_OPS_I(_)

#define __MTS_AUDIO_OPS_DI(_)                                                                                          \
  __MTS_AUDIO_OPS_D(_);                                                                                                \
  __MTS_AUDIO_OPS_I(_)

#define __MTS_AUDIO_OPS_FDI(_)                                                                                         \
  __MTS_AUDIO_OPS_F(_);                                                                                                \
  __MTS_AUDIO_OPS_D(_);                                                                                                \
  __MTS_AUDIO_OPS_I(_)

#define MTS_AUDIO_DECLARE_OP_F(NAME) __MTS_AUDIO_OPS_F(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_D(NAME) __MTS_AUDIO_OPS_D(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_I(NAME) __MTS_AUDIO_OPS_I(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_FD(NAME) __MTS_AUDIO_OPS_FD(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_FDI(NAME) __MTS_AUDIO_OPS_FDI(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_FI(NAME) __MTS_AUDIO_OPS_FI(MTS_CONCAT(MTS_AUDIO_OP_, NAME))
#define MTS_AUDIO_DECLARE_OP_DI(NAME) __MTS_AUDIO_OPS_DI(MTS_CONCAT(MTS_AUDIO_OP_, NAME))

/// @def MTS_AUDIO_OP_CLEAR
#define MTS_AUDIO_OP_CLEAR(TYPE) void clear(TYPE* sd, stride_t s_sd, length_t length)

/// @def MTS_AUDIO_OP_FILL
#define MTS_AUDIO_OP_FILL(TYPE) void fill(TYPE* sd, stride_t s_sd, TYPE value, length_t length)

/// @def MTS_AUDIO_OP_COPY
/// d1[i] = s1[i]
#define MTS_AUDIO_OP_COPY(TYPE) void copy(const TYPE* s1, stride_t s_s1, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_LSHIFT
#define MTS_AUDIO_OP_LSHIFT(TYPE) void lshift(TYPE* sd, length_t delta, length_t length)

/// @def MTS_AUDIO_OP_CLIP
/// d1[i] = clamp(s1[i], min, max)
#define MTS_AUDIO_OP_CLIP(TYPE)                                                                                        \
  void clip(const TYPE* s1, stride_t s_s1, TYPE min, TYPE max, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_ABS
/// d1[i] = abs(s1[i])
#define MTS_AUDIO_OP_ABS(TYPE) void abs(const TYPE* s1, stride_t s_s1, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_NORMALIZE
#define MTS_AUDIO_OP_NORMALIZE(TYPE)                                                                                   \
  void normalize(const TYPE* s1, stride_t s_s1, TYPE* d1, stride_t s_d1, length_t length)

//
//
//

/// @def MTS_AUDIO_OP_SQRT
/// d1[i] = sqrt(s1[i])
#define MTS_AUDIO_OP_SQRT(TYPE) void sqrt(const TYPE* s1, TYPE* d1, length_t length)

/// @def MTS_AUDIO_OP_SIN
/// d1[i] = sin(s1[i])
#define MTS_AUDIO_OP_SIN(TYPE) void sin(const TYPE* s1, TYPE* d1, length_t length)

/// @def MTS_AUDIO_OP_COS
/// d1[i] = cos(s1[i])
#define MTS_AUDIO_OP_COS(TYPE) void cos(const TYPE* s1, TYPE* d1, length_t length)

/// @def MTS_AUDIO_OP_TAN
/// d1[i] = tan(s1[i])
#define MTS_AUDIO_OP_TAN(TYPE) void tan(const TYPE* s1, TYPE* d1, length_t length)

/// @def MTS_AUDIO_OP_TANH
/// d1[i] = tanh(s1[i])
#define MTS_AUDIO_OP_TANH(TYPE) void tanh(const TYPE* s1, TYPE* d1, length_t length)

//
//
//

/// @def MTS_AUDIO_OP_S_ADD
/// d1[i] = s1[i] + value
#define MTS_AUDIO_OP_S_ADD(TYPE)                                                                                       \
  void add(const TYPE* s1, stride_t s_s1, TYPE value, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_S_MUL
/// d1[i] = s1[i] * value
#define MTS_AUDIO_OP_S_MUL(TYPE)                                                                                       \
  void mul(const TYPE* s1, stride_t s_s1, TYPE value, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_S_DIV
/// d1[i] = s1[i] / value
#define MTS_AUDIO_OP_S_DIV(TYPE)                                                                                       \
  void div(const TYPE* s1, stride_t s_s1, TYPE value, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_S_VDIV
/// d1[i] = value / s1[i]
#define MTS_AUDIO_OP_S_VDIV(TYPE)                                                                                      \
  void vdiv(TYPE value, const TYPE* s1, stride_t s_s1, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_V_SUM
/// d1[i] = s1[i] + s2[i]
#define MTS_AUDIO_OP_V_SUM(TYPE)                                                                                       \
  void sum(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_V_SUB
/// d1[i] = s1[i] - s2[i]
#define MTS_AUDIO_OP_V_SUB(TYPE)                                                                                       \
  void sub(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_V_MUL
/// d1[i] = s1[i] * s2[i]
#define MTS_AUDIO_OP_V_MUL(TYPE)                                                                                       \
  void mul(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_V_DIV
/// d1[i] = s1[i] / s2[i]
#define MTS_AUDIO_OP_V_DIV(TYPE)                                                                                       \
  void div(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE* d1, stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_SUM_S_MUL
/// d1[i] = (s1[i] + s2[i]) * value
#define MTS_AUDIO_OP_SUM_S_MUL(TYPE)                                                                                   \
  void sum_mul(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE value, TYPE* d1, stride_t s_d1,      \
      length_t length)

/// @def MTS_AUDIO_OP_SUM_V_MUL
/// d1[i] = (s1[i] + s2[i]) * s3[i]
#define MTS_AUDIO_OP_SUM_V_MUL(TYPE)                                                                                   \
  void sum_mul(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, const TYPE* s3, stride_t s_s3, TYPE* d1,  \
      stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OP_SUM_MUL
/// d1[i] = (s1[i] + value) * s2[i]
#define MTS_AUDIO_OP_SUM_MUL(TYPE)                                                                                     \
  void sum_mul(const TYPE* s1, stride_t s_s1, TYPE value, const TYPE* src2, stride_t s_s2, TYPE* d1, stride_t s_d1,    \
      length_t length)

/// @def MTS_AUDIO_OP_MUL_S_SUM
/// d1[i] = (s1[i] * s2[i]) + value
#define MTS_AUDIO_OP_MUL_S_SUM(TYPE)                                                                                   \
  void mul_sum(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, TYPE value, TYPE* d1, stride_t s_d1,      \
      length_t length)

/// @def MTS_AUDIO_OPS_MUL_V_SUM
/// d1[i] = (s1[i] * s2[i]) + s3[i]
#define MTS_AUDIO_OPS_MUL_V_SUM(TYPE)                                                                                  \
  void mul_sum(const TYPE* s1, stride_t s_s1, const TYPE* s2, stride_t s_s2, const TYPE* s3, stride_t s_s3, TYPE* d1,  \
      stride_t s_d1, length_t length)

/// @def MTS_AUDIO_OPS_MUL_SUM
/// d1[i] = (s1[i] * value) + s2[i]
#define MTS_AUDIO_OPS_MUL_SUM(TYPE)                                                                                    \
  void mul_sum(const TYPE* s1, stride_t s_s1, TYPE value, const TYPE* s2, stride_t s_s2, TYPE* d1, stride_t s_d1,      \
      length_t length)
