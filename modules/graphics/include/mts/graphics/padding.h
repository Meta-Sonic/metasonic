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
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

MTS_BEGIN_NAMESPACE
template <typename _Tp>
class padding {
public:
  using value_type = _Tp;
  static_assert(std::is_arithmetic_v<value_type>, "value_type is not arithmetic");

  value_type left, right, top, bottom;

  inline constexpr padding() noexcept
      : left(0)
      , right(0)
      , top(0)
      , bottom(0) {}

  inline constexpr padding(value_type Left, value_type Right, value_type Top, value_type Bottom) noexcept
      : left(Left)
      , right(Right)
      , top(Top)
      , bottom(Bottom) {}

  template <typename T>
  inline constexpr padding(T Left, T Right, T Top, T Bottom) noexcept
      : left(Left)
      , right(Right)
      , top(Top)
      , bottom(Bottom) {}

  padding(const std::string& padding_str)
      : padding() {
    // Split string.
    std::vector<std::string> elem;
    std::string r = padding_str;
    size_t pos = 0;
    std::string token;
    const std::string delimiter(",");

    while ((pos = r.find(delimiter)) != std::string::npos) {
      token = r.substr(0, pos);
      elem.push_back(token);
      r.erase(0, pos + delimiter.length());
    }

    elem.push_back(r);

    if (elem.size() != 4) {
      return;
    }

    left = (value_type)std::stod(elem[0]);
    right = (value_type)std::stod(elem[1]);
    top = (value_type)std::stod(elem[2]);
    bottom = (value_type)std::stod(elem[3]);
  }

  inline constexpr padding& operator=(const padding& p) noexcept {
    left = p.left;
    right = p.right;
    top = p.top;
    bottom = p.bottom;
    return *this;
  }

  template <typename T>
  inline constexpr padding& operator=(const padding<T>& p) noexcept {
    left = p.left;
    right = p.right;
    top = p.top;
    bottom = p.bottom;
    return *this;
  }

  inline constexpr value_type width() const noexcept { return left + right; }

  inline constexpr value_type height() const noexcept { return top + bottom; }

  template <typename P>
  inline constexpr padding<P> cast() const noexcept {
    return padding<P>((P)left, (P)right, (P)top, (P)bottom);
  }

  template <typename T>
  inline constexpr bool operator==(const padding<T>& p) const noexcept {
    return (left == p.left && right == p.right && top == p.top && bottom == p.bottom);
  }

  template <typename T>
  inline constexpr bool operator!=(const padding<T>& p) const noexcept {
    return !this->operator==(p);
  }

  std::string to_string() const {
    return std::to_string(left) + ", " + std::to_string(right) + ", " + std::to_string(top) + ", "
        + std::to_string(bottom);
  }

  friend std::ostream& operator<<(std::ostream& stream, const padding& p) {
    stream << p.left << ", " << p.right << ", " << p.top << ", " << p.bottom;
    return stream;
  }
};

template <typename T>
using margin = padding<T>;
MTS_END_NAMESPACE
