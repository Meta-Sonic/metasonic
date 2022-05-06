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
#include "mts/graphics/point.h"

MTS_BEGIN_NAMESPACE
template <typename _Tp>
class quad {
public:
  using value_type = _Tp;
  static_assert(std::is_arithmetic_v<value_type>, "value_type is not arithmetic");

  using point_type = mts::point<value_type>;

  point_type top_left;
  point_type top_right;
  point_type bottom_right;
  point_type bottom_left;

  inline constexpr quad(const point_type& tl, const point_type& tr, const point_type& br, const point_type& bl) noexcept
      : top_left(tl)
      , top_right(tr)
      , bottom_right(br)
      , bottom_left(bl)

  {}

  inline constexpr quad& operator=(const quad& p) noexcept = default;

  template <typename T>
  inline constexpr quad& operator=(const quad<T>& p) noexcept {
    top_left = p.top_left;
    top_right = p.top_right;
    bottom_right = p.bottom_right;
    bottom_left = p.bottom_left;

    return *this;
  }

  template <typename P>
  inline constexpr quad<P> cast() const noexcept {
    return quad<P>((P)top_left, (P)top_right, (P)bottom_right, (P)bottom_left);
  }

  template <typename T>
  inline constexpr bool operator==(const quad<T>& p) const noexcept {
    return (top_left == p.top_left && top_right == p.top_right && bottom_right == p.bottom_right
        && bottom_left == p.bottom_left);
  }

  template <typename T>
  inline constexpr bool operator!=(const quad<T>& p) const noexcept {
    return !this->operator==(p);
  }

  friend std::ostream& operator<<(std::ostream& stream, const quad& p) {
    stream << "[{" << p.top_left << "}, {" << p.top_right << "}, {" << p.bottom_right << "}, {" << p.bottom_left
           << "}]";
    return stream;
  }
};
MTS_END_NAMESPACE
