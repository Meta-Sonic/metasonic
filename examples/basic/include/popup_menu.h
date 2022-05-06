//#pragma once
//#include "mts/view.h"
//
// class popup_menu : public mts::window, private mts::view::listener {
// public:
//  popup_menu(mts::view& __view);
//
//  virtual ~popup_menu() override;
//
// protected:
//  //  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
//  virtual void view_visibility_changed(mts::view& v) override;
//  virtual void view_focus_changed(mts::view& v, bool focused) override;
//  virtual void paint(mts::graphics::context& gc, const mts::frect& dirty_rect) override;
//
//  //  virtual _VDNFT::size get_size_constrain(const _VDNFT::size& frame_size) const override {
//  //    return {std::clamp(frame_size.width, 200, 500), mts::maximum(frame_size.height, 200)};
//  ////    return {mts::maximum(frame_size.width, 200), mts::maximum(frame_size.height, 200)};
//  //  }
//
// private:
//  mts::view& _view;
//};
