//#include "settings_view.h"
//
//settings_view::settings_view() {
//  _font = _VDNFT::get_assets()->fonts.get("roboto-medium-12");
//
//  _label.set_text("Button :");
//  add(_label, { 0, 35, 75, 20 });
//
//  _cb_label.set_text("Combo Box :");
//  add(_cb_label, { 0, 65, 75, 20 });
//
//  _button.set_text("Button");
//  _button.attributes.set_style("button");
//  add(_button, { 85, 35, 100, 20 });
//
//  _combo_box.add_item("Small");
//  _combo_box.add_item("Medium");
//  _combo_box.add_item("Large");
//  _combo_box.set_selected_item_index(0);
//  add(_combo_box, { 85, 65, 100, 20 });
//
//  _tb_label.set_text("Text Box :");
//  add(_tb_label, { 0, 95, 75, 20 });
//
//  _text_box.set_text("Text Box");
//  _text_box.set_placeholder("Bingo");
//
//  //  _text_box.set_font(_VDNFT::get_assets()->fonts.get("roboto-medium-14"));
//  //    _text_box.set_font(_VDNFT::get_assets()->fonts.get("password"));
//  //    _text_box.set_text("Alexandre");
//  //  _text_box.set_maximum_text_size(4);
//  add(_text_box, { 85, 95, 100, 20 });
//
//  set_drawing_margin({ 3, 3, 3, 3 });
//
//  set_size({ 200, 130 });
//}
//
//settings_view::~settings_view() { mts::print("settings_view::~settings_view"); }
//
//void settings_view::mouse_left_down(const mts::mouse_event& evt) {
//  if (mts::frect(0, 0, get_width(), 20).intersect(evt.get_position())) {
//    _is_selected = true;
//    _click_pos = evt.get_absolute_position();
//    _start_pos = get_position();
//
//    _text_box.set_text("BANANA");
//    //    set_cursor(mts::cursor_type::move);
//    repaint();
//  }
//  //
//}
//void settings_view::mouse_left_drag(const mts::mouse_event& evt) {
//  if (!_is_selected) {
//    return;
//  }
//
//  mts::fpoint m_abs = evt.get_absolute_position();
//  //  mts::fpoint abs_pos = get_absolute_position();
//
//  mts::point new_pos = m_abs - _click_pos + _start_pos;
//  //  new_pos.x = std::clamp(new_pos.x, 0, get_parent()->get_width() - get_width());
//  //  new_pos.y = std::clamp(new_pos.y, 0, get_parent()->get_height() - get_height());
//
//  set_position(new_pos);
//}
//void settings_view::mouse_left_up(const mts::mouse_event& evt) {
//  _VDNFT::unused(evt);
//  if (mts::assign(_is_selected, false)) {
//    //    set_cursor(mts::cursor_type::normal);
//    repaint();
//  }
//}
//
//void settings_view::bounds_changed(const mts::view::bounds_status& bs) {
//  _VDNFT::unused(bs);
//  _bg_path = mts::path();
//  _bg_path.add_rounded_rect(get_local_fbounds(), 5.0f);
//
//  _header_path = mts::path();
//  _header_path.add_rounded_rect(
//      get_local_fbounds().with_height(20).reduced({ 0.1, 0.1 }), { 5.0f, 5.0f }, true, true, false, false);
//  //  _header_path.add_rounded_rect(get_local_fbounds().with_height(20), 5.0f);
//}
//
//void settings_view::paint(mts::graphics::context& gc, const mts::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//  //  mts::print("DDDDRIRRIIR", dirty_rect);
//
//  mts::frect bounds = get_local_fbounds();
//
//  gc.set_fill_color(0xEAEBEBFF);
//  //  gc.fill_path(_bg_path);
//  gc.fill_path_with_shadow(_bg_path, 3.0f, 0x00000022, { 0, 0 });
//  //  gc.fill_rect(dirty_rect);
//
//  //  gc.save_state();
//  //  gc.clip_to_path(_bg_path);
//
//  mts::color header_color = _is_selected ? 0xDFDFDFFF : 0xDCDCDCFF;
//  gc.set_fill_color(header_color);
//  gc.fill_path(_header_path);
//  //  gc.restore_state();
//
//  gc.set_fill_color(0x666666FF);
//  gc.draw_text_centered(_font, "Widgets", { 0, 0, bounds.width, 20 });
//
//  mts::color contour_color = _is_selected ? 0x777777FFF : 0xAAAAAAFF;
//
//  gc.set_line_width(1.0f);
//  gc.set_stroke_color(contour_color);
//  gc.stroke_path(_bg_path);
//
//  gc.stroke_line({ 0, 20 }, { bounds.width, 20 });
//  //  gc.stroke_rect(get_local_fbounds());
//
//  gc.set_fill_color(0xFF0000FF);
//  gc.fill_rect({ 0.0f, 0.0f, 22.0312f, 22.0312f });
//}
