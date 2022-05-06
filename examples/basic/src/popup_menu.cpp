//#include "popup_menu.h"
//
// popup_menu::popup_menu(mts::view& __view)
//    : _view(__view) {
//
//  if (mts::view* p = _view.get_root_parent()) {
//    p->listeners.add(this);
//  }
//
//  listeners.add(this);
//}
//
// popup_menu::~popup_menu() {
//  mts::print("popup_menu::~popup_menu");
//  if (mts::view* p = _view.get_root_parent()) {
//    p->listeners.remove(this);
//  }
//
//  listeners.remove(this);
//}
//
// void popup_menu::view_visibility_changed(mts::view& v) {
//  mts::print("popup_menu::view_visibility_changed");
//  if (!v.is_visible() && &v != this) {
//    //    hide();
//    //    mts::desktop::get_modal_manager().remove(this);
//    close();
//  }
//}
//
// void popup_menu::view_focus_changed(mts::view& v, bool focused) {
//  mts::print("popup_menu::view_focus_changed");
//
//  if ((!focused && &v == this) || (focused && &v != this)) {
//    //    hide();
//    //    mts::desktop::get_modal_manager().remove(this);
//    close();
//  }
//}
//
// void popup_menu::paint(mts::graphics::context& gc, const mts::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//  gc.set_fill_color(0xFF0000FF);
//  gc.fill_rect(get_local_bounds());
//}
