#include "mts/ui/window.h"
#include "mts/ui/native_view.h"

MTS_BEGIN_NAMESPACE
window::window()
    : view(view::type::window) {}

void window::set_title(const std::string& title) {
  if (mts::assign(_title, title)) {
    if (native_view* nv = get_native_view()) {
      nv->set_title(title);
    }
  }
}

void window::close() {
  mts::print("window::close", this);
  if (_is_closing) {
    return;
  }

  _is_closing = true;
  if (native_view* nv = get_native_view()) {
    nv->close();
  }
}

void window::set_frame_position(const mts::point<int>& position) {
  if (native_view* nv = get_native_view()) {
    nv->set_bounds({ position, { nv->get_bounds().size } });
  }
}

mts::padding<int> window::get_frame_padding() const {
  if (const native_view* nv = get_native_view()) {
    return nv->get_frame_padding();
  }

  return mts::padding<int>();
}

mts::rect<int> window::get_frame_bounds() const {
  if (const native_view* nv = get_native_view()) {
    return nv->get_bounds();
  }

  return mts::rect<int>();
}

void window::set_wants_unbounded_mouse_events(bool active) {
  mts::set_flag(view::flags::f_unbounded_mouse_events, active, view::_flags);
}
bool window::get_wants_unbounded_mouse_events() const {
  return mts::has_flag(view::flags::f_unbounded_mouse_events, view::_flags);
}

void window::grab_focus() {
  if (native_view* nv = get_native_view()) {
    nv->grab_focus();
  }
}

void window::center() {
  if (native_view* nv = get_native_view()) {
    nv->center();
    nv->grab_focus();
  }
}

void window::add_to_desktop(window::type __type, window::style_flags flags) {
  native_view::add_to_desktop(this, __type, flags, nullptr);
}

void window::add_to_desktop_centered(window::type __type, window::style_flags flags) {
  native_view::add_to_desktop(this, __type, flags, nullptr);
  get_native_view()->center();
}
MTS_END_NAMESPACE
