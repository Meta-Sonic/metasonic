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
#include "mts/util.h"
#include "mts/graphics/point.h"
#include "mts/graphics/size.h"
#include "mts/graphics/padding.h"

MTS_BEGIN_NAMESPACE
template <typename _Tp>
class rect {
public:
  using value_type = _Tp;
  static_assert(std::is_arithmetic_v<value_type>, "value_type is not arithmetic");
  using size_type = mts::size<value_type>;
  using point_type = mts::point<value_type>;

#if defined(__MTS_CLANG__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wnested-anon-types"
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#endif // __MTS_CLANG__

  union {
    point_type position;
    struct {
      value_type x, y;
    };
  };

  union {
    size_type size;
    struct {
      value_type width, height;
    };
  };

#if defined(__MTS_CLANG__)
  #pragma clang diagnostic pop
#endif // __MTS_CLANG__

  inline constexpr rect() noexcept
      : position()
      , size() {}

  inline constexpr rect(const point_type& pt_, const size_type& size_) noexcept
      : position(pt_)
      , size(size_) {}

  inline constexpr rect(value_type x, value_type y, value_type w, value_type h) noexcept
      : position(x, y)
      , size(w, h) {}

  inline constexpr rect(value_type x, value_type y, const size_type& size_) noexcept
      : position(x, y)
      , size(size_) {}

  inline constexpr rect(const rect& r) noexcept
      : position(r.position)
      , size(r.size) {}

  template <typename T>
  inline constexpr rect(const rect<T>& r) noexcept
      : position(r.position.template cast<value_type>())
      , size(r.size.template cast<value_type>()) {}

  template <typename T>
  inline constexpr rect(const padding<T>& q) noexcept
      : position(q.left, q.top)
      , size(q.width(), q.height()) {}

  inline constexpr rect& operator=(const rect& r) noexcept {
    position = r.position;
    size = r.size;
    return *this;
  }

  inline constexpr static rect from_bottom_left(value_type x, value_type y, value_type w, value_type h) noexcept {
    return rect(x, y - h, w, h);
  }

  template <typename P>
  inline constexpr rect<P> cast() const noexcept {
    return rect<P>(position.template cast<P>(), size.template cast<P>());
  }

  template <typename T>
  inline constexpr rect operator+(const rect<T>& rect) const noexcept {
    return rect(position.x + rect.position.x, position.y + rect.position.y, size.width + rect.size.width,
        size.height + rect.size.height);
  }

  template <typename T>
  inline constexpr rect operator+(const point<T>& pos) const noexcept {
    return rect(position.x + pos.x, position.y + pos.y, size.width, size.height);
  }

  template <typename T>
  inline constexpr rect operator-(const rect<T>& rect) const noexcept {
    return rect(position.x - rect.position.x, position.y - rect.position.y, size.width - rect.size.width,
        size.height - rect.size.height);
  }

  template <typename T>
  inline constexpr rect operator-(const point<T>& pos) const noexcept {
    return rect(position.x - pos.x, position.y - pos.y, size.width, size.height);
  }

  template <typename T>
  inline constexpr rect& operator+=(const point<T>& pos) noexcept {
    position += pos;
    return *this;
  }

  template <typename T>
  inline constexpr rect operator+(const padding<T>& m) const noexcept {
    return rect(position.x - m.left, position.y - m.top, size.width + m.width(), size.height + m.height());
  }

  template <typename T>
  inline constexpr rect with_interior_padding(const padding<T>& p) const noexcept {
    return rect(x + p.left, y + p.top, width - p.width(), height - p.height());
  }

  template <typename T>
  inline constexpr rect with_exterior_padding(const padding<T>& p) const noexcept {
    return rect(x - p.left, y - p.top, width + p.width(), height + p.height());
  }

  template <typename T>
  inline constexpr bool operator==(const rect<T>& rect) const noexcept {
    return position == rect.position && size == rect.size;
  }

  template <typename T>
  inline constexpr bool operator!=(const rect<T>& rect) const noexcept {
    return !operator==(rect);
  }

  inline constexpr value_type left() const noexcept { return position.x; }
  inline constexpr value_type right() const noexcept { return position.x + size.width; }
  inline constexpr value_type top() const noexcept { return position.y; }

  inline constexpr value_type bottom() const noexcept { return position.y + size.height; }

  template <typename T>
  inline constexpr bool intersect(const rect<T>& r) const noexcept {
    return ((mts::minimum(right(), r.right()) - mts::maximum(x, r.x)) > 0)
        && ((mts::minimum(bottom(), r.bottom()) - mts::maximum(y, r.y)) > 0);
  }

