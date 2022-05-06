#include "mts/ui/controls/combo_box.h"

MTS_BEGIN_NAMESPACE

combo_box::combo_box() {
  set_size({ 100, 25 });

  _menu.set_callback(std::bind(&combo_box::callback, this, std::placeholders::_1));
  _font = mts::get_assets()->fonts.get("roboto-regular-12");
  _current_font_name = "roboto-regular-12";

  _current_color = get_attribute<mts::style::combo_box::background_color>();
  _current_text_color = get_attribute<mts::style::combo_box::text_color>();
  _current_shadow_color = get_attribute<mts::style::combo_box::shadow_color>();
  update_path();

  _arrow_path = mts::path::create_down_arrow();

  if (_current_shadow_color.alpha()) {
    set_drawing_margin({ 3, 3, 3, 3 });
  }
}

combo_box::~combo_box() {}

void combo_box::set_text(const std::string& text) {
  if (mts::assign(_text, text)) {
    repaint();
  }
}

mts::popup_menu::item::uid combo_box::add_item(const std::string& name) { return _menu.add_item(name); }

void combo_box::set_selected_item_id(mts::popup_menu::item::uid item_id) {
  if (!mts::assign(_selected_item_id, item_id)) {
    return;
  }

  set_selected_item(_menu.get_item(item_id));
}

void combo_box::set_selected_item_index(std::size_t item_index) {
  set_selected_item(_menu.get_item_from_index(item_index));
}

void combo_box::set_selected_item(const mts::popup_menu::item* item) {
  if (!item) {
    if (!mts::assign(_selected_item_id, mts::popup_menu::item::invalid_id)) {
      return;
    }

    set_text("");
    _menu.unselect_all();
    return;
  }

  if (!mts::assign(_selected_item_id, item->id)) {
    return;
  }

  set_text(item->text);
  _menu.set_selected_item_id(item->id);
}

void combo_box::callback(mts::popup_menu::item::uid item_id) {
  if (!mts::assign(_selected_item_id, item_id)) {
    return;
  }

  if (const mts::popup_menu::item* item = _menu.get_item(item_id)) {
    set_text(item->text);
    _menu.set_selected_item_id(item_id);
    return;
  }

  set_text("");
  _menu.unselect_all();
}

void combo_box::mouse_left_down(const mts::mouse_event& evt) {

  mts::unused(evt);
  _menu.show_menu(*this, get_screen_position(), { get_width(), 22 });
}

void combo_box::update_path() {
  _bg_path = mts::path();
  int bw = get_attribute<mts::style::combo_box::border_width>();
  float border_radius = get_attribute<mts::style::combo_box::border_radius>();

  if (border_radius) {
    _bg_path.add_rounded_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }), border_radius);
  }
  else {
    _bg_path.add_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }));
  }
}

void combo_box::attribute_changed(std::string_view group, std::string_view name) {
  mts::unused(name);
  if (group == "combo_box") {
    style_changed();
  }
}

void combo_box::style_changed() {
  _current_color = get_attribute<mts::style::combo_box::background_color>();
  _current_text_color = get_attribute<mts::style::combo_box::text_color>();
  update_path();

  std::string font_name = get_attribute<mts::style::combo_box::font_name>();
  if (!font_name.empty() && font_name != _current_font_name) {
    _font = mts::get_assets()->fonts.get(font_name);
    _current_font_name = font_name;
  }

  if (mts::assign(_current_shadow_color, get_attribute<mts::style::combo_box::shadow_color>())) {
    if (_current_shadow_color.alpha()) {
      set_drawing_margin({ 3, 3, 3, 3 });
    }
    else {
      set_drawing_margin({ 0, 0, 0, 0 });
    }
  }

  set_opaque(get_attribute<mts::style::combo_box::border_radius>() == 0 && _current_color.is_opaque()
      && (get_attribute<mts::style::combo_box::border_width>() == 0
          || get_attribute<mts::style::combo_box::border_color>().is_opaque())
      && (_current_shadow_color.alpha() == 0));
  repaint();
}

void combo_box::bounds_changed(const mts::view::bounds_status& bs) {
  if (bs.size_changed()) {
    update_path();
    repaint();
  }
}

void combo_box::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {
  mts::unused(dirty_rect);
  mts::rect<float> bounds = get_local_fbounds();
  gc.set_fill_color(_current_color);

  if (_current_shadow_color.alpha()) {
    gc.fill_path_with_shadow(_bg_path, 2.0f, _current_shadow_color, { 0, -0.5 });
  }
  else {
    gc.fill_path(_bg_path);
  }

  if (!_text.empty()) {
    gc.set_fill_color(_current_text_color);
    gc.draw_text_centered(_font, _text, bounds);
  }

  // Border.
  gc.set_line_width(get_attribute<mts::style::combo_box::border_width>());
  gc.set_stroke_color(get_attribute<mts::style::combo_box::border_color>());
  gc.stroke_path(_bg_path);

  gc.set_fill_color(0x888888FF);
  constexpr mts::size<float> arrow_size = { 8, 5 };
  gc.fill_path(_arrow_path,
      { { bounds.width - arrow_size.width - 7, (bounds.height - arrow_size.height) * 0.5f + 0.5f }, arrow_size });
}

MTS_END_NAMESPACE
