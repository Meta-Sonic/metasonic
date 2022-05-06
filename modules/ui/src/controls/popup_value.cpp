//#include "dragon-fruit/controls/popup_value.h"
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
// popup_value_content::popup_value_content() {
//  set_wants_keyboard_focus(false);
//  set_wants_mouse_move(false);
//
//
//  _font = _VDNFT::get_assets()->fonts.get("roboto-regular-12");
//  set_size({ 100, 25 });
//
//  //  constexpr int width = style::size.width;
//  //  constexpr int height = style::size.height;
//  //  constexpr float half_height = get * 0.5f;
//  //
//  //  setSize(width, height);
//
//
//}
//
// popup_value_content::~popup_value_content() { dnft::print("popup_value::~popup_value()"); }
//
// void popup_value_content::set_text(const std::string& text) {
//  if (_VDNFT::assign(_text, text)) {
//    repaint();
//  }
//}
//
// void popup_value_content::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//  if (bs.size_changed()) {
//    _VDNFT::frect bounds = get_local_fbounds();
//
//    _path = _VDNFT::path();
//    _path.start_subpath({ 0.0f, bounds.height * 0.5f });
//    _path.line_to({ 10, bounds.height });
//    _path.line_to({ bounds.width - 10, bounds.height });
//    _path.line_to({ bounds.width - 10, 0 });
//    //    _path.quadratic_to(_VDNFT::fpoint(bounds.width - 4, bounds.height * 0.5f), {bounds.width - 10, 0});
//    _path.line_to({ 10, 0 });
//    _path.close_subpath();
//
//
//    //  _inv_path.startNewSubPath(0, 0);
//    //  _inv_path.lineTo(width - 10, 0);
//    //  _inv_path.lineTo(width, half_height);
//    //  _inv_path.lineTo(width - 10, height);
//    //  _inv_path.lineTo(0, height);
//    //  _inv_path.closeSubPath();
//
//    repaint();
//  }
//}
//
// void popup_value_content::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  _VDNFT::unused(dirty_rect);
//
//  _VDNFT::frect bounds = get_local_fbounds();
//
//  gc.set_fill_color(0x222222FF);
//  //    gc.fill_rect(dirty_rect);
//  gc.fill_path(_path);
//
//  //  gc.set_fill_color(0xFF0000FF);
//  //  gc.fill_rect(bounds.reduced({10, 0}));
//
//  gc.set_fill_color(0xFFFFFFFF);
//  gc.draw_text_centered(_font, _text, bounds.reduced({ 10, 0 }));
//  //  dnft::print("COMBO BOX PAINT", gc.get_clipping_rect(), "---", dirty_rect, "---", get_local_fbounds());
//}
//
// DNFT_END_NAMESPACE
