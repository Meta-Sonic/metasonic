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
//#include "dragon-fruit/controls/scroll_bar.h"
//
// DNFT_BEGIN_NAMESPACE
// class scroll_view : public _VDNFT::view, private _VDNFT::view::listener, private _VDNFT::scroll_bar::listener {
// public:
//  scroll_view();
//
//  virtual ~scroll_view() override {}
//
//  inline void set_content_view(_VDNFT::view& v) { set_content_view(_VDNFT::managed_ptr<_VDNFT::view>(&v, false)); }
//
//  void set_content_view(_VDNFT::managed_ptr<_VDNFT::view>&& v);
//
//  inline void set_content_view(std::unique_ptr<_VDNFT::view>&& v) {
//    set_content_view(_VDNFT::managed_ptr<_VDNFT::view>(std::move(v)));
//  }
//
//  inline void set_content_view(std::shared_ptr<_VDNFT::view> v) {
//    set_content_view(_VDNFT::managed_ptr<_VDNFT::view>(v));
//  }
//
//  void set_content_view_position(const _VDNFT::point& pos);
//
// protected:
//  virtual void on_scrolled(_VDNFT::scroll_bar& sb) override;
//  virtual void view_bounds_changed(_VDNFT::view& v, const _VDNFT::view::bounds_status& bs) override;
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void mouse_wheel(const _VDNFT::mouse_event& evt, const _VDNFT::scroll_event& sevt) override;
//  //  virtual void paint(_VDNFT::context& gc) override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  _VDNFT::scroll_bar _scroll_bar;
//  _VDNFT::managed_ptr<_VDNFT::view> _content_view;
//
//  inline float get_content_view_height_ratio() const noexcept {
//    return _VDNFT::minimum(get_height() / (float)_content_view->get_height(), 1.0f);
//  }
//
//  void update_content_view();
//};
// DNFT_END_NAMESPACE
