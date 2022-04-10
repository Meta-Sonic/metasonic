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
#include "mts/traits.h"

MTS_BEGIN_NAMESPACE
namespace enum_traits_detail {
template <class T>
using count_t = decltype((void)T::count, void());

template <class T>
using Count_t = decltype((void)T::Count, void());

template <class Enum>
inline constexpr bool has_enum_count_v = _VMTS::is_detected_v<count_t, Enum>;

template <class Enum>
inline constexpr bool has_enum_Count_v = _VMTS::is_detected_v<Count_t, Enum>;

} // namespace enum_traits_detail.

template <class Enum>
using is_sequence_enum
    = std::bool_constant<(enum_traits_detail::has_enum_count_v<Enum> || enum_traits_detail::has_enum_Count_v<Enum>)&&(
        enum_traits_detail::has_enum_count_v<Enum> != enum_traits_detail::has_enum_Count_v<Enum>)>;

template <class Enum>
inline constexpr bool is_sequence_enum_v = is_sequence_enum<Enum>::value;

template <class Enum, class Enable = void>
struct sequence_enum_count {};

template <class Enum>
struct sequence_enum_count<Enum, std::enable_if_t<is_sequence_enum_v<Enum>>> {
  static constexpr Enum value = []() {
    if constexpr (enum_traits_detail::has_enum_count_v<Enum>) {
      return Enum::count;
    }
    else {
      return Enum::Count;
    }
  }();
};

MTS_END_NAMESPACE
