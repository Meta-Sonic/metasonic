//#include "dragon-fruit/controls/scroll_view.h"
//
// DNFT_BEGIN_NAMESPACE
// scroll_view::scroll_view() {
//
//  _VDNFT::size size = { 100, 100 };
//  _scroll_bar.add_listener(this);
//  add(_scroll_bar, { size.width - 10, 0, 10, size.height });
//  _scroll_bar.set_scroll_ratio(1);
//
//  set_size(size);
//}
//
// void scroll_view::set_content_view(_VDNFT::managed_ptr<_VDNFT::view>&& v) {
//  if (_content_view != v) {
//    if (_content_view) {
//      _content_view->listeners.remove(this);
//      remove(_content_view.get());
//    }
//
//    _content_view = std::move(v);
//    add(*_content_view);
//    if (_content_view) {
//      _content_view->listeners.add(this);
//    }
//
//    update_content_view();
//    _scroll_bar.to_front();
//    _scroll_bar.set_value(0);
//
//    repaint();
//    //    _scroll_bar.set_value(0, false);
//    //    set_content_view_position({0, 0});
//  }
//}
//
// void scroll_view::on_scrolled(_VDNFT::scroll_bar& sb) {
//  if (!_content_view) {
//    return;
//  }
//
//  float svalue = sb.get_value();
//  set_content_view_position({ 0, -(int)(svalue * (_content_view->get_height() - get_height())) });
//}
//
// void scroll_view::set_content_view_position(const _VDNFT::point& pos) {
//  if (_VDNFT::view* content = _content_view.get()) {
//    if (get_height() >= content->get_height()) {
//      content->set_position({ 0, 0 });
//    }
//    else {
//      int x = pos.x;
//      int y = std::clamp(pos.y, get_height() - content->get_height(), content->get_height() - get_height());
//      content->set_position({ x, y });
//    }
//  }
//}
//
// void scroll_view::update_content_view() {
//  if (_VDNFT::view* content = _content_view.get()) {
//    float ratio = get_content_view_height_ratio();
//    _scroll_bar.set_scroll_ratio(ratio);
//  }
//}
//
// void scroll_view::view_bounds_changed(_VDNFT::view& v, const _VDNFT::view::bounds_status& bs) {
//  if (&v == _content_view.get() && bs.size_changed()) {
//    float new_ratio = get_content_view_height_ratio();
//    float old_ratio = _scroll_bar.get_scroll_ratio();
//    float old_value = _scroll_bar.get_value();
//    update_content_view();
//    _scroll_bar.set_value(old_value * (new_ratio / old_ratio), false);
//  }
//}
//
// void scroll_view::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//  if (bs.size_changed()) {
//    _scroll_bar.set_bounds(
//        { get_size().width - _scroll_bar.get_size().width, 0, _scroll_bar.get_size().width, get_size().height });
//
//    update_content_view();
//  }
//}
//
// void scroll_view::mouse_wheel(const _VDNFT::mouse_event& evt, const _VDNFT::scroll_event& sevt) {
//  _VDNFT::unused(evt);
//  _scroll_bar.scroll(sevt.is_reversed() ? sevt.get_delta().y : -sevt.get_delta().y);
//}
//
// void scroll_view::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  _VDNFT::unused(gc, dirty_rect);
//  //    gc.set_fill_color(0x999999FF);
//  //    gc.fill_rect(dirty_rect);
//}
//
// DNFT_END_NAMESPACE
