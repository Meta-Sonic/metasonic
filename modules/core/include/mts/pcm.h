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
#include <cstdint>
#include <type_traits>

MTS_BEGIN_NAMESPACE
struct pcm {
  enum type {
    pcm_8,
    pcm_16,
    pcm_24,
    pcm_32,
  };

  template <typename T, type c_opts, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
  static inline T convert(const std::uint8_t* data) noexcept {

    if constexpr (c_opts == type::pcm_8) {
      constexpr T div = 1 << 7;
      return static_cast<T>(int(data[0]) - 128) / div;
    }
    else if constexpr (c_opts == type::pcm_16) {
      constexpr T denom = T(1.0) / (T)(1 << 15);
      const std::int16_t* value_data = reinterpret_cast<const std::int16_t*>(data);

      return (T)((*value_data) * denom);
    }
    else if constexpr (c_opts == type::pcm_24) {
      constexpr T denom = 1.0 / (T)8388608.0;
      std::int32_t value = (static_cast<std::int32_t>(data[2]) << 16) | (static_cast<std::int32_t>(data[1]) << 8)
          | static_cast<std::int32_t>(data[0]);

      // If the 24th bit is set, this is a negative number in 24-bit world.
      // Make sure sign is extended to the 32 bit float.
      return (value & 0x800000 ? (value | ~0xFFFFFF) : value) * denom;
    }
    else if constexpr (c_opts == type::pcm_32) {
      constexpr T div = long(1) << long(31);
      const std::int32_t* value_data = reinterpret_cast<const std::int32_t*>(data);

      return *value_data / div;
    }
  }
};
MTS_END_NAMESPACE
