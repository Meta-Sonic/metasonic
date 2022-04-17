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
#include "mts/range.h"
#include "mts/graphics/context.h"
#include "mts/graphics/alignment.h"

#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

MTS_BEGIN_NAMESPACE
namespace tl {
// Top - The maximum distance above the baseline for the tallest glyph in the font at a given text size.
// Ascent - The recommended distance above the baseline for singled spaced text.
// Descent - The recommended distance below the baseline for singled spaced text.
// Bottom - The maximum distance below the baseline for the lowest glyph in the font at a given text size.
// Leading - The recommended additional space to add between lines of text.

class line {
public:
  using native_line = void*;
  static constexpr std::size_t invalid_index = std::numeric_limits<std::size_t>::max();

  line();
  line(const line& l);
  line(line&& l);
  line(const mts::font& f, const std::u32string& text = std::u32string());
  line(const mts::font& f, std::u32string&& text);
  line(const mts::font& f, std::u32string_view text = std::u32string_view());
  line(const mts::font& f, std::string_view text = std::string_view());

  ~line();

  line& operator=(const line& l);
  line& operator=(line&& l);

  void set_font(const mts::font& f);
  const mts::font& get_font() const noexcept { return _font; }

  void set_text(const std::u32string& text, bool replace_tabs = false);
  void set_text(std::u32string_view text, bool replace_tabs = false);
  void set_text(std::u16string_view text, bool replace_tabs = false);
  void set_text(std::string_view text, bool replace_tabs = false);

  std::string get_text() const;
  std::string get_text_utf8() const;
  std::u16string get_text_utf16() const;
  inline const std::u32string& get_text_utf32() const noexcept { return _text; }

  void set_password(bool is_password);
  inline bool is_password() const noexcept { return _is_password; }

  template <typename _Fct>
  inline void modify_text(_Fct op, bool do_update = true) {
    op(_text);
    if (do_update) {
      update();
    }
  }

  mts::rect<float> get_bounds() const;
  mts::size<float> get_size() const;
  float get_width() const;

  mts::point<float> get_aligned_position(
      const mts::rect<float>& rect, mts::alignment alignment = mts::alignment::centered()) const;

  mts::rect<float> get_aligned_bounds(
      const mts::rect<float>& rect, mts::alignment alignment = mts::alignment::centered()) const;

  std::size_t get_char_index_at_position(const mts::point<float>& pos) const;

  std::size_t get_char_index_at_position(const mts::point<float>& pos, const mts::rect<float>& rect,
      mts::alignment alignment = mts::alignment::centered()) const;

  float get_offset_for_char_index(std::size_t index) const;

  mts::range<float> get_offset_for_char_index(mts::range<std::size_t> indexes) const;

  void draw(mts::context& gc, const mts::point<float>& pos);

  void draw(mts::context& gc, const mts::rect<float>& rect, mts::alignment alignment = mts::alignment::centered());

  inline bool is_valid() const noexcept { return _line; }

  inline native_line get_native_line() { return _line; }
  inline const native_line get_native_line() const { return _line; }

  inline void set_change_callback(std::function<void()> fct) { _change_callback = fct; }

private:
  mts::font _font;
  std::u32string _text;
  native_line _line = nullptr;
  std::function<void()> _change_callback;
  bool _is_password = false;

  class native_line_impl;
  friend class native_line_impl;

  void update();
};
} // namespace tl.
MTS_END_NAMESPACE
