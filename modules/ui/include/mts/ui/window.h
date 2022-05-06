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
#include "mts/ui/view.h"

MTS_BEGIN_NAMESPACE

class window : public view {
public:
  enum class type { normal, floating, status, modal, popup };

  enum class style_flags {
    none = mts::get_flag_value(0),
    has_title_bar = mts::get_flag_value(1),
    is_resizable = mts::get_flag_value(2),
    has_minimise_button = mts::get_flag_value(3),
    has_maximise_button = mts::get_flag_value(4),
    has_close_button = mts::get_flag_value(5),
    has_drop_shadow = mts::get_flag_value(6),

    all = has_title_bar | is_resizable | has_minimise_button | has_maximise_button | has_close_button | has_drop_shadow
  };

  window();
  virtual ~window() override = default;

  void set_title(const std::string& title);
  inline const std::string& get_title() const noexcept { return _title; }

  void close();

  void set_frame_position(const mts::point<int>& position);

  mts::padding<int> get_frame_padding() const;
  mts::rect<int> get_frame_bounds() const;

  void grab_focus();

  void set_wants_unbounded_mouse_events(bool active);
  bool get_wants_unbounded_mouse_events() const;

  void center();

  void add_to_desktop(window::type __type = window::type::normal, style_flags __flags = style_flags::all);
  void add_to_desktop_centered(window::type __type = window::type::normal, style_flags __flags = style_flags::all);

protected:
  virtual mts::size<int> get_size_constrain(const mts::size<int>& frame_size) const { return frame_size; }
  virtual void frame_changed() {}

private:
  std::string _title;
  bool _is_closing = false;

  friend class native_view;
};

MTS_DECLARE_ENUM_CLASS_FLAGS(window::style_flags);

MTS_END_NAMESPACE
