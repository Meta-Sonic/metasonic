//#include "dragon-fruit/controls/label.h"
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
// label::label() {
//  set_size({ 100, 25 });
//
//  _font = _VDNFT::get_assets()->fonts.get("roboto-regular-12");
//  set_wants_mouse_move(false);
//}
//
// label::~label() {}
//
// void label::set_text(const std::string& text) {
//  if (_VDNFT::assign(_text, text)) {
//    repaint();
//  }
//}
//
// void label::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//
//  _VDNFT::frect bounds = get_local_fbounds();
//
//  //  gc.set_fill_color(0x77777733);
//  //  gc.fill_rect(dirty_rect);
//
//  gc.set_fill_color(0x444444FF);
//  gc.draw_text_right_aligned(_font, _text, bounds);
//  //  dnft::print("COMBO BOX PAINT", gc.get_clipping_rect(), "---", dirty_rect, "---", get_local_fbounds());
//}
//
// DNFT_END_NAMESPACE
