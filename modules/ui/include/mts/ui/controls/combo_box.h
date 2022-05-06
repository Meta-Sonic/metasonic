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
#include "mts/ui/controls/popup_menu.h"

MTS_BEGIN_NAMESPACE
class combo_box : public mts::view {
public:
  combo_box();

  virtual ~combo_box() override;

  void set_text(const std::string& text);

  mts::popup_menu::item::uid add_item(const std::string& name);

  void set_selected_item_id(mts::popup_menu::item::uid item_id);
  void set_selected_item_index(std::size_t item_index);

protected:
  virtual void mouse_left_down(const mts::mouse_event& evt) override;
  virtual void attribute_changed(std::string_view group, std::string_view name) override;
  virtual void style_changed() override;
  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) override;

private:
  std::vector<std::string> _items;
  std::string _text;
  mts::popup_menu _menu;
  mts::font _font;
  mts::color _current_color;
  mts::color _current_text_color;
  mts::color _current_shadow_color;
  std::string _current_font_name;
  mts::path _bg_path;
  mts::path _arrow_path;
  mts::popup_menu::item::uid _selected_item_id = mts::popup_menu::item::invalid_id;

  void callback(mts::popup_menu::item::uid item_id);
  void set_selected_item(const mts::popup_menu::item* item);
  void update_path();
};
MTS_END_NAMESPACE
