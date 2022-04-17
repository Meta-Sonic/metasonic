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

MTS_BEGIN_NAMESPACE
class alignment {
public:
  enum class horizontal : std::uint8_t { left, center, right };
  enum class vertical : std::uint8_t { top, center, bottom };

  enum class justification {
    left_top,
    left_center,
    left_bottom,
    center_top,
    center_center,
    center_bottom,
    right_top,
    right_center,
    right_bottom
  };

  inline alignment(horizontal h = horizontal::center, vertical v = vertical::center) {
    _data.data = { (std::uint8_t)h, (std::uint8_t)v };
  }

  inline alignment(justification j) {
    switch (j) {
    case justification::left_top: {
      _data.data = { 0, 0 };
    } break;
    case justification::left_center: {
      _data.data = { 0, 1 };
    } break;
    case justification::left_bottom: {
      _data.data = { 0, 2 };
    } break;
    case justification::center_top: {
      _data.data = { 1, 0 };
    } break;
    case justification::center_center: {
      _data.data = { 1, 1 };
    } break;
    case justification::center_bottom: {
      _data.data = { 1, 2 };
    } break;
    case justification::right_top: {
      _data.data = { 2, 0 };
    } break;
    case justification::right_center: {
      _data.data = { 2, 1 };
    } break;
    case justification::right_bottom: {
      _data.data = { 2, 2 };
    } break;
    }
  }

  inline horizontal get_horizontal() const noexcept { return (horizontal)_data.data.horizontal; }
  inline vertical get_vertical() const noexcept { return (vertical)_data.data.vertical; }

  inline justification get_justification() const noexcept {
    std::uint8_t h = _data.data.horizontal;
    std::uint8_t v = _data.data.vertical;
    return (justification)(h * 3 + v);
  }

  inline bool operator==(alignment algnt) const noexcept { return _data.byte == algnt._data.byte; }

  static inline alignment centered() noexcept { return alignment(); }
  static inline alignment left_centered() noexcept { return alignment(horizontal::left); }
  static inline alignment right_centered() noexcept { return alignment(horizontal::right); }

private:
  union {
    std::uint8_t byte;
    struct {
      std::uint8_t horizontal : 4;
      std::uint8_t vertical : 4;
    } data;
  } _data;
};

MTS_END_NAMESPACE
