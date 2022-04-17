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
//#include "dragon-fruit/controls/scroll_view.h"
//
// DNFT_BEGIN_NAMESPACE
//
// class list_model : public _VDNFT::managed {
// public:
//  list_model() = default;
//  virtual ~list_model() = default;
//
//  virtual std::size_t row_count() const = 0;
//  virtual void draw_row(std::size_t index, _VDNFT::context& gc, const _VDNFT::size& size, bool is_selected) = 0;
//};
//
// class list_view : public _VDNFT::view {
// public:
//  static constexpr std::size_t invalid_index = std::numeric_limits<std::size_t>::max();
//
//  list_view();
//  virtual ~list_view() override;
//
//  void set_model(_VDNFT::managed_ptr<list_model>&& model);
//
//  std::size_t get_row_index_at(const _VDNFT::fpoint& pos) const;
//
//  inline std::size_t get_row_count_for_height(float h) const { return std::ceil(std::ceil(h) / _row_height); }
//
// protected:
//  virtual void mouse_left_down(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_up(const _VDNFT::mouse_event& evt) override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  _VDNFT::managed_ptr<list_model> _model;
//  int _row_height = 25;
//  std::size_t _selected_row = invalid_index;
//};
//
//// class scroll_list_view : public _VDNFT::scroll_view {
//// public:
////   scroll_list_view() {
////     components.add<_VDNFT::clipping>();
////   }
////
////   virtual ~scroll_list_view() override = default;
////
//// protected:
////   virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override {
////
////     _path = _VDNFT::path();
////     _path.add_rounded_rect(get_local_fbounds(), 5.0f);
////     components.get<_VDNFT::clipping>()->set_clipping_path(_path);
////     _VDNFT::scroll_view::bounds_changed(bs);
////
////   }
//// private:
////   _VDNFT::path _path;
//// };
//
// class scroll_list : public _VDNFT::view, private _VDNFT::view::listener {
// public:
//  scroll_list();
//
//  virtual ~scroll_list() override;
//
//  void set_model(_VDNFT::managed_ptr<list_model>&& model);
//
// protected:
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  _VDNFT::scroll_view _scroll_view;
//  _VDNFT::list_view _list;
//
//  virtual void view_bounds_changed(_VDNFT::view& v, const _VDNFT::view::bounds_status& bs) override;
//};
// DNFT_END_NAMESPACE
