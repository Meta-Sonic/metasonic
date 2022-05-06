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
#include "mts/util.h"
#include <algorithm>
#include <cctype>
#include <string>

MTS_BEGIN_NAMESPACE

inline bool is_space(char ch) { return std::isspace(static_cast<unsigned char>(ch)); }

inline bool is_digit(char ch) { return std::isdigit(static_cast<unsigned char>(ch)); }

inline bool is_xdigit(char ch) { return std::isxdigit(static_cast<unsigned char>(ch)); }

inline bool is_alpha(char ch) { return std::isalpha(static_cast<unsigned char>(ch)); }

std::string char_to_string(char c) { return std::string(1, c); }

std::string to_upper(const std::string& istring) {
  std::string str = istring;
  for (auto& c : str) {
    c = std::toupper(c);
  }
  return str;
}

std::string to_lower(const std::string& istring) {
  std::string str = istring;
  for (auto& c : str) {
    c = std::tolower(c);
  }
  return str;
}

void replace(std::string& str, const std::string& from, const std::string& to) {
  if (from.empty()) {
    return;
  }

  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);

    // In case 'to' contains 'from', like replacing 'x' with 'yx'.
    start_pos += to.length();
  }
}

std::string ltrim(const std::string& s) {
  std::string::size_type pos = s.find_first_not_of(" \n\t\r\v");
  return pos == std::string::npos ? std::string() : s.substr(pos);
}

inline std::string_view ltrim(std::string_view s) {
  std::string_view::size_type pos = s.find_first_not_of(" \n\t\r\v");
  return pos == std::string_view::npos ? std::string_view() : s.substr(pos);
}

std::string rtrim(const std::string& s) {
  std::string::size_type pos = s.find_last_not_of(" \n\t\r\v"); /// must succeed
  return pos == std::string::npos ? std::string() : s.substr(0, pos + 1);
}

std::string_view rtrim(std::string_view s) {
  std::string_view::size_type pos = s.find_last_not_of(" \n\t\r\v"); /// must succeed
  return pos == std::string_view::npos ? std::string_view() : s.substr(0, pos + 1);
}

std::string trim(const std::string& s) { return rtrim(ltrim(s)); }

std::string_view trim(std::string_view s) { return rtrim(ltrim(s)); }

MTS_END_NAMESPACE
