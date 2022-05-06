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
//#include "dragon-fruit/gui/popup_window.h"
//
// DNFT_BEGIN_NAMESPACE
// class popup_value_content : public _VDNFT::view {
// public:
//  popup_value_content();
//  virtual ~popup_value_content() override;
//
//  void set_text(const std::string& text);
//
// protected:
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  std::string _text;
//  _VDNFT::font _font;
//  _VDNFT::path _path;
//};
//
// class popup_value {
// public:
//  void show(_VDNFT::view& parent, const _VDNFT::rect& bounds, const std::string& text) {
//    _VDNFT::managed_ptr<_VDNFT::popup_value_content> content = dnft::make_managed<_VDNFT::popup_value_content>();
//    content->set_size(bounds.size);
//    content->set_text(text);
//
//    _VDNFT::managed_ptr<_VDNFT::popup_window> pwin
//        = _VDNFT::popup_window::create(parent, std::move(content), bounds.position);
//    _window = pwin;
//  }
//
//  void hide() {
//    if (_window) {
//      _window->close();
//      _window.reset();
//    }
//  }
//
//  void set_text(const std::string& text) {
//    if (_window) {
//      if (_VDNFT::weak_managed_ptr<_VDNFT::popup_value_content> pvalue = _window->get_weak_content()) {
//        pvalue->set_text(text);
//      }
//    }
//  }
//
// private:
//  _VDNFT::weak_managed_ptr<_VDNFT::popup_window> _window;
//};
// DNFT_END_NAMESPACE
