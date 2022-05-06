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
#include "mts/pointer.h"
#include "mts/graphics/rect.h"
#include "mts/graphics/quad.h"
#include "mts/graphics/color.h"
#include "mts/graphics/font.h"
#include "mts/graphics/path.h"
#include "mts/graphics/image.h"

#include <memory>
#include <string>

MTS_BEGIN_NAMESPACE

enum class text_alignment { left, center, right };

class context {
public:
  using native_context = void*;

  enum class line_join { miter, round, bevel };

  enum class line_cap { butt, round, square };

  context(mts::not_null<native_context> nc)
      : _native_context(nc) {}

  void save_state();
  void restore_state();
  void translate(const mts::point<float>& pos);

  void begin_transparent_layer(float alpha);
  void end_transparent_layer();

  void clip_to_rect(const mts::rect<float>& rect);
  void clip_to_path(const mts::path& p);
  mts::rect<float> get_clipping_rect();

  void set_line_width(float width);
  void set_line_join(line_join lj);
  void set_line_cap(line_cap lc);
  void set_line_style(float width, line_join lj, line_cap lc);

  void set_fill_color(const mts::color& c);
  void set_stroke_color(const mts::color& c);

  void fill_rect(const mts::rect<float>& r);
  void stroke_rect(const mts::rect<float>& r, float line_width = 1.0f);

  void fill_quad(const mts::quad<float>& q);
  void stroke_quad(const mts::quad<float>& q);

  void stroke_line(const mts::point<float>& p0, const mts::point<float>& p1);

  void fill_path(const mts::path& p);
  void fill_path(const mts::path& p, const mts::rect<float>& rect);
  void fill_path_with_shadow(
      const path& p, float blur, const mts::color& shadow_color, const mts::size<float>& offset = { 0, 0 });

  void stroke_path(const path& p);

  void fill_ellipse(const mts::rect<float>& r);
  void stroke_ellipse(const mts::rect<float>& r);

  void draw_text(mts::font& f, const std::string& text, const mts::point<float>& pos);
  void draw_text(mts::font& f, const std::string& text, const mts::rect<float>& rect, text_alignment alignment);
  void draw_text_left_aligned(mts::font& f, const std::string& text, const mts::rect<float>& rect);
  void draw_text_right_aligned(mts::font& f, const std::string& text, const mts::rect<float>& rect);
  void draw_text_centered(mts::font& f, const std::string& text, const mts::rect<float>& rect);

  void draw_text_left_aligned(font& f, const std::u32string& text, const mts::rect<float>& rect);

  void draw_image(const mts::image& img, const mts::point<float>& pos);
  void draw_image(const mts::image& img, const rect<float>& r);
  void draw_image(const mts::image& img, const rect<float>& r, const rect<float>& clip_rect);

  void set_shadow(float blur, const mts::color& shadow_color, const mts::size<float>& offset = { 0, 0 });

  inline native_context get_native_context() { return _native_context; }
  inline const native_context get_native_context() const { return _native_context; }

private:
  native_context _native_context;
};
MTS_END_NAMESPACE
