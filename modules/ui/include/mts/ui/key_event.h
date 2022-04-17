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
#include "mts/utf8.h"
#include "mts/util.h"
#include "mts/ui/key_modifiers.h"
#include <string_view>

MTS_BEGIN_NAMESPACE

class key_event {
public:
  using key_code = int;

  enum class arrow_direction : std::uint8_t { none, left, right, up, down };

  key_event() = default;

  inline key_event(key_code code, key_modifiers mods, char32_t character)
      : _char(character)
      , _key_code(code)
      , _mods(mods) {}

  key_event(const key_event&) = default;
  key_event(key_event&&) = default;

  ~key_event() = default;

  key_event& operator=(const key_event&) = default;
  key_event& operator=(key_event&&) = default;

  inline char32_t get_char() const { return _char; }
  inline std::string get_string() const { return _VMTS::utf8::utf32to8(std::u32string_view(&_char, 1)); }

  inline key_code get_key_code() const noexcept { return _key_code; }

  //  inline bool is_any_modifiers() const noexcept {
  //    return _VMTS::has_flag(
  //        (modifier)(modifier::shift | modifier::ctrl | modifier::alt | modifier::command | modifier::option), _mods);
  //  }

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

  inline bool is_space() const noexcept { return _key_code == spaceKey; }
  inline bool is_escape() const noexcept { return _key_code == escapeKey; }
  inline bool is_return() const noexcept { return _key_code == returnKey; }
  inline bool is_tab() const noexcept { return _key_code == tabKey; }
  inline bool is_delete() const noexcept { return _key_code == deleteKey; }
  inline bool is_arrow() const noexcept { return _VMTS::is_one_of(_key_code, leftKey, rightKey, upKey, downKey); }
  inline bool is_left_arrow() const noexcept { return _key_code == leftKey; }
  inline bool is_right_arrow() const noexcept { return _key_code == rightKey; }
  inline bool is_up_arrow() const noexcept { return _key_code == upKey; }
  inline bool is_down_arrow() const noexcept { return _key_code == downKey; }

  inline arrow_direction get_arrow_direction() const noexcept {
    if (_key_code == leftKey) {
      return arrow_direction::left;
    }
    else if (_key_code == rightKey) {
      return arrow_direction::right;
    }
    else if (_key_code == upKey) {
      return arrow_direction::up;
    }
    else if (_key_code == downKey) {
      return arrow_direction::down;
    }

    return arrow_direction::none;
  }

  static const key_code spaceKey; /**< key-code for the space bar */
  static const key_code escapeKey; /**< key-code for the escape key */
  static const key_code returnKey; /**< key-code for the return key*/
  static const key_code tabKey; /**< key-code for the tab key*/

  static const key_code deleteKey; /**< key-code for the delete key (not backspace) */
  static const key_code backspaceKey; /**< key-code for the backspace key */
  static const key_code insertKey; /**< key-code for the insert key */

  static const key_code upKey; /**< key-code for the cursor-up key */
  static const key_code downKey; /**< key-code for the cursor-down key */
  static const key_code leftKey; /**< key-code for the cursor-left key */
  static const key_code rightKey; /**< key-code for the cursor-right key */
  static const key_code pageUpKey; /**< key-code for the page-up key */
  static const key_code pageDownKey; /**< key-code for the page-down key */
  static const key_code homeKey; /**< key-code for the home key */
  static const key_code endKey; /**< key-code for the end key */

