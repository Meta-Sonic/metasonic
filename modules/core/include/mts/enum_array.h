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
#include <array>

MTS_BEGIN_NAMESPACE

template <class T, class Enum>
struct enum_array_base : private std::array<T, std::size_t(_VMTS::sequence_enum_count<Enum>::value)> {
  using enum_type = Enum;
  static constexpr std::size_t enum_count = std::size_t(_VMTS::sequence_enum_count<Enum>::value);

  using array_t = std::array<T, enum_count>;
  using value_type = typename array_t::value_type;
  using reference = typename array_t::reference;
  using const_reference = typename array_t::const_reference;
  using iterator = typename array_t::iterator;
  using const_iterator = typename array_t::const_iterator;
  using pointer = typename array_t::pointer;
  using const_pointer = typename array_t::const_pointer;
  using size_type = typename array_t::size_type;
  using difference_type = typename array_t::difference_type;
  using reverse_iterator = typename array_t::reverse_iterator;
  using const_reverse_iterator = typename array_t::const_reverse_iterator;

  template <class... Ts>
  inline constexpr enum_array_base(Ts&&... ts)
      : array_t{ std::forward<Ts>(ts)... } {}

  using array_t::back;
  using array_t::begin;
  using array_t::cbegin;
  using array_t::cend;
  using array_t::crbegin;
  using array_t::crend;
  using array_t::data;
  using array_t::empty;
  using array_t::end;
  using array_t::fill;
  using array_t::front;
  using array_t::max_size;
  using array_t::rbegin;
  using array_t::rend;
  using array_t::size;

  template <Enum E>
  inline constexpr reference at() noexcept {
    return std::get<std::size_t(E)>(*this);
  }

  template <Enum E>
  inline constexpr const_reference at() const noexcept {
    return std::get<std::size_t(E)>(*this);
  }

  inline constexpr reference at(Enum e) noexcept { return array_t::at(size_type(e)); }
  inline constexpr const_reference at(Enum e) const noexcept { return array_t::at(size_type(e)); }

  inline constexpr reference operator[](Enum e) noexcept { return array_t::operator[](size_type(e)); }
  inline constexpr const_reference operator[](Enum e) const noexcept { return array_t::operator[](size_type(e)); }

  inline constexpr void swap(enum_array_base& __a) noexcept(std::is_nothrow_swappable_v<value_type>) {
    std::swap_ranges(data(), data() + enum_count, __a.data());
  }

protected:
  using array_t::at;
  using array_t::operator[];

  inline constexpr array_t& to_array() noexcept { return *this; }

  inline constexpr const array_t& to_array() const noexcept { return *this; }
};

template <class T, class Enum, bool IntegerAccess = false, class Enable = void>
struct enum_array {
  static_assert(always_false_v<T>, "Enum type must has either a count or Count value but not both");
};

template <class T, class Enum>
struct enum_array<T, Enum, false, std::enable_if_t<_VMTS::is_sequence_enum_v<Enum>>> : public enum_array_base<T, Enum> {
  using base_t = enum_array_base<T, Enum>;
  using base_t::base_t;
};

template <class T, class Enum>
struct enum_array<T, Enum, true, std::enable_if_t<_VMTS::is_sequence_enum_v<Enum>>> : public enum_array_base<T, Enum> {
  using base_t = enum_array_base<T, Enum>;
  using base_t::at;
  using base_t::base_t;
  using base_t::operator[];
  using base_t::to_array;
};

template <class T, class Enum>
inline constexpr bool operator==(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class T, class Enum>
inline constexpr bool operator!=(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return !(__x == __y);
}

template <class T, class Enum>
inline constexpr bool operator<(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class T, class Enum>
inline constexpr bool operator>(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return __y < __x;
}

template <class T, class Enum>
inline constexpr bool operator<=(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return !(__y < __x);
}

template <class T, class Enum>
inline constexpr bool operator>=(const enum_array_base<T, Enum>& __x, const enum_array_base<T, Enum>& __y) {
  return !(__x < __y);
}
MTS_END_NAMESPACE

namespace std {

//

template <auto I, class T, class Enum, typename = _VMTS::enable_if_same_t<decltype(I), Enum>>
inline constexpr T& get(_VMTS::enum_array_base<T, Enum>& a) noexcept {
  return a.template at<I>();
}

template <auto I, class T, class Enum, typename = _VMTS::enable_if_same_t<decltype(I), Enum>>
inline constexpr T&& get(_VMTS::enum_array_base<T, Enum>&& a) noexcept {
  return a.template at<I>();
}

template <auto I, class T, class Enum, typename = _VMTS::enable_if_same_t<decltype(I), Enum>>
inline constexpr const T& get(const _VMTS::enum_array_base<T, Enum>& a) noexcept {
  return a.template at<I>();
}

template <auto I, class T, class Enum, typename = _VMTS::enable_if_same_t<decltype(I), Enum>>
inline constexpr const T&& get(const _VMTS::enum_array_base<T, Enum>&& a) noexcept {
  return a.template at<I>();
}

template <std::size_t I, class T, class Enum>
inline constexpr T& get(_VMTS::enum_array<T, Enum, true>& a) noexcept {
  return std::get<I>(a.to_array());
}

template <std::size_t I, class T, class Enum>
inline constexpr T&& get(_VMTS::enum_array<T, Enum, true>&& a) noexcept {
  return std::get<I>(a.to_array());
}

template <std::size_t I, class T, class Enum>
inline constexpr const T& get(const _VMTS::enum_array<T, Enum, true>& a) noexcept {
  return std::get<I>(a.to_array());
}

template <std::size_t I, class T, class Enum>
inline constexpr const T&& get(const _VMTS::enum_array<T, Enum, true>&& a) noexcept {
  return std::get<I>(a.to_array());
}
} // namespace std.
