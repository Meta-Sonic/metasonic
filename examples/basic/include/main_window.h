#pragma once
#include "mts/ui/window.h"
#include "main_view.h"

class main_window : public mts::window {
public:
  main_window();

  virtual ~main_window() override;

protected:
  virtual void mouse_left_down(const mts::mouse_event& evt) override;
  virtual void mouse_moved(const mts::mouse_event& evt) override;
  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
  virtual void frame_changed() override;
  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) override;
  virtual void visibily_changed() override;

  //    virtual _VDNFT::size get_size_constrain(const _VDNFT::size& frame_size) const override {
  //      return {std::clamp(frame_size.width, 200, 500), mts::maximum(frame_size.height, 200)};
  //  //    return {mts::maximum(frame_size.width, 200), mts::maximum(frame_size.height, 200)};
  //    }

private:
  mts::color _color = 0x0000FFFF;
  main_view _content;
//  mts::view _view;
};
