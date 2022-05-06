#include "main_window.h"

inline void check(mts::view* v1, mts::view* v2) {
  //  mts::print("ffdfdfdfdfdf", typeid(*v1).name(), typeid(*v2).name());
}

main_window::main_window() {
  //  set_title("Demo");
  set_name("W");
  //  set_wants_unbounded_mouse_events(true);

  //  _content = std::make_unique<main_view>();
  _content.set_bounds({ 50, 50, 200, 200 });
  add(_content);

  //  _view.set_bounds({ 50, 50, 30, 30 });
  //  _content.add(_view);

  //  std::unique_ptr<mts::view> vv = std::make_unique<mts::view>();

  //  auto& vvv = *vv;
  //  mts::view& mv = *(&_content);

  //  check(vv.get(), &_content);
  //  mts::print("DLKLDKLD", typeid(vvv).name(), typeid(mv).name(), typeid(*this).name());

  //  _bail = std::make_unique<mts::view::bailout_checker>(*_content);
}

main_window::~main_window() {
  //  _content.reset();
  //  mts::print("MAIN WIN DDD", _bail->expired());
}

void main_window::mouse_moved(const mts::mouse_event& evt) {
  //  _VDNFT::unused(evt);
  //  mts::print("main_window::mouse_moved", evt.get_position(), get_local_bounds().intersect(evt.get_position()),
  //  "--",
  //      get_local_bounds());
}

void main_window::mouse_left_down(const mts::mouse_event& evt) {
  mts::print("main_window::mouse_left_down", evt.get_position());
}

void main_window::visibily_changed() { mts::print("main_window::visibily_changed"); }

void main_window::bounds_changed(const mts::view::bounds_status& bs) {
  mts::print("main_window::bounds_changed", get_bounds());
  if (bs.size_changed()) {
    _content.set_bounds({ 0, 0, get_size() });
  }
}

void main_window::frame_changed() { mts::print("main_window::frame_changed", get_screen_position()); }

void main_window::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {
  //  _VDNFT::unused(dirty_rect);
  gc.set_fill_color(0xFF0000FF);
  gc.fill_rect(get_local_bounds());
}
