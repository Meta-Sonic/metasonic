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
#include "mts/graphics/rect.h"
#include "mts/ui/key_modifiers.h"

MTS_BEGIN_NAMESPACE
class mouse_event {
public:
  enum class source_type : std::uint8_t { mouse, touch, pen };

  struct common {
    source_type stype;
    key_modifiers modifiers;
    _VMTS::point<float> absolute_position;
    int click_count;
  };

  mouse_event(source_type __source_type, const _VMTS::point<float>& absolute_pos,
      const _VMTS::point<float>& relative_pos, key_modifiers mods, int click_count = 1)
      : _absolute_position(absolute_pos)
      , _relative_pos(relative_pos)
      , _mods(mods)
      , _click_count(click_count)
      , _source_type(__source_type) {}

  mouse_event(const common& c, const _VMTS::point<float>& relative_pos)
      : _absolute_position(c.absolute_position)
      , _relative_pos(relative_pos)
      , _mods(c.modifiers)
      , _click_count(c.click_count)
      , _source_type(c.stype) {}

  inline const _VMTS::point<float>& get_position() const noexcept { return _relative_pos; }
  inline const _VMTS::point<float>& get_absolute_position() const noexcept { return _absolute_position; }

  inline int get_click_count() const noexcept { return _click_count; }

  inline source_type get_source_type() const noexcept { return _source_type; }

  inline key_modifiers get_modifiers() const noexcept { return _mods; }

  inline bool is_control_down() const noexcept { return _mods.is_control_down(); }
  inline bool is_command_down() const noexcept { return _mods.is_command_down(); }
  inline bool is_option_down() const noexcept { return _mods.is_option_down(); }
  inline bool is_shift_down() const noexcept { return _mods.is_shift_down(); }
  inline bool is_alt_down() const noexcept { return _mods.is_alt_down(); }

  inline bool is_left_button_down() const noexcept { return _mods.is_left_button_down(); }
  inline bool is_middle_button_down() const noexcept { return _mods.is_middle_button_down(); }
  inline bool is_right_button_down() const noexcept { return _mods.is_right_button_down(); }

  inline bool is_any_mouse_button_down() const noexcept { return _mods.is_any_mouse_button_down(); }
  inline bool is_any_mod_key_down() const noexcept { return _mods.is_any_mod_key_down(); }
  inline int get_button_down_count() const noexcept { return _mods.get_button_down_count(); }

private:
  _VMTS::point<float> _absolute_position;
  _VMTS::point<float> _relative_pos;
  key_modifiers _mods;
  int _click_count = 1;
  source_type _source_type = source_type::mouse;
};

class scroll_event {
public:
  scroll_event(const _VMTS::point<float>& delta, bool reversed)
      : _delta(delta)
      , _is_reversed(reversed) {}

  inline const _VMTS::point<float>& get_delta() const noexcept { return _delta; }
  inline bool is_reversed() const noexcept { return _is_reversed; }

private:
  _VMTS::point<float> _delta;
  bool _is_reversed;
};

MTS_END_NAMESPACE
