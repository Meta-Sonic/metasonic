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
//#include "dragon-fruit/core/flags.h"
//#include "dragon-fruit/core/time.h"
#include <algorithm>
#include <string_view>

#include "mts/detail/utf8/checked.h"
#include "mts/detail/utf8/unchecked.h"

MTS_BEGIN_SUB_NAMESPACE(utf8)
inline char32_t convert_first_utf8_char_to_32(std::string_view s) {
  char32_t value = 0;
  unsigned int codepoint;

  for (std::size_t i = 0; i < s.size(); i++) {
    unsigned char ch = static_cast<unsigned char>(s[i]);
    if (ch <= 0x7f) {
      codepoint = ch;
    }
    else if (ch <= 0xbf) {
      codepoint = (codepoint << 6) | (ch & 0x3f);
    }
    else if (ch <= 0xdf) {
      codepoint = ch & 0x1f;
    }
    else if (ch <= 0xef) {
      codepoint = ch & 0x0f;
    }
    else {
      codepoint = ch & 0x07;
    }

    unsigned int next_char = ((i + 1) >= s.size()) ? 0 : s[i + 1];
    if (((next_char & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
      value = static_cast<char32_t>(codepoint);
    }
  }

  return value;
}
MTS_END_SUB_NAMESPACE(utf8)
