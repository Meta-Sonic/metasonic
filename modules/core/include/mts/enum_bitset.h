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
#include "mts/enum_traits.h"
#include <algorithm>
#include <bitset>

MTS_BEGIN_NAMESPACE

template <class Enum>
struct enum_bitset_base : private std::bitset<std::size_t(_VMTS::sequence_enum_count<Enum>::value)> {
  using enum_type = Enum;
  static constexpr std::size_t enum_count = std::size_t(_VMTS::sequence_enum_count<Enum>::value);

  using bitset_t = std::bitset<enum_count>;
  using bitset_t::all;
  using bitset_t::any;
  using bitset_t::count;
  using bitset_t::flip;
  using bitset_t::reset;
  using bitset_t::size;
  using bitset_t::test;
  using bitset_t::operator==;
  using bitset_t::operator!=;
  using bitset_t::operator~;
  using bitset_t::operator&=;
  using bitset_t::operator|=;
  using bitset_t::operator^=;
  using bitset_t::operator<<=;
  using bitset_t::operator>>=;

  template <Enum E>
  inline bool get() const {
    return bitset_t::test((std::size_t)E);
  }

  inline bool get(Enum e) const { return bitset_t::test((std::size_t)e); }

  template <Enum E>
  inline enum_bitset_base& set(bool value) {
    bitset_t::set((std::size_t)E, value);
    return *this;
  }

  inline enum_bitset_base& set(Enum e, bool value) {
    bitset_t::set((std::size_t)e, value);
    return *this;
  }

  template <Enum E>
  inline enum_bitset_base& set() noexcept {
    bitset_t::set((std::size_t)E);
    return *this;
  }

  inline enum_bitset_base& set(Enum e) noexcept {
    bitset_t::set((std::size_t)e);
    return *this;
  }

  template <Enum E>
  inline enum_bitset_base& flip() {
    bitset_t::flip((std::size_t)E);
    return *this;
  }

  inline enum_bitset_base& flip(Enum e) {
    bitset_t::flip((std::size_t)e);
    return *this;
  }

  template <Enum E>
  inline enum_bitset_base& reset() {
    bitset_t::reset((std::size_t)E);
    return *this;
  }

  inline enum_bitset_base& reset(Enum e) {
    bitset_t::reset((std::size_t)e);
    return *this;
  }
};

MTS_END_NAMESPACE
