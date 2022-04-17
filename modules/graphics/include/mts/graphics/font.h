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
#include "mts/filesystem.h"
#include "mts/memory_range.h"
#include "mts/graphics/size.h"
#include "mts/graphics/path.h"
#include <string_view>

MTS_BEGIN_NAMESPACE
class font {
public:
  using native_font = void*;
  using glyph_index = int;
  static constexpr glyph_index invalid_glyph_index = -1;

  font() = default;
  font(const mts::filesystem::path& filepath, float font_size);
  font(mts::byte_view data, float font_size);
  font(const font& f);
  font(font&& f);

  ~font();

  font& operator=(const font& f);
  font& operator=(font&& f);

  float get_height() const;
  float get_font_size() const { return _font_size; }

  bool is_valid() const;

  font make_copy(float font_size) const;

  float get_string_width(std::string_view text) const;
  float get_string_width(std::u32string_view text) const;
  int get_char_index(std::u32string_view text, float x) const;

  void get_characters_positions(std::u32string_view text, std::vector<float>& offsets) const;
  void get_glyph_positions(std::string_view text, std::vector<glyph_index>& glyphs, std::vector<float>& offsets) const;

  path get_glyph_outline(glyph_index index) const;

  glyph_index get_glyph_index(std::string_view c) const;
  glyph_index get_glyph_index(char c) const;
  glyph_index get_glyph_index(char16_t c) const;
  glyph_index get_glyph_index(char32_t c) const;
  std::vector<glyph_index> get_glyph_indexes(std::string_view text) const;

  inline native_font get_native_font() { return _font; }
  inline const native_font get_native_font() const { return _font; }

private:
  native_font _font = nullptr;
  float _font_size = 0;
};
MTS_END_NAMESPACE
