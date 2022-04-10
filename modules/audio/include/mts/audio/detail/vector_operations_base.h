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
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <type_traits>

#define __MTS_AUDIO_USING_OP(_)                                                                                        \
  using base_ops::_;                                                                                                   \
  using impl_ops::_

#define __MTS_AUDIO_OP_LIST(_)                                                                                         \
  _(clear);                                                                                                            \
  _(fill);                                                                                                             \
  _(copy);                                                                                                             \
  _(lshift);                                                                                                           \
  _(abs);                                                                                                              \
  _(add);                                                                                                              \
  _(mul);                                                                                                              \
  _(div);                                                                                                              \
  _(vdiv);                                                                                                             \
  _(sqrt);                                                                                                             \
  _(sin);                                                                                                              \
  _(cos);                                                                                                              \
  _(tan);                                                                                                              \
  _(tanh)

#define __MTS_AUDIO_OPS_DECLARE_USING() __MTS_AUDIO_OP_LIST(__MTS_AUDIO_USING_OP)

MTS_BEGIN_SUB_NAMESPACE(vec)
using stride_t = long;
using index_t = std::size_t;
using length_t = std::size_t;

template <typename T>
struct indexed_value {
  index_t index;
  T value;
};

struct ops_type;

struct base_ops {
private:
  template <typename T>
  static inline T& sincr(T*& sd, stride_t s_sd) {
    T& v = *sd;
    sd += s_sd;
    return v;
  }

  template <typename T>
  static inline T sincr(const T*& sd, stride_t s_sd) {
    T v = *sd;
    sd += s_sd;
    return v;
  }

public:
  template <typename T>
  static inline void clear(T* sd, stride_t s_sd, length_t length) {
    if (s_sd == 1) {
      std::memset((void*)sd, 0, length * sizeof(T));
      return;
    }

    while (length--) {
      sincr(sd, s_sd) = 0;
    }
  }

  template <typename T>
  static inline void fill(T* sd, stride_t s_sd, T value, length_t length) {
    while (length--) {
      sincr(sd, s_sd) = value;
    }
  }

  template <typename T>
  static inline void copy(const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
    if (s_s1 == 1 && s_d1 == 1) {
      std::memmove((void*)d1, (const void*)s1, length * sizeof(T));
      return;
    }

    while (length--) {
      sincr(d1, s_d1) = sincr(s1, s_s1);
    }
  }

  template <typename T>
  static inline void clip(const T* s1, stride_t s_s1, T min, T max, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = std::clamp(sincr(s1, s_s1), min, max);
    }
  }

  template <typename T>
  static inline void abs(const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = std::abs(sincr(s1, s_s1));
    }
  }

  template <typename T>
  static inline void lshift(T* sd, length_t delta, length_t length) {
    std::memmove((void*)sd, (const void*)(sd + delta), length * sizeof(T));
  }

  template <typename T>
  static inline void add(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = sincr(s1, s_s1) + value;
    }
  }

  template <typename T>
  static inline void mul(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = sincr(s1, s_s1) * value;
    }
  }

  template <typename T>
  static inline void div(const T* s1, stride_t s_s1, T value, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = sincr(s1, s_s1) / value;
    }
  }

  template <typename T>
  static inline void vdiv(T value, const T* s1, stride_t s_s1, T* d1, stride_t s_d1, length_t length) {
    while (length--) {
      sincr(d1, s_d1) = value / sincr(s1, s_s1);
    }
  }

  template <typename T>
  static inline void sqrt(const T* s1, T* d1, length_t length) {
    while (length--) {
      *d1++ = std::sqrt(*s1++);
    }
  }

  template <typename T>
  static inline void sin(const T* s1, T* d1, length_t length) {
    while (length--) {
      *d1++ = std::sin(*s1++);
    }
  }

  template <typename T>
  static inline void cos(const T* s1, T* d1, length_t length) {
    while (length--) {
      *d1++ = std::cos(*s1++);
    }
  }

  template <typename T>
  static inline void tan(const T* s1, T* d1, length_t length) {
    while (length--) {
      *d1++ = std::tan(*s1++);
    }
  }

  template <typename T>
  static inline void tanh(const T* s1, T* d1, length_t length) {
    while (length--) {
      *d1++ = std::tanh(*s1++);
    }
  }
};

MTS_END_SUB_NAMESPACE(vec)