  template <typename T>
  inline constexpr bool intersect(const point<T>& p) const noexcept {
    if constexpr (std::is_floating_point_v<value_type>) {
      return ((mts::minimum(right(), p.x + std::numeric_limits<value_type>::epsilon()) - mts::maximum(x, p.x)) >= 0)
          && ((mts::minimum(bottom(), p.y + std::numeric_limits<value_type>::epsilon()) - mts::maximum(y, p.y)) >= 0);
    }
    else {
      return ((mts::minimum(right(), p.x) - mts::maximum(x, p.x)) >= 0)
          && ((mts::minimum(bottom(), p.y) - mts::maximum(y, p.y)) >= 0);
    }
  }

  template <typename T>
  inline constexpr bool contains(const point<T>& pos) const noexcept {
    return pos.x >= position.x && pos.x <= position.x + size.width && pos.y >= position.y
        && pos.y <= position.y + size.height;
  }

  template <typename T>
  inline constexpr rect get_interior_rect(const point<T>& point) const noexcept {
    return rect(position.x + point.x, position.y + point.y, size.width - (value_type)(2.0 * point.x),
        size.height - (value_type)(2.0 * point.y));
  }

  template <typename T>
  inline constexpr rect get_exterior_rect(const point<T>& point) const noexcept {
    return rect(position.x - point.x, position.y - point.y, size.width + (value_type)(2.0 * point.x),
        size.height + (value_type)(2.0 * point.y));
  }

  template <typename T>
  inline constexpr void reduce(const point<T>& point) noexcept {
    position.x += point.x;
    position.y += point.y;
    size.width -= (value_type)(2.0 * point.x);
    size.height -= (value_type)(2.0 * point.y);
  }

  inline constexpr rect reduced(const point_type& point) const noexcept {
    return rect(x + point.x, y + point.y, width - (value_type)(2.0 * point.x), height - (value_type)(2.0 * point.y));
  }

  template <typename T>
  inline constexpr rect reduced(const point<T>& point) const noexcept {
    return rect(x + point.x, y + point.y, width - (value_type)(2.0 * point.x), height - (value_type)(2.0 * point.y));
  }

  inline constexpr rect reduced(value_type v) const noexcept {
    return rect(x + v, y + v, width - (value_type)(2.0 * v), height - (value_type)(2.0 * v));
  }

  inline constexpr rect reduced_vertically(value_type v) const noexcept {
    return rect(x, y + v, width, height - (value_type)(2 * v));
  }

  inline constexpr rect reduced_from_left_percent(float v) const noexcept {
    return reduced_from_left(width * (float)v);
  }

  inline constexpr rect reduced_from_right_percent(float v) const noexcept {
    return reduced_from_right(width * (float)v);
  }

  inline constexpr rect reduced_from_left(value_type x) const noexcept {
    return rect(position.x + x, position.y, size.width - x, size.height);
  }

  inline constexpr rect reduced_from_right(value_type x) const noexcept {
    return rect(position.x, position.y, size.width - x, size.height);
  }

  inline constexpr rect reduced_from_top(value_type y) const noexcept {
    return rect(position.x, position.y + y, size.width, size.height - y);
  }

  inline constexpr rect reduced_from_bottom(value_type y) const noexcept {
    return rect(position.x, position.y, size.width, size.height - y);
  }

  template <typename T>
  inline constexpr void expand(const mts::size<T>& s) noexcept {
    position.x -= s.width;
    position.y -= s.height;
    size.width += (value_type)(2.0 * s.width);
    size.height += (value_type)(2.0 * s.height);
  }

  inline constexpr rect expanded(value_type delta) const noexcept {
    return rect(x - delta, y - delta, width + (value_type)(2.0 * delta), height + (value_type)(2.0 * delta));
  }

  template <typename T>
  inline constexpr rect expanded(const mts::size<T>& s) const noexcept {
    return rect(x - s.width, y - s.height, width + (value_type)(2.0 * s.width), height + (value_type)(2.0 * s.height));
  }

  inline constexpr point_type in_middle(const size_type& s) const noexcept {
    return point_type(x + (width - s.width) * 0.5, y + (height - s.height) * 0.5);
  }

  template <typename T>
  inline constexpr point_type in_middle(const mts::size<T>& s) const noexcept {
    return point_type(x + (width - s.width) * 0.5, y + (height - s.height) * 0.5);
  }

  inline constexpr point_type top_left() const noexcept { return position; }

  inline constexpr point_type top_right(value_type dx = 0, value_type dy = 0) const noexcept {
    return point_type(position.x + size.width + dx, position.y + dy);
  }

  inline constexpr point_type bottom_left() const noexcept { return point_type(position.x, position.y + size.height); }

  inline constexpr point_type bottom_right() const noexcept {
    return point_type(position.x + size.width, position.y + size.height);
  }

