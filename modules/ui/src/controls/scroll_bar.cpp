//#include "dragon-fruit/controls/scroll_bar.h"
//
// DNFT_BEGIN_NAMESPACE
// scroll_bar::scroll_bar()
//    : _diff_fade(*this) {
//
//  _current_bg_color = _bg_color;
//  _bar_color = 0x44444499;
//
//  _bar_selected_color = 0xFF5500FF;
//  _current_bar_color = _bar_color;
//
//  set_visible(false);
//  set_opacity(0);
//}
//
// void scroll_bar::set_value(float v, bool notify_listeners) {
//  if (_VDNFT::assign(_h_value, std::clamp(v, 0.0f, 1.0f))) {
//    _VDNFT::rect bounds = get_local_bounds();
//    float height = bounds.height * _h_ratio;
//    float y = _h_value * (bounds.height - height);
//
//    _VDNFT::frect last_bar_rect = _bar_rect;
//    set_bar_rect({ 2.0f, y, bounds.width - 4.0f, height });
//    repaint_bar(last_bar_rect);
//
//    if (notify_listeners) {
//      _listeners.notify(&listener::on_scrolled, *this);
//    }
//  }
//}
//
// void scroll_bar::set_scroll_ratio(float ratio) {
//  if (_VDNFT::assign(_h_ratio, std::clamp(ratio, 0.1f, 1.0f))) {
//    _VDNFT::rect bounds = get_local_bounds();
//    float height = bounds.height * _h_ratio;
//    float y = _h_value * (bounds.height - height);
//    set_bar_rect({ 2.0f, y, get_size().width - 4.0f, height });
//    repaint();
//  }
//
//  //  if (_VDNFT::assign(_h_ratio, std::clamp(ratio, 0.0f, 1.0f))) {
//  //    _VDNFT::rect bounds = get_local_bounds();
//  //    float height = bounds.height * _h_ratio;
//  //    float y = _h_value * (bounds.height - height);
//  //    set_bar_rect({ 2.0f, y, get_size().width - 4.0f, height });
//  //    repaint();
//  //  }
//}
//
// void scroll_bar::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//  if (bs.size_changed()) {
//    _VDNFT::rect bounds = get_local_bounds();
//    float height = bounds.height * _h_ratio;
//    float y = _h_value * (bounds.height - height);
//    set_bar_rect({ 2.0f, y, get_size().width - 4.0f, height });
//  }
//}
//
// void scroll_bar::scroll(float dy) {
//  if (is_scrollable()) {
//    _diff_fade.fade_in();
//    _diff_fade.fade_out();
//  }
//
//  _VDNFT::rect bounds = get_local_bounds();
//  const float height = bounds.height * _h_ratio;
//
//  const float by = _bar_rect.y + dy;
//  const float h_value = std::clamp<float>(by / (bounds.height - height), 0.0f, 1.0f);
//
//  set_value(h_value);
//}
//
// void scroll_bar::mouse_wheel(const mouse_event& evt, const _VDNFT::scroll_event& sevt) {
//  if (is_scrollable()) {
//    _diff_fade.fade_in();
//  }
//
//  if (!get_local_bounds().contains(evt.get_position())) {
//    _diff_fade.fade_out();
//  }
//
//  _VDNFT::fpoint delta = sevt.get_delta();
//
//  _VDNFT::rect bounds = get_local_bounds();
//  const float height = bounds.height * _h_ratio;
//
//  const float by = _bar_rect.y + (sevt.is_reversed() ? delta.y : -delta.y);
//  const float h_value = std::clamp<float>(by / (bounds.height - height), 0.0f, 1.0f);
//
//  set_value(h_value);
//}
//
// void scroll_bar::mouse_left_down(const _VDNFT::mouse_event& evt) {
//  if (_bar_rect.intersect(evt.get_position())) {
//    _click_delta = evt.get_position().y - _bar_rect.y;
//    _is_dragging = true;
//  }
//  repaint();
//}
//
// void scroll_bar::mouse_left_up(const _VDNFT::mouse_event& evt) {
//  if (!get_local_bounds().contains(evt.get_position())) {
//    _diff_fade.fade_out();
//  }
//
//  if (_is_dragging) {
//    _is_dragging = false;
//  }
//}
//
// void scroll_bar::mouse_left_drag(const _VDNFT::mouse_event& evt) {
//  if (!_is_dragging) {
//    return;
//  }
//
//  _diff_fade.stop_fade_out();
//
//  _VDNFT::rect bounds = get_local_bounds();
//  const float height = bounds.height * _h_ratio;
//
//  const float by = evt.get_position().y - _click_delta;
//  const float h_value = std::clamp<float>(by / (bounds.height - height), 0.0f, 1.0f);
//  set_value(h_value);
//
//  //  if (_VDNFT::assign(_h_value, h_value)) {
//  //    const float y = _h_value * (bounds.height - height);
//  //    _VDNFT::frect last_bar_rect = _bar_rect;
//  //    set_bar_rect({ 2.0f, y, get_size().width - 4.0f, height });
//  //    repaint_bar(last_bar_rect);
//  //  }
//}
//
// void scroll_bar::mouse_moved(const _VDNFT::mouse_event& evt) {
//  _diff_fade.stop_fade_out();
//
//  if (_bar_rect.intersect(evt.get_position())) {
//    if (_is_selected) {
//      return;
//    }
//    _is_selected = true;
//    _current_bar_color = _bar_selected_color;
//    repaint_bar();
//  }
//  else {
//    if (_is_selected) {
//      _is_selected = false;
//      _current_bar_color = _bar_color;
//      repaint_bar();
//    }
//  }
//}
//
// void scroll_bar::mouse_enter(const _VDNFT::mouse_event& evt) {
//  _diff_fade.stop_fade_out();
//
//  if (is_visible() && is_scrollable()) {
//    _diff_fade.fade_in();
//  }
//
//  if (_bar_rect.intersect(evt.get_position())) {
//    _is_selected = true;
//    _current_bar_color = _bar_selected_color;
//    repaint_bar();
//  }
//
//  if (is_visible()) {
//    enlarge();
//  }
//}
//
// void scroll_bar::mouse_leave() {
//  _diff_fade.fade_out();
//
//  if (_is_selected) {
//    _is_selected = false;
//    _current_bar_color = _bar_color;
//    repaint_bar();
//  }
//}
//
// void scroll_bar::enlarge() {
//  if (!_is_enlarge) {
//    _is_enlarge = true;
//    _VDNFT::rect bounds = get_bounds();
//    set_bounds({ bounds.x - 3, bounds.y, bounds.width + 3, bounds.height });
//  }
//}
//
// void scroll_bar::visibily_changed() {
//  if (!is_visible() && _is_enlarge) {
//    _VDNFT::rect bounds = get_bounds();
//    set_bounds({ bounds.x + 3, bounds.y, bounds.width - 3, bounds.height });
//    _is_enlarge = false;
//  }
//}
//
// void scroll_bar::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//
//  if (_is_enlarge) {
//    // Background.
//    gc.set_fill_color(_current_bg_color);
//    gc.fill_rect(get_local_bounds());
//
//    // Line.
//    gc.set_fill_color(_current_bg_color.darker(0.6f));
//    gc.fill_rect(get_local_bounds().with_width(1));
//
//    // Bar.
//    gc.set_fill_color(_bar_color);
//    gc.fill_path(_bar_path_enlarge);
//  }
//  else {
//    // Bar.
//    gc.set_fill_color(_bar_color);
//    gc.fill_path(_bar_path);
//  }
//}
// DNFT_END_NAMESPACE
