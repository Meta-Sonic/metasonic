//#pragma once
//#include "mts/view.h"
//
// class settings_view : public mts::view {
// public:
//  settings_view();
//
//  virtual ~settings_view() override;
//
// protected:
//  virtual void mouse_left_down(const mts::mouse_event& evt) override;
//  virtual void mouse_left_drag(const mts::mouse_event& evt) override;
//  virtual void mouse_left_up(const mts::mouse_event& evt) override;
//
//  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
//  virtual void paint(mts::graphics::context& gc, const mts::frect& dirty_rect) override;
//
// private:
//  mts::label _label;
//  mts::label _cb_label;
//  mts::button _button;
//  mts::combo_box _combo_box;
//  mts::text_box _text_box;
//  mts::label _tb_label;
//  mts::path _bg_path;
//  mts::path _header_path;
//  mts::font _font;
//  mts::fpoint _click_pos = { 0, 0 };
//  mts::fpoint _start_pos = { 0, 0 };
//  bool _is_selected = false;
//};
