#include "mts/ui/controls/button.h"

MTS_BEGIN_NAMESPACE
button::button() {
  set_size({ 100, 25 });
  set_clipping(false);

  _font = mts::get_assets()->fonts.get("roboto-medium-12");
  _current_font_name = "roboto-medium-12";
  _current_color = get_attribute<mts::style::button::background_color>();
  _current_text_color = get_attribute<mts::style::button::text_color>();

  update_path();

  set_wants_keyboard_focus(true);
}

button::button(const std::string& name)
    : view(name) {
  set_size({ 100, 25 });
  set_clipping(false);

  _font = mts::get_assets()->fonts.get("roboto-medium-12");
  _current_font_name = "roboto-medium-12";
  _current_color = get_attribute<mts::style::button::background_color>();
  _current_text_color = get_attribute<mts::style::button::text_color>();

  update_path();

  set_wants_keyboard_focus(true);
}

void button::attribute_changed(std::string_view group, std::string_view name) {
  mts::unused(name);
  if (group == "button") {
    style_changed();
  }
}

void button::update_path() {
  _bg_path = mts::path();
  float bw = get_attribute<mts::style::button::border_width>();
  float border_radius = get_attribute<mts::style::button::border_radius>();

  if (border_radius) {
    _bg_path.add_rounded_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }), border_radius);
  }
  else {
    _bg_path.add_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }));
  }
}

void button::style_changed() {
  _current_color = get_attribute<mts::style::button::background_color>();
  _current_text_color = get_attribute<mts::style::button::text_color>();
  update_path();

  std::string font_name = get_attribute<mts::style::button::font_name>();
  ;
  if (!font_name.empty() && font_name != _current_font_name) {
    _font = mts::get_assets()->fonts.get(font_name);
    _current_font_name = font_name;
  }

  set_opaque(get_attribute<mts::style::button::border_radius>() == 0 && _current_color.is_opaque()
      && (get_attribute<mts::style::button::border_width>() == 0
          || get_attribute<mts::style::button::border_color>().is_opaque()));
  repaint();
}

void button::mouse_enter(const mts::mouse_event& evt) {
  mts::unused(evt);
  _current_color = get_attribute<mts::style::button::hover_background_color>();
  _current_text_color = get_attribute<mts::style::button::hover_text_color>();
  repaint();
}

void button::mouse_leave() {
  _current_color = get_attribute<mts::style::button::background_color>();
  _current_text_color = get_attribute<mts::style::button::text_color>();
  repaint();
}

void button::mouse_left_down(const mts::mouse_event& evt) {
  mts::unused(evt);
  _is_down = true;

  _current_color = get_attribute<mts::style::button::pressed_background_color>();
  _current_text_color = get_attribute<mts::style::button::pressed_text_color>();

  if (!_click_on_mouse_up && on_click) {
    on_click();
  }
  repaint();
}

void button::mouse_left_up(const mts::mouse_event& evt) {
  _is_down = false;
  if (get_local_bounds().contains(evt.get_position())) {
    _current_color = get_attribute<mts::style::button::hover_background_color>();
    _current_text_color = get_attribute<mts::style::button::hover_text_color>();

    if (_click_on_mouse_up && on_click) {
      on_click();
    }
  }
  else {
    _current_color = get_attribute<mts::style::button::background_color>();
    _current_text_color = get_attribute<mts::style::button::text_color>();
  }

  repaint();
}

void button::bounds_changed(const mts::view::bounds_status& bs) {
  if (bs.size_changed()) {
    update_path();
    repaint();
  }
}

void button::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {
  mts::unused(dirty_rect);

  gc.set_fill_color(_current_color);
  gc.fill_path(_bg_path);

  gc.set_fill_color(_current_text_color);
  gc.draw_text_centered(_font, _text, get_local_bounds());

  // Border.
  gc.set_line_width(get_attribute<mts::style::button::border_width>());
  gc.set_stroke_color(get_attribute<mts::style::button::border_color>());

  gc.stroke_path(_bg_path);
}

MTS_END_NAMESPACE
