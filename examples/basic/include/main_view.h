#pragma once
#include "mts/ui/view.h"
#include "mts/ui/controls/button.h"
#include "mts/ui/controls/styled_view.h"
#include "mts/ui/controls/popup_menu.h"
#include "mts/ui/controls/combo_box.h"

class main_view : public mts::view, public mts::view::listener {
public:
  main_view();

  virtual ~main_view() override;

protected:
  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
  virtual void mouse_left_down(const mts::mouse_event& evt) override;
  //  virtual void paint(mts::graphics::context& gc) override;
  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) override;

//  virtual void observe_value_for_key_path(
//      const mts::object* sender, const std::string& key_path, mts::property msg) override;

  virtual void view_being_deleted(mts::view& v) override;

    virtual void property_changed(const std::string& name) override;
private:
    mts::button _button_1;
    mts::styled_view _view_1;
    mts::view _view_2;
    mts::popup_menu _menu;
    mts::combo_box _box;
    mts::image _img;
};