  inline constexpr point_type middle() const noexcept {
    return point_type(position.x + size.width * 0.5, position.y + size.height * 0.5);
  }

  inline constexpr point_type middle_right() const noexcept {
    return point_type(position.x + size.width, position.y + size.height * 0.5);
  }

  inline constexpr point_type next_left(value_type delta) const noexcept {
    return point_type(position.x, position.y - delta);
  }

  inline constexpr point_type next_right(value_type delta) const noexcept {
    return point_type(position.x + size.width + delta, position.y);
  }

  inline constexpr point_type next_down(value_type delta) const noexcept {
    return point_type(position.x, position.y + size.height + delta);
  }

  inline constexpr point_type next_up(value_type delta) const noexcept {
    return point_type(position.x, position.y - delta);
  }

  inline constexpr rect with_position(const point_type& point) const noexcept { return rect(point, size); }

  template <typename T>
  inline constexpr rect with_position(const point<T>& point) const noexcept {
    return rect(point, size);
  }

  template <typename T>
  inline constexpr rect with_size(const mts::size<T>& s) const noexcept {
    return rect(position, s);
  }

  inline constexpr rect with_width(value_type w) const noexcept {
    rect r = *this;
    r.width = w;
    return r;
  }

  inline constexpr rect with_height(value_type h) const noexcept {
    rect r = *this;
    r.height = h;
    return r;
  }

  template <typename T>
  inline constexpr void clip_to(const rect<T>& rect) noexcept {
    x = mts::maximum(rect.x, x);
    y = mts::maximum(rect.y, y);
    width = mts::minimum(rect.width, width);
    height = mts::minimum(rect.height, height);
  }

  inline constexpr value_type get_area() const noexcept { return size.width * size.height; }

  template <typename T>
  inline constexpr rect get_union(const rect<T>& rhs) const noexcept {
    value_type nx = mts::minimum(x, rhs.x);
    value_type ny = mts::minimum(y, rhs.y);

    return { nx, ny, mts::maximum(right(), rhs.right()) - nx, mts::maximum(bottom(), rhs.bottom()) - ny };
  }

  template <typename T>
  inline constexpr rect& merge(const rect<T>& rhs) noexcept {
    value_type nx = mts::minimum(x, rhs.x);
    value_type ny = mts::minimum(y, rhs.y);
    return *this = { nx, ny, mts::maximum(right(), rhs.right()) - nx, mts::maximum(bottom(), rhs.bottom()) - ny };
  }

  template <typename T>
  inline constexpr rect merged(const rect<T>& rhs) const noexcept {
    value_type nx = mts::minimum(x, rhs.x);
    value_type ny = mts::minimum(y, rhs.y);
    return { nx, ny, (value_type)mts::maximum(right(), rhs.right()) - nx,
      (value_type)mts::maximum(bottom(), rhs.bottom()) - ny };
  }

  inline constexpr rect& round() noexcept {
    return *this = { mts::math::floor(x), mts::math::floor(y), mts::math::ceil(width), mts::math::ceil(height) };
  }

  inline constexpr rect rounded() const noexcept {
    return { mts::math::floor(x), mts::math::floor(y), mts::math::ceil(width), mts::math::ceil(height) };
  }

  template <typename T>
  inline constexpr rect get_intersection(const rect<T>& rhs) const noexcept {
    value_type nx = mts::maximum(x, rhs.x);
    value_type nw = mts::minimum(right(), rhs.right()) - nx;

    if (nw < 0) {
      return { 0, 0, 0, 0 };
    }

    value_type ny = mts::maximum(y, rhs.y);
    value_type nh = mts::minimum(bottom(), rhs.bottom()) - ny;

    if (nh < 0) {
      return { 0, 0, 0, 0 };
    }

    return { nx, ny, nw, nh };
  }

  /// Same as get_intersection but assuming the left hand rect has a position of {0, 0}.
  template <typename S, typename T>
  inline constexpr static rect get_intersection_from_size(const mts::size<S>& __size, const rect<T>& rhs) noexcept {
    value_type nw = mts::minimum(__size.width, rhs.right()) - rhs.x;
    value_type nh = mts::minimum(__size.height, rhs.bottom()) - rhs.y;
    return (nw < 0 || nh) ? rect{ 0, 0, 0, 0 } : rect{ rhs.x, rhs.y, nw, nh };
  }

  inline std::string to_string() const {
    return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " + std::to_string(height);
  }

  friend std::ostream& operator<<(std::ostream& stream, const rect& rect) {
    stream << rect.position.x << ", " << rect.position.y << ", " << rect.size.width << ", " << rect.size.height;
    return stream;
  }
};
MTS_END_NAMESPACE
