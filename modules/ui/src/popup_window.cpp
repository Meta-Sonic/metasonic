#include "mts/ui/popup_window.h"
#include "mts/ui/desktop.h"
#include "mts/ui/native_view.h"

MTS_BEGIN_NAMESPACE

mts::managed_ptr<popup_window> popup_window::create(
    mts::view& parent, mts::managed_ptr<mts::view>&& content, const mts::point<int>& position) {

  bool key_focus = content->wants_keyboard_focus();
  mts::managed_ptr<popup_window> pwin(new popup_window(parent, std::move(content)), true);

  //  std::shared_ptr<popup_window> pwin(new popup_window(parent, std::move(content)));
  mts::desktop::get_modal_manager().add(pwin);

  pwin->set_wants_keyboard_focus(key_focus);

  pwin->add_to_desktop(mts::window::type::popup, mts::window::style_flags::has_drop_shadow);
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

popup_window::popup_window(mts::view& __view, mts::managed_ptr<mts::view>&& content)
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

popup_window::~popup_window() {
  if (mts::view* p = _view.get_root_parent()) {
    p->listeners.remove(this);
  }

  _content->listeners.remove(this);
  listeners.remove(this);
}

// void popup_window::close() {
//   dnft::print("popup_window::close()", this);
//
//   hide();
//   if(mts::native_view* nv = get_native_view()) {
//     dnft::print("popup_window::close()");
//     nv->close();
//   }
////  mts::desktop::get_modal_manager().remove(this);
//}

void popup_window::mouse_left_up(const mts::mouse_event& evt) {
  if (get_local_bounds().intersect(evt.get_position())) {
    return;
  }

  if (_close_on_left_up_outside) {
    if (native_view* nv = get_native_view()) {
      if (nv->dragged_after_creation()) {
        close();
      }
    }
  }
}

bool popup_window::key_pressed(const mts::key_event& kevt) {
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

void popup_window::view_bounds_changed(mts::view& v, const mts::view::bounds_status& bs) {
  if (&v == _content.get() && bs.size_changed()) {
    set_size(_content->get_size());
  }
}

void popup_window::view_being_deleted(mts::view& v) {
  if (&v == _content.get()) {
    _content->listeners.remove(this);
    close();
  }
}

void popup_window::view_visibility_changed(mts::view& v) {
  if (&v == _content.get()) {
    if (!v.is_visible()) {
      close();
    }
  }
  else if (!v.is_visible() && &v != this) {
    close();
  }
}

void popup_window::view_focus_changed(mts::view& v, bool focused) {
  if ((!focused && &v == this) || (focused && &v != this)) {
    close();
  }
}
MTS_END_NAMESPACE
