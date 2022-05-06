#include "settings_view.h"
#include "mts/print.h"
#include "mts/ui/desktop.h"
#include "mts/ui/native_view.h"

settings_view::settings_view() {}

settings_view::~settings_view() { mts::print("settings_view::~settings_view"); }

void settings_view::bounds_changed(const mts::view::bounds_status& bs) {}

void settings_view::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {

  gc.set_fill_color(get_attribute<mts::style::global::background_color>());
  gc.fill_rect(dirty_rect);
}
// void settings_view::view_visibility_changed(mts::view& v) {
//   if (&v == _content.get()) {
//     if (!v.is_visible()) {
//       close();
//     }
//   }
//   else if (!v.is_visible() && &v != this) {
//     close();
//   }
// }
void settings_view::show(_VMTS::view& parent, const _VMTS::point<int>& position) {
  _VMTS::managed_ptr<settings_view> content = _VMTS::make_managed<settings_view>();
  //
  content->set_size({ 300, 300 });
  settings_view* ptr = content.get();

  mts::managed_ptr<settings_window> win = settings_window::create(parent, std::move(content), position);

  //
  ////    void add_to_desktop(window::type __type = window::type::normal, style_flags __flags = style_flags::all);
  //    _VMTS::managed_ptr<_VMTS::window> win = _VMTS::make_managed<mts::window>();
  ////    mts::window* win = new mts::window();
  //
  //    win->add(std::move(content));
  //
  //
  //    win->set_size(ptr->get_size());
  //    mts::desktop::get_modal_manager().add(win);
  //    win->add_to_desktop_centered(mts::window::type::normal, mts::window::style_flags::has_title_bar |
  //    mts::window::style_flags::has_close_button |  mts::window::style_flags::has_drop_shadow);
  //
  //    win->set_visible(true);
  //
  ////    ptr->_settings_window = win;
  //
  ////  win->set_close_on_left_up_outside(true);
  ////  win->set_close_on_escape_key(true);
  ////  win->set_close_on_return_key(true);
}

mts::managed_ptr<settings_window> settings_window::create(
    mts::view& parent, mts::managed_ptr<mts::view>&& content, const mts::point<int>& position) {

  bool key_focus = content->wants_keyboard_focus();
  mts::managed_ptr<settings_window> pwin(new settings_window(parent, std::move(content)), true);

  //  std::shared_ptr<popup_window> pwin(new popup_window(parent, std::move(content)));
  mts::desktop::get_modal_manager().add(pwin);

  pwin->set_wants_keyboard_focus(key_focus);

  pwin->add_to_desktop(mts::window::type::modal,
      mts::window::style_flags::has_title_bar | mts::window::style_flags::has_close_button
          | mts::window::style_flags::has_drop_shadow);
  pwin->set_frame_position(position);

  if (key_focus) {
    pwin->grab_focus();
  }
  return pwin;
}

// mts::managed_ptr<popup_window> popup_window::create(
//     mts::view& parent, mts::managed_ptr<mts::view>&& content, const mts::point& position) {
//
//   std::shared_ptr<popup_window> pwin(new popup_window(parent, std::move(content)));
//   mts::desktop::get_modal_manager().add(mts::managed_ptr<mts::view>(pwin));
//
//   pwin->add_to_desktop(mts::window::type::popup, mts::window::style_flags::has_drop_shadow);
//   pwin->set_frame_position(position);
//
//   pwin->grab_focus();
//
//   return pwin;
// }

settings_window::settings_window(mts::view& __view, mts::managed_ptr<mts::view>&& content)
    : _view(__view)
    , _content(std::move(content)) {

  if (mts::view* p = _view.get_root_parent()) {
    p->listeners.add(this);
  }

  listeners.add(this);

  add(*_content);
  _content->listeners.add(this);
  set_size(_content->get_size());

  set_wants_unbounded_mouse_events(true);
  set_wants_unfocused_key_events(true);
}

settings_window::~settings_window() {
  if (mts::view* p = _view.get_root_parent()) {
    p->listeners.remove(this);
  }

  _content->listeners.remove(this);
  listeners.remove(this);
}

void settings_window::mouse_left_up(const mts::mouse_event& evt) {
  if (get_local_bounds().intersect(evt.get_position())) {
    return;
  }

  if (_close_on_left_up_outside) {
    if (mts::native_view* nv = get_native_view()) {
      if (nv->dragged_after_creation()) {
        close();
      }
    }
  }
}

bool settings_window::key_pressed(const mts::key_event& kevt) {
  if (_close_on_escape && kevt.is_escape()) {
    close();
    return true;
  }
  else if (_close_on_return_key && kevt.is_return()) {
    close();
    return true;
  }

  return false;
}

void settings_window::view_bounds_changed(mts::view& v, const mts::view::bounds_status& bs) {
  if (&v == _content.get() && bs.size_changed()) {
    set_size(_content->get_size());
  }
}

void settings_window::view_being_deleted(mts::view& v) {
  if (&v == _content.get()) {
    _content->listeners.remove(this);
    close();
  }
}

void settings_window::view_visibility_changed(mts::view& v) {
  if (&v == _content.get()) {
    if (!v.is_visible()) {
      close();
    }
  }
  else if (!v.is_visible() && &v != this) {
    close();
  }
}

void settings_window::view_focus_changed(mts::view& v, bool focused) {
  //  if ((!focused && &v == this) || (focused && &v != this)) {
  //    close();
  //  }
}
