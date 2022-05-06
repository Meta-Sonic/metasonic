/////
///// Copyright (C) 2021, Meta-Sonic
///// All rights reserved.
/////
///// Proprietary and confidential.
///// Any unauthorized copying, alteration, distribution, transmission,
///// performance, display or other use of this material is strictly prohibited.
/////
///// Written by Alexandre Arsenault <alx.arsenault@gmail.com>
/////
//
//#pragma once
//#include "dragon-fruit/gui/view.h"
//#include "dragon-fruit/gui/animator.h"
//
// DNFT_BEGIN_NAMESPACE
//
// class scroll_bar : public _VDNFT::view {
// public:
//  class listener {
//  public:
//    virtual ~listener() = default;
//    virtual void on_scrolled(scroll_bar& sb) = 0;
//  };
//
//  scroll_bar();
//
//  void set_value(float v, bool notify_listeners = true);
//  void set_scroll_ratio(float ratio);
//  void scroll(float dy);
//
//  inline bool is_scrollable() const noexcept { return _h_ratio != 1; }
//
//  inline float get_value() const noexcept { return _h_value; }
//  inline float get_scroll_ratio() const noexcept { return _h_ratio; }
//
//  inline void add_listener(_VDNFT::not_null<listener*> l) { _listeners.add(l); }
//
//  inline void remove_listener(_VDNFT::not_null<listener*> l) { _listeners.remove(l); }
//
// protected:
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void mouse_left_down(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_drag(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_up(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_moved(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_enter(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_leave() override;
//  virtual void mouse_wheel(const mouse_event& evt, const _VDNFT::scroll_event& sevt) override;
//  virtual void visibily_changed() override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  _VDNFT::frect _bar_rect;
//  _VDNFT::path _bar_path;
//  _VDNFT::path _bar_path_enlarge;
//  float _h_ratio = 0.3f;
//  float _h_value = 0.3f;
//
//  _VDNFT::color _current_bg_color;
//  _VDNFT::color _bg_color = 0xEEEEEE66;
//  _VDNFT::color _hover_bg_color = 0x88888811;
//  _VDNFT::color _bar_color;
//  _VDNFT::color _current_bar_color;
//  _VDNFT::color _bar_selected_color;
//  float _click_delta = 0;
//  bool _is_dragging = false;
//  bool _is_selected = false;
//  bool _is_enlarge = false;
//
//  _VDNFT::listener_manager<listener> _listeners;
//  _VDNFT::differed_fade_animator _diff_fade;
//
//  void enlarge();
//
//  inline void set_bar_rect(const _VDNFT::frect& r) {
//    _bar_rect = r;
//    _bar_path = _VDNFT::path();
//    _bar_path.add_rounded_rect(_bar_rect, 9.0f);
//
//    _bar_path_enlarge = _VDNFT::path();
//    _bar_path_enlarge.add_rounded_rect(_bar_rect.reduced({ 1.0f, 0.0f }).with_width(_bar_rect.width - 1), 9.0f);
//  }
//
//  inline void repaint_bar() { repaint(_bar_rect.rounded().expanded(2.0f)); }
//
//  inline void repaint_bar(const _VDNFT::frect& r) { repaint(r.merged(_bar_rect).rounded().expanded(2.0f)); }
//};
// DNFT_END_NAMESPACE
