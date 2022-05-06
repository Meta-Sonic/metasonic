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
#include "mts/ui/view.h"

MTS_BEGIN_NAMESPACE
class button : public view {
public:
  button();
  button(const std::string& name);

  virtual ~button() override {}

  std::function<void()> on_click;

  inline void set_click_on_mouse_up(bool value) { _click_on_mouse_up = value; }
  inline bool get_click_on_mouse_up() const { return _click_on_mouse_up; }

  inline void set_text(const std::string& text) {
    if (mts::assign(_text, text)) {
      repaint();
    }
  }

protected:
  virtual void mouse_left_down(const mts::mouse_event& evt) override;
  virtual void mouse_left_up(const mts::mouse_event& evt) override;
  virtual void mouse_enter(const mts::mouse_event& evt) override;
  virtual void mouse_leave() override;
  virtual void attribute_changed(std::string_view group, std::string_view name) override;
  virtual void style_changed() override;
  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) override;

private:
  std::string _text;
  mts::color _current_color;
  mts::color _current_text_color;
  mts::font _font;
  mts::path _bg_path;
  std::string _current_font_name;
  bool _is_down = false;
  bool _click_on_mouse_up = false;

  void update_path();
};
MTS_END_NAMESPACE
