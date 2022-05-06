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
#include "mts/memory_range.h"
#include "mts/graphics/padding.h"
#include "mts/graphics/rect.h"

MTS_BEGIN_NAMESPACE
class font;

class path {
public:
  using native_path = void*;

  path();
  path(const path& p);
  path(path&& p);
  ~path();
  path& operator=(const path& p);
  path& operator=(path&& p);

  bool operator==(const path& p) const;
  bool operator!=(const path& p) const;

  bool is_empty() const;
  inline explicit operator bool() const { return !is_empty(); }

  inline native_path get_native_path() { return _native_path; }

  inline const native_path get_native_path() const { return _native_path; }

  mts::point<float> get_current_point() const;

  /// The bounding box is the smallest rectangle completely enclosing all points in the path,
  /// including control points for Bézier and quadratic curves.
  mts::rect<float> get_bounding_box() const;

  /// The path bounding box is the smallest rectangle completely enclosing all points in the path but
  /// not including control points for Bézier and quadratic curves.
  mts::rect<float> get_path_bounding_box() const;

  bool contains(const mts::point<float>& p) const;

  void start_subpath(const mts::point<float>& p);
  void close_subpath();

  /// Append a straight line segment from the current point to 'p' and move the current point to 'p'.
  void line_to(const mts::point<float>& p);

  /// Append a quadratic curve from the current point to 'p' with control point 'c' and move the current point to 'p'.
  void quadratic_to(const mts::point<float>& c, const mts::point<float>& p);

  /// Append a cubic Bézier curve from the current point to 'p' with control points 'c1' and 'c2' and move the current
  /// point to 'p'.
  void curve_to(const mts::point<float>& c1, const mts::point<float>& c2, const mts::point<float>& p);

  void add_line(const mts::point<float>& p0, const mts::point<float>& p1);
  void add_rect(const mts::rect<float>& r);
  void add_rounded_rect(const mts::rect<float>& r, float radius);
  void add_rounded_rect(const mts::rect<float>& r, const mts::size<float>& radius, bool top_left, bool top_right,
      bool bottom_left, bool bottom_right);

  void add_ellipse(const mts::rect<float>& r);
  void add_relative_arc(const mts::point<float>& center, float radius, float start_angle, float angle_delta);

  bool load_data(mts::byte_view data);

  path get_normalized_path() const;

  static path create_rounded_rect(const mts::rect<float>& r, float radius);

  static path create_down_arrow();
  static path create_check_mark();

private:
  friend class font;

  path(native_path np);
  native_path _native_path;
};
MTS_END_NAMESPACE
