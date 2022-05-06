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
#include "mts/flags.h"

MTS_BEGIN_NAMESPACE

class key_modifiers {
public:
  enum flags : std::uint32_t {
    none = _VMTS::get_flag_value(0),
    shift = _VMTS::get_flag_value(1),
    ctrl = _VMTS::get_flag_value(2),
    option = _VMTS::get_flag_value(3),
    command = _VMTS::get_flag_value(4),
    alt = option,
    left_mouse_button = _VMTS::get_flag_value(5),
    middle_mouse_button = _VMTS::get_flag_value(6),
    right_mouse_button = _VMTS::get_flag_value(7),
  };

  key_modifiers() noexcept = default;
  key_modifiers(const key_modifiers&) noexcept = default;
  key_modifiers(key_modifiers&&) noexcept = default;

  key_modifiers(flags f) noexcept
      : _flags(f) {}

  ~key_modifiers() noexcept = default;
  key_modifiers& operator=(const key_modifiers&) noexcept = default;
  key_modifiers& operator=(key_modifiers&&) noexcept = default;

  inline bool is_control_down() const noexcept { return _VMTS::has_flag(flags::ctrl, _flags); }
  inline bool is_command_down() const noexcept { return _VMTS::has_flag(flags::command, _flags); }
  inline bool is_option_down() const noexcept { return _VMTS::has_flag(flags::option, _flags); }
  inline bool is_shift_down() const noexcept { return _VMTS::has_flag(flags::shift, _flags); }
  inline bool is_alt_down() const noexcept { return _VMTS::has_flag(flags::alt, _flags); }

  inline bool is_left_button_down() const noexcept { return _VMTS::has_flag(flags::left_mouse_button, _flags); }
  inline bool is_middle_button_down() const noexcept { return _VMTS::has_flag(flags::middle_mouse_button, _flags); }
  inline bool is_right_button_down() const noexcept { return _VMTS::has_flag(flags::right_mouse_button, _flags); }

  inline bool is_any_mouse_button_down() const noexcept {
    return _VMTS::has_flag(
        (flags)(flags::left_mouse_button | flags::middle_mouse_button | flags::right_mouse_button), _flags);
  }

  inline bool is_any_mod_key_down() const noexcept {
    return _VMTS::has_flag((flags)(flags::shift | flags::ctrl | flags::alt | flags::command | flags::option), _flags);
  }

  inline int get_button_down_count() const noexcept {
    return int(is_left_button_down()) + int(is_middle_button_down()) + int(is_right_button_down());
  }

  inline flags get_flags() const noexcept { return _flags; }
  inline bool has(flags flags_to_verity) const noexcept { return _VMTS::has_flag(flags_to_verity, _flags); }

  inline key_modifiers without_flags(flags flags_to_remove) const noexcept {
    return key_modifiers((flags)(_flags & ~flags_to_remove));
  }

  inline key_modifiers with_flags(flags flags_to_set) const noexcept {
    return key_modifiers((flags)(_flags | flags_to_set));
  }

  inline key_modifiers without_flags(key_modifiers flags_to_remove) const noexcept {
    return key_modifiers((flags)(_flags & ~flags_to_remove._flags));
  }

  inline key_modifiers with_flags(key_modifiers flags_to_set) const noexcept {
    return key_modifiers((flags)(_flags | flags_to_set._flags));
  }

  inline bool operator==(key_modifiers rhs) const noexcept { return _flags == rhs._flags; }
  inline bool operator!=(key_modifiers rhs) const noexcept { return _flags != rhs._flags; }

private:
  flags _flags = flags::none;
};
MTS_END_NAMESPACE
