//#include "dragon-fruit/controls/scroll_list.h"
//#include "dragon-fruit/core/print.h"
////#include <unordered_set>
//
// DNFT_BEGIN_NAMESPACE
//// namespace {
//// struct cbstyle {
////   static constexpr auto bg_color = _VDNFT::combo_box_colors::background_color;
////   static constexpr auto txt_color = _VDNFT::combo_box_colors::text_color;
////   static constexpr auto border_color = _VDNFT::combo_box_colors::border_color;
////   static constexpr auto shadow_color = _VDNFT::combo_box_colors::shadow_color;
////   static constexpr auto border_width = _VDNFT::combo_box_values::border_width;
////   static constexpr auto border_radius = _VDNFT::combo_box_values::border_radius;
////   static constexpr auto font_name = _VDNFT::combo_box_strings::font_name;
//// };
//// } // namespace
//
// list_view::list_view() {
//  set_size({ 500, 500 });
//  set_drawing_margin({ 1, 1, 1, 1 });
//  //  _font = _VDNFT::get_assets()->fonts.get("roboto-regular-12");
//  //  set_wants_mouse_move(false);
//}
//
// list_view::~list_view() {}
//
// void list_view::set_model(_VDNFT::managed_ptr<list_model>&& model) {
//  _model = model;
//
//  if (_model) {
//    //    dnft::print("MODEL");
//
//    std::size_t row_count = _model->row_count();
//    set_height(row_count * _row_height);
//  }
//}
//
// std::size_t list_view::get_row_index_at(const _VDNFT::fpoint& pos) const {
//  if (!_model) {
//    return invalid_index;
//  }
//  std::size_t row_count = _model->row_count();
//  std::size_t row_index = std::floor(_VDNFT::maximum(pos.y, 0.0f) / _row_height);
//  if (row_index > row_count) {
//    return invalid_index;
//  }
//
//  return row_index;
//}
//
// void list_view::mouse_left_down(const _VDNFT::mouse_event& evt) {
//  std::size_t start_index = get_row_index_at(evt.get_position());
//  //  dnft::print("SSS", start_index);
//
//  if (_VDNFT::assign(_selected_row, start_index)) {
//    repaint();
//  }
//}
//
// void list_view::mouse_left_up(const _VDNFT::mouse_event& evt) { _VDNFT::unused(evt); }
//
// void list_view::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//
//  _VDNFT::frect bounds = get_local_fbounds();
//
//  //  gc.set_fill_color(0xFF0000FF);
//  //  gc.fill_rect({{5.0f, 5.0f}, bounds.size});
//
//  if (_model) {
//    //    dnft::print("MODEL", dirty_rect, "--", bounds);
//
//    std::size_t start_index = get_row_index_at(dirty_rect.position);
//
//    if (start_index == invalid_index) {
//      return;
//    }
//    std::size_t row_count = _model->row_count();
//
//    std::size_t end_index = _VDNFT::minimum(row_count, start_index + get_row_count_for_height(dirty_rect.height) + 1);
//    //    dnft::print(dirty_rect.height, "get_row_index_at", start_index, end_index);
//    for (std::size_t i = start_index; i < end_index; i++) {
//      //      gc.fill_rect({ 0, (int)i * _row_height, (int)bounds.width, _row_height });
//
//      gc.save_state();
//      gc.translate({ 0, (int)i * _row_height });
//      _model->draw_row(i, gc, { (int)bounds.width, _row_height }, _selected_row == i);
//      gc.restore_state();
//    }
//
//    //    bool color_switch = false;
//    //    for (int i = 0; i < row_count; i++) {
//    //      color_switch = !color_switch;
//    //      if (color_switch) {
//    //        gc.set_fill_color(0xFF0000FF);
//    //      }
//    //      else {
//    //        gc.set_fill_color(0x0000FFFF);
//    //      }
//    //      gc.fill_rect({ 0, i * _row_height, (int)bounds.width, _row_height });
//    //
//    //      gc.save_state();
//    //      gc.translate({0, i * _row_height});
//    //      _model->draw_row(i, gc, {(int)bounds.width, _row_height}, false);
//    //      gc.restore_state();
//    //    }
//  }
//}
//
////
////
////
////
// scroll_list::scroll_list() {
//   set_size({ 100, 25 });
//   _scroll_view.listeners.add(this);
//   _scroll_view.components.add<_VDNFT::clipping>();
//   add(_scroll_view, _VDNFT::rect{ { 0, 0 }, get_size() });
//   _scroll_view.set_content_view(_list);
//
//   set_drawing_margin({ 5, 5, 5, 5 });
// }
//
// scroll_list::~scroll_list() {}
//
// void scroll_list::set_model(_VDNFT::managed_ptr<list_model>&& model) { _list.set_model(std::move(model)); }
// void scroll_list::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//   if (bs.size_changed()) {
//     _list.set_width(get_width());
//     _scroll_view.set_size(get_size());
//     //    _scroll_view.set_bounds({ { 0, 0 }, get_size() });
//   }
// }
//
// void scroll_list::view_bounds_changed(_VDNFT::view& v, const _VDNFT::view::bounds_status& bs) {
//   if (&v == &_scroll_view && bs.size_changed()) {
//     v.components.get<_VDNFT::clipping>()->set_clipping_path(
//         _VDNFT::path::create_rounded_rect(v.get_local_fbounds(), 5.0f));
//   }
// }
//
// void scroll_list::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//   _VDNFT::unused(dirty_rect);
//
//   gc.set_shadow(5.0f, 0x00000033, { 0, 0 });
//   gc.set_line_width(1.0f);
//   gc.set_stroke_color(0x888888FF);
//   gc.stroke_path(_scroll_view.components.get<_VDNFT::clipping>()->get_clipping_path());
// }
//
// DNFT_END_NAMESPACE
