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

template <typename _FlagType = int>
inline constexpr _FlagType get_flag_value(_FlagType index) noexcept {
  return index > 1 ? (_FlagType(1) << (index - 1)) : index;
}

template <typename _FlagType = int>
inline constexpr bool has_flag(_FlagType flags_to_verity, _FlagType flags) noexcept {
  return ((flags & flags_to_verity) != 0);
}

template <typename _FlagType>
inline constexpr void set_flag(_FlagType flag_to_set, bool value, _FlagType& flags) noexcept {
  if (value) {
    flags = (_FlagType)(flags | flag_to_set);
    return;
  }

  flags = (_FlagType)(flags & ~flag_to_set);
}

#define MTS_DECLARE_ENUM_CLASS_FLAGS(enum_class)                                                                      \
  inline enum_class operator|(enum_class lhs, enum_class rhs) {                                                        \
    using type = std::underlying_type_t<enum_class>;                                                                   \
    return (enum_class)(static_cast<type>(lhs) | static_cast<type>(rhs));                                              \
  }                                                                                                                    \
                                                                                                                       \
  inline enum_class operator&(enum_class lhs, enum_class rhs) {                                                        \
    using type = std::underlying_type_t<enum_class>;                                                                   \
    return (enum_class)(static_cast<type>(lhs) & static_cast<type>(rhs));                                              \
  }                                                                                                                    \
                                                                                                                       \
  inline enum_class& operator|=(enum_class& lhs, enum_class rhs) { return lhs = (lhs | rhs); }                         \
  inline enum_class& operator&=(enum_class& lhs, enum_class rhs) { return lhs = (lhs & rhs); }                         \
                                                                                                                       \
  inline enum_class& operator~(enum_class& lhs) {                                                                      \
    using type = std::underlying_type_t<enum_class>;                                                                   \
    return lhs = (enum_class)(~type(lhs));                                                                             \
  }                                                                                                                    \
                                                                                                                       \
  inline bool operator==(enum_class lhs, std::underlying_type_t<enum_class> value) {                                   \
    return std::underlying_type_t<enum_class>(lhs) == value;                                                           \
  }                                                                                                                    \
                                                                                                                       \
  inline bool operator!=(enum_class lhs, std::underlying_type_t<enum_class> value) {                                   \
    return std::underlying_type_t<enum_class>(lhs) != value;                                                           \
  }

MTS_END_NAMESPACE