  static const key_code F1Key; /**< key-code for the F1 key */
  static const key_code F2Key; /**< key-code for the F2 key */
  static const key_code F3Key; /**< key-code for the F3 key */
  static const key_code F4Key; /**< key-code for the F4 key */
  static const key_code F5Key; /**< key-code for the F5 key */
  static const key_code F6Key; /**< key-code for the F6 key */
  static const key_code F7Key; /**< key-code for the F7 key */
  static const key_code F8Key; /**< key-code for the F8 key */
  static const key_code F9Key; /**< key-code for the F9 key */
  static const key_code F10Key; /**< key-code for the F10 key */
  static const key_code F11Key; /**< key-code for the F11 key */
  static const key_code F12Key; /**< key-code for the F12 key */
  static const key_code F13Key; /**< key-code for the F13 key */
  static const key_code F14Key; /**< key-code for the F14 key */
  static const key_code F15Key; /**< key-code for the F15 key */
  static const key_code F16Key; /**< key-code for the F16 key */
  static const key_code F17Key; /**< key-code for the F17 key */
  static const key_code F18Key; /**< key-code for the F18 key */
  static const key_code F19Key; /**< key-code for the F19 key */
  static const key_code F20Key; /**< key-code for the F20 key */
  static const key_code F21Key; /**< key-code for the F21 key */
  static const key_code F22Key; /**< key-code for the F22 key */
  static const key_code F23Key; /**< key-code for the F23 key */
  static const key_code F24Key; /**< key-code for the F24 key */
  static const key_code F25Key; /**< key-code for the F25 key */
  static const key_code F26Key; /**< key-code for the F26 key */
  static const key_code F27Key; /**< key-code for the F27 key */
  static const key_code F28Key; /**< key-code for the F28 key */
  static const key_code F29Key; /**< key-code for the F29 key */
  static const key_code F30Key; /**< key-code for the F30 key */
  static const key_code F31Key; /**< key-code for the F31 key */
  static const key_code F32Key; /**< key-code for the F32 key */
  static const key_code F33Key; /**< key-code for the F33 key */
  static const key_code F34Key; /**< key-code for the F34 key */
  static const key_code F35Key; /**< key-code for the F35 key */

  static const key_code numberPad0; /**< key-code for the 0 on the numeric keypad. */
  static const key_code numberPad1; /**< key-code for the 1 on the numeric keypad. */
  static const key_code numberPad2; /**< key-code for the 2 on the numeric keypad. */
  static const key_code numberPad3; /**< key-code for the 3 on the numeric keypad. */
  static const key_code numberPad4; /**< key-code for the 4 on the numeric keypad. */
  static const key_code numberPad5; /**< key-code for the 5 on the numeric keypad. */
  static const key_code numberPad6; /**< key-code for the 6 on the numeric keypad. */
  static const key_code numberPad7; /**< key-code for the 7 on the numeric keypad. */
  static const key_code numberPad8; /**< key-code for the 8 on the numeric keypad. */
  static const key_code numberPad9; /**< key-code for the 9 on the numeric keypad. */

  static const key_code numberPadAdd; /**< key-code for the add sign on the numeric keypad. */
  static const key_code numberPadSubtract; /**< key-code for the subtract sign on the numeric keypad. */
  static const key_code numberPadMultiply; /**< key-code for the multiply sign on the numeric keypad. */
  static const key_code numberPadDivide; /**< key-code for the divide sign on the numeric keypad. */
  static const key_code numberPadSeparator; /**< key-code for the comma on the numeric keypad. */
  static const key_code numberPadDecimalPoint; /**< key-code for the decimal point sign on the numeric keypad. */
  static const key_code numberPadEquals; /**< key-code for the equals key on the numeric keypad. */
  static const key_code numberPadDelete; /**< key-code for the delete key on the numeric keypad. */

  static const key_code playKey; /**< key-code for a multimedia 'play' key, (not all keyboards will have one) */
  static const key_code stopKey; /**< key-code for a multimedia 'stop' key, (not all keyboards will have one) */
  static const key_code fastForwardKey; /**< key-code for a multimedia 'fast-forward' key */
  static const key_code rewindKey; /**< key-code for a multimedia 'rewind' key */

private:
  char32_t _char;
  key_code _key_code = 0;
  key_modifiers _mods;
};
MTS_END_NAMESPACE
