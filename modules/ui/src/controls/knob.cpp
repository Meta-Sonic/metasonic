//#include "dragon-fruit/controls/knob.h"
// DNFT_BEGIN_NAMESPACE
// knob::knob() {
//
//  _knob_rect = { _style.padding.left, _style.padding.top, _style.knob_size.width, _style.knob_size.height };
//  _knob_center
//      = _VDNFT::frect(_style.padding.left, _style.padding.top, _style.knob_size.width,
//      _style.knob_size.width).middle();
//  _knob_radius = _style.knob_size.width * 0.5f;
//
//  _VDNFT::fsize size = { _knob_rect.size.width + _style.padding.left + _style.padding.right,
//    _knob_rect.size.height + _style.padding.top + _style.padding.bottom };
//
//  set_size(size);
//}
//
// knob::~knob() {}
//
// void knob::set_normalized_value(float value) {
//  if (_VDNFT::assign(_normalized_value, std::clamp(value, 0.0f, 1.0f))) {
//    dnft::print("KNOB VALUE", _normalized_value);
//    repaint();
//  }
//}
//
// void knob::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//  if (bs.size_changed()) {
//    //    _VDNFT::rect bounds = get_local_bounds();
//    //    float height = bounds.height * _h_ratio;
//    //    float y = _h_value * (bounds.height - height);
//    //    set_bar_rect({ 2.0f, y, get_size().width - 4.0f, height });
//  }
//}
//
// void knob::mouse_wheel(const _VDNFT::mouse_event& evt, const _VDNFT::scroll_event& sevt) {}
//
// void knob::mouse_left_down(const _VDNFT::mouse_event& evt) {
//  if (_knob_rect.contains(evt.get_position())) {
//    _last_mouse_y = evt.get_position().y;
//    _knob_dragging = true;
//
//    _VDNFT::size pop_size = { 80, 20 };
//    float middle_y = _knob_rect.position.y + (_knob_rect.size.height - pop_size.height) * 0.5f;
//    _VDNFT::rect prect = { get_screen_bounds().top_right() + _VDNFT::point(0, middle_y), pop_size };
//    _popup_value.show(*this, prect, std::to_string(_normalized_value));
//
//    //    _VDNFT::point abs_pos = get_screen_position() + _VDNFT::point{ get_width(), 0 };
//    //    _VDNFT::point ppp
//    //        = _VDNFT::point(abs_pos.x, abs_pos.y + _knob_rect.position.y + (_knob_rect.size.height -
//    pop_size.height)
//    //        * 0.5f);
//
//    //    _popup_value.show(*this, prect + ppp, std::to_string(_normalized_value));
//
//    //    _VDNFT::managed_ptr<_VDNFT::popup_value_content> content =
//    dnft::make_managed<_VDNFT::popup_value_content>();
//    //    content->set_size({ 90, 20 });
//    //    content->set_text(std::to_string(_normalized_value));
//    //
//    //
//    //    _VDNFT::point abs_pos = get_screen_position() + _VDNFT::point{get_width(), 0};
//    //          _VDNFT::point ppp = _VDNFT::point(abs_pos.x,
//    //              abs_pos.y + _knob_rect.position.y + _knob_rect.size.height * 0.5f - content->get_height() * 0.5f);
//    //
//    //    _VDNFT::managed_ptr<_VDNFT::popup_window> pwin = _VDNFT::popup_window::create(
//    //        *this, std::move(content), ppp);
//    //
//    //    _popup_value = _VDNFT::weak_managed_ptr<_VDNFT::popup_window>(pwin);
//
//    //    pwin->set_wants_keyboard_focus(false);
//
//    //    _popup_value =_VDNFT::weak_managed_ptr<_VDNFT::popup_window>( pwin);
//
//    //    _popup_value
//
//    //      _popup_value = std::make_unique<popup_value_type>();
//    //
//    //
//    //      juce::Point<int> abs_pos = getScreenBounds().getTopRight();
//    //      gfx::point ppp = gfx::point(abs_pos.x,
//    //          abs_pos.y + knob_rect.position.y + knob_rect.size.height * 0.5 - _popup_value->get_height() * 0.5);
//    //
//    //      bool inverted = _show_popup_value_on_left_size || _popup_value->should_invert(ppp);
//    //
//    //
//    //
//    //      if (inverted) {
//    //        _popup_value->show(ppp - gfx::point{ (int)knob_rect.width, 0 }, inverted);
//    //      }
//    //      else {
//    //        _popup_value->show(ppp, inverted);
//    //      }
//    //
//    //      _popup_value->set_string_value(std::string(parameter_control::get_parameter_string_value()));
//    //
//    //      if (auto p = parameter_control::get_parameter()) {
//    //        p->begin_gesture();
//    //      }
//    repaint();
//  }
//}
//
// void knob::mouse_left_drag(const _VDNFT::mouse_event& evt) {
//  if (!_knob_dragging) {
//    return;
//  }
//  //
//  float dy = _last_mouse_y - evt.get_position().y;
//  _last_mouse_y = evt.get_position().y;
//
//  float speed = evt.is_shift_down() || evt.is_command_down() ? 10000.0f : 100.0f;
//  set_normalized_value(_normalized_value + dy / speed);
//
//  _popup_value.set_text(std::to_string(_normalized_value));
//  //
//
//  //    if (_popup_value) {
//  ////      _VDNFT::weak_managed_ptr<_VDNFT::view> pview = _popup_value->get_weak_content();
//  //      if(_VDNFT::weak_managed_ptr<_VDNFT::popup_value_content> pvalue= _popup_value->get_weak_content()) {
//  //        pvalue->set_text(std::to_string(_normalized_value));
//  //      }
//  //
//  ////      _popup_value->set_string_value(std::string(parameter_control::get_parameter_string_value()));
//  //    }
//  // ;
//}
//
// void knob::mouse_left_up(const _VDNFT::mouse_event& evt) {
//  if (_knob_dragging) {
//    _knob_dragging = false;
//    //    if (auto p = parameter_control::get_parameter()) {
//    //      p->end_gesture();
//    //    }
//  }
//
//  _popup_value.hide();
//
//  //  if (_popup_value) {
//  //    _popup_value->close();
//  //    _popup_value.reset();
//  //
//  //  }
//
//  //  if(_popup_value) {
//  //    _popup_value.reset();
//  //  }
//  //    if (_popup_value.is_shown()) {
//  //      _popup_value.hide();
//  //    }
//
//  repaint();
//}
//
// void knob::mouse_enter(const _VDNFT::mouse_event& evt) {}
//
// void knob::mouse_leave() {}
//
// void knob::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//  //  _VDNFT::unused(dirty_rect);
//
//  //  _VDNFT::frect bounds = get_local_fbounds();
//  //  gc.set_fill_color(0x77777733);
//  //  gc.fill_rect(dirty_rect);
//
//  //  if (isDarken()) {
//  //    g.beginTransparencyLayer(0.5f);
//  //  }
//  //
//  //  auto create_default_stroke = [](float line_thickness) {
//  //    return juce::PathStrokeType(
//  //        line_thickness, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
//  //  };
//  //
//  //  if constexpr (is_highlightable) {
//  //    if (highlight_base::is_highlighted) {
//  //      g.setColour(gfx::to_color(0xFFC60855));
//  //      gfx::frect r = knob_rect.get_exterior_rect({ 2.0f, 2.0f });
//  //      g.drawEllipse(gfx::to_rect(r), 1.3f);
//  //    }
//  //  }
//  //
//  //  if constexpr (has_background_image) {
//  //    //      g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
//  //    g.drawImage(background_image_base::background_image,
//  //        juce::Rectangle<float>(knob_rect.x, knob_rect.y, knob_rect.width, knob_rect.height));
//  //  }
//
//  if (_style.background_color.alpha() != 0) {
//    gc.set_fill_color(_style.background_color);
//    gc.fill_ellipse(_knob_rect);
//
//    if (_style.interior_contour_color.alpha() != 0 && _style.interior_contour_width) {
//      gc.set_stroke_color(_style.interior_contour_color);
//      gc.set_line_width(_style.interior_contour_width);
//      gc.stroke_ellipse(_knob_rect.reduced({ 2, 2 }));
//
//      //        g.drawEllipse(knob_rect.x + 2, knob_rect.y + 2, knob_rect.width - 4, knob_rect.height - 4,
//      //            style::interior_contour_width);
//
//      //      if constexpr (has_interior_contour_width) {
//      //        g.drawEllipse(knob_rect.x + 2, knob_rect.y + 2, knob_rect.width - 4, knob_rect.height - 4,
//      //            style::interior_contour_width);
//      //      }
//      //      else {
//      //        g.drawEllipse(knob_rect.x + 2, knob_rect.y + 2, knob_rect.width - 4, knob_rect.height - 4, 1.5f);
//      //      }
//    }
//
//    if (_style.contour_color.alpha() != 0 && _style.contour_width) {
//      gc.set_stroke_color(_style.contour_color);
//      gc.set_line_width(_style.contour_width);
//      gc.stroke_ellipse(_knob_rect);
//    }
//  }
//
//  float angle_range = _style.angle_range.length();
//  //
//  float normalized_value = _normalized_value;
//  //
//  //  if (is_discrete()) {
//  //    normalized_value = std::floor(normalized_value * (_n_steps - 1)) / (float)(_n_steps - 1);
//  //  }
//  //
//  float angle = _style.angle_range.min + normalized_value * angle_range;
//  float cursor_angle = _VDNFT::math::pi<float> - angle;
//  //
//  _VDNFT::fpoint pos = { std::sin(cursor_angle), std::cos(cursor_angle) };
//
//  //  if constexpr (has_cursor) {
//  // Cursor.
//  //      juce::Path cursor;
//  //      gfx::fpoint c_start = knob_center + pos * radius * style::cursor_radius_percent_base;
//  //      gfx::fpoint c_end = knob_center + pos * radius * style::cursor_radius_percent_tip;
//  //      cursor.startNewSubPath(c_start.x, c_start.y);
//  //      cursor.lineTo(c_end.x, c_end.y);
//
//  //  return juce::PathStrokeType(
//  //        line_thickness, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded);
//
//  _VDNFT::fpoint c_start = _knob_center + pos * _knob_radius * _style.cursor_radius_percent_base;
//  _VDNFT::fpoint c_end = _knob_center + pos * _knob_radius * _style.cursor_radius_percent_tip;
//
//  gc.set_stroke_color(_style.cursor_color);
//  gc.set_line_style(_style.cursor_width, _VDNFT::context::line_join::round, _VDNFT::context::line_cap::round);
//  gc.stroke_line(c_start, c_end);
//  //      if constexpr (has_cursor_style) {
//  //        if constexpr (style::cursor_style == gfx::knob_cursor_style::squared) {
//  //          g.strokePath(cursor,
//  //              juce::PathStrokeType(style::cursor_width, juce::PathStrokeType::JointStyle::mitered,
//  //                  juce::PathStrokeType::EndCapStyle::square));
//  //        }
//  //        else {
//  //          g.strokePath(cursor, create_default_stroke(style::cursor_width));
//  //        }
//  //      }
//  //      else {
//  //        g.strokePath(cursor, create_default_stroke(style::cursor_width));
//  //      }
//  //  }
//
//  //
//  //  if constexpr (style::has_ring) {
//  //    constexpr float ring_offset = knob_detail::get_ring_offset<style>();
//  //
//  //    // Ring.
//  //    juce::Path ring;
//  //    if constexpr (style::is_bipolar) {
//  //      constexpr float middle_angle = (style::start_angle + style::end_angle) * 0.5f;
//  //      ring.addCentredArc(knob_center.x, knob_center.y, radius.x + ring_offset, radius.y + ring_offset, 0.0f,
//  //          middle_angle, middle_angle + (normalized_value * 2.0f - 1.0f) * angle_range * 0.5f, true);
//  //    }
//  //    else {
//  //      ring.addCentredArc(knob_center.x, knob_center.y, radius.x + ring_offset, radius.y + ring_offset, 0.0f,
//  //          style::start_angle, angle, true);
//  //    }
//  //
//  //    g.setColour(to_color(style::ring_color));
//  //    g.strokePath(ring, create_default_stroke(style::ring_width));
//  //  }
//  //
//  //  if constexpr (style::is_middle_marked) {
//  //    g.setColour(to_color(style::marker_color));
//  //
//  //    if constexpr (!style::is_marked) {
//  //      constexpr float a = atk::pi<float> - (style::start_angle + 0.5f * angle_range);
//  //      constexpr gfx::fpoint p = { atk::sin(a), atk::cos(a) };
//  //
//  //      constexpr gfx::frect rr(knob_center + p * (radius + 5.0f),
//  //          gfx::fsize(style::marker_middle_diameter, style::marker_middle_diameter));
//  //      constexpr gfx::frect rrr(rr.position - gfx::fpoint(rr.size.to_pair()) * 0.5f, rr.size);
//  //
//  //      if constexpr (style::marker_is_round) {
//  //        g.fillEllipse(rrr.x, rrr.y, rrr.width, rrr.height);
//  //      }
//  //      else {
//  //        g.fillRect(rrr.x, rrr.y, rrr.width, rrr.height);
//  //      }
//  //    }
//  //
//  //    if (is_discrete()) {
//  //      if constexpr (style::is_marked) {
//  //        for (int i = 0; i < _n_steps; i++) {
//  //
//  //          float a = atk::pi<float> - (style::start_angle + i / (_n_steps - 1.0) * angle_range);
//  //          gfx::fpoint p = { std::sin(a), std::cos(a) };
//  //          gfx::frect rr(knob_center + p * (radius + 5.0f),
//  //              gfx::fsize(style::marker_middle_diameter, style::marker_middle_diameter));
//  //          gfx::frect rrr(rr.position - gfx::fpoint(rr.size.to_pair()) * 0.5f, rr.size);
//  //
//  //          if constexpr (style::marker_is_round) {
//  //            g.fillEllipse(rrr.x, rrr.y, rrr.width, rrr.height);
//  //          }
//  //          else {
//  //            g.fillRect(rrr.x, rrr.y, rrr.width, rrr.height);
//  //          }
//  //        }
//  //      }
//  //    }
//  //  }
//  //
//  //  if constexpr (has_cursor) {
//  //    // Cursor.
//  //    juce::Path cursor;
//  //    gfx::fpoint c_start = knob_center + pos * radius * style::cursor_radius_percent_base;
//  //    gfx::fpoint c_end = knob_center + pos * radius * style::cursor_radius_percent_tip;
//  //    cursor.startNewSubPath(c_start.x, c_start.y);
//  //    cursor.lineTo(c_end.x, c_end.y);
//  //
//  //    g.setColour(to_color(style::cursor_color));
//  //
//  //    if constexpr (has_cursor_style) {
//  //      if constexpr (style::cursor_style == gfx::knob_cursor_style::squared) {
//  //        g.strokePath(cursor,
//  //            juce::PathStrokeType(style::cursor_width, juce::PathStrokeType::JointStyle::mitered,
//  //                juce::PathStrokeType::EndCapStyle::square));
//  //      }
//  //      else {
//  //        g.strokePath(cursor, create_default_stroke(style::cursor_width));
//  //      }
//  //    }
//  //    else {
//  //      g.strokePath(cursor, create_default_stroke(style::cursor_width));
//  //    }
//  //  }
//  //
//  //  if (isDarken()) {
//  //    g.endTransparencyLayer();
//  //    //      g.restoreState();
//  //  }
//}
//
// DNFT_END_NAMESPACE
