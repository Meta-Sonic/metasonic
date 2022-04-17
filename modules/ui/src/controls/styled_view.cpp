#include "mts/ui/controls/styled_view.h"

MTS_BEGIN_NAMESPACE
styled_view::styled_view() {
  set_size({ 100, 25 });
  set_clipping(false);

  _text_line.set_font(mts::get_assets()->fonts.get("roboto-medium-12"));
  _current_font_name = "roboto-medium-12";
  _current_color = get_attribute<mts::style::styled_view::background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::text_color>();

  update_path();

  set_wants_keyboard_focus(true);
}

styled_view::styled_view(const std::string& name)
    : view(name) {
  set_size({ 100, 25 });
  set_clipping(false);

  _text_line.set_font(mts::get_assets()->fonts.get("roboto-medium-12"));
  _current_font_name = "roboto-medium-12";
  _current_color = get_attribute<mts::style::styled_view::background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::text_color>();

  update_path();

  set_wants_keyboard_focus(true);
}

void styled_view::attribute_changed(std::string_view group, std::string_view name) {
  mts::unused(name);
  if (group == "styled_view") {
    style_changed();
  }
}

void styled_view::update_path() {
  _bg_path = mts::path();
  float bw = get_attribute<mts::style::styled_view::border_width>();
  float border_radius = get_attribute<mts::style::styled_view::border_radius>();

  if (border_radius) {
    _bg_path.add_rounded_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }), border_radius);
  }
  else {
    _bg_path.add_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }));
  }
}

void styled_view::style_changed() {
  _current_color = get_attribute<mts::style::styled_view::background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::text_color>();
  update_path();

  std::string font_name = get_attribute<mts::style::styled_view::font_name>();
  ;
  if (!font_name.empty() && font_name != _current_font_name) {

    _text_line.set_font(mts::get_assets()->fonts.get(font_name));
    _current_font_name = font_name;
  }

  set_opaque(get_attribute<mts::style::styled_view::border_radius>() == 0 && _current_color.is_opaque()
      && (get_attribute<mts::style::styled_view::border_width>() == 0
          || get_attribute<mts::style::styled_view::border_color>().is_opaque()));
  repaint();
}

void styled_view::mouse_enter(const mts::mouse_event& evt) {
  mts::unused(evt);
  _current_color = get_attribute<mts::style::styled_view::hover_background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::hover_text_color>();
  repaint();
}

void styled_view::mouse_leave() {
  _current_color = get_attribute<mts::style::styled_view::background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::text_color>();
  repaint();
}

void styled_view::mouse_left_down(const mts::mouse_event& evt) {
  mts::unused(evt);
  _is_down = true;

  _current_color = get_attribute<mts::style::styled_view::pressed_background_color>();
  _current_text_color = get_attribute<mts::style::styled_view::pressed_text_color>();

  if (!_click_on_mouse_up && on_click) {
    on_click();
  }
  repaint();
}

void styled_view::mouse_left_up(const mts::mouse_event& evt) {
  _is_down = false;
  if (get_local_bounds().contains(evt.get_position())) {
    _current_color = get_attribute<mts::style::styled_view::hover_background_color>();
    _current_text_color = get_attribute<mts::style::styled_view::hover_text_color>();

    if (_click_on_mouse_up && on_click) {
      on_click();
    }
  }
  else {
    _current_color = get_attribute<mts::style::styled_view::background_color>();
    _current_text_color = get_attribute<mts::style::styled_view::text_color>();
  }

  repaint();
}

void styled_view::bounds_changed(const mts::view::bounds_status& bs) {
  if (bs.size_changed()) {
    update_path();
    repaint();
  }
}

void styled_view::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {
  mts::unused(dirty_rect);

  gc.set_fill_color(_current_color);
  gc.fill_path(_bg_path);

  gc.set_fill_color(_current_text_color);
  _text_line.draw(gc, get_local_fbounds() + get_attribute<mts::style::styled_view::text_offset>(),
      get_attribute<mts::style::styled_view::text_alignment>());

  draw_geometries(gc, dirty_rect);

  // Border.
  gc.set_line_width(get_attribute<mts::style::styled_view::border_width>());
  gc.set_stroke_color(get_attribute<mts::style::styled_view::border_color>());

  gc.stroke_path(_bg_path);
}

MTS_END_NAMESPACE
