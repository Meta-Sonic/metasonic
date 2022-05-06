/////
///// Copyright (C) 2021, Meta-Sonic
///// All rights reserved.
/////
///// Proprietary and confidential.
///// Any unauthorized copying, alteration, distribution, transmission,
///// performance, display or other use of this material is strictly prohibited.
/////
///// Written by Alexandre Arsenault <alx.arsenault@gmail.com>
/////
//
//#pragma once
//#include "dragon-fruit/gui/view.h"
//#include "dragon-fruit/gui/popup_window.h"
//#include "dragon-fruit/controls/popup_value.h"
//
//// GFX_HAS_PROPERTY(background_image);
//// GFX_HAS_PROPERTY(background_color);
//// GFX_HAS_PROPERTY(contour_color);
//// GFX_HAS_PROPERTY(contour_width);
//// GFX_HAS_PROPERTY(interior_contour_color);
//// GFX_HAS_PROPERTY(interior_contour_width);
//// GFX_HAS_PROPERTY(ring_offset);
//// GFX_HAS_PROPERTY(cursor_style);
//// GFX_HAS_PROPERTY(is_highlightable);
//
//// static constexpr bool has_background_image = knob_detail::has_background_image<_Style>::value;
//// static constexpr bool has_background_color = knob_detail::has_background_color<_Style>::value;
//// static constexpr bool has_contour_color = knob_detail::has_contour_color<_Style>::value;
//// static constexpr bool has_contour_width = knob_detail::has_contour_width<_Style>::value;
//// static constexpr bool has_interior_contour_color = knob_detail::has_interior_contour_color<_Style>::value;
//// static constexpr bool has_interior_contour_width = knob_detail::has_interior_contour_width<_Style>::value;
//// static constexpr bool has_cursor = knob_detail::has_cursor<_Style>::value;
//// static constexpr bool has_cursor_style = knob_detail::has_cursor_style<_Style>::value;
//// static constexpr bool has_click_anywhere = knob_detail::has_click_anywhere<_Style>::value;
//// static constexpr bool is_highlightable = knob_detail::has_is_highlightable<_Style>::value;
//
//// icxpr gfx::color knob_background_color = 0x6E6E6E55;
//// icxpr gfx::color knob_contour_color = 0x131313FF;
//// icxpr gfx::color knob_interior_contour_color = 0x7B7B7B99;
//// icxpr gfx::color knob_background_ring_color = 0x808080FF;
//// icxpr gfx::color knob_ring_color = 0x888888FF;
////
//// template <bool _HasRing = false, bool _IsBipolar = false, typename _PopupValueStyle = popup_value_style>
//// struct knob_style_base {
////   using popop_value_style = _PopupValueStyle;
////   scxpr gfx::fpadding padding = { 4.0f, 4.0f, 4.0f, 4.0f };
////   scxpr gfx::color background_color = knob_background_color;
////   scxpr gfx::color contour_color = knob_contour_color;
////   scxpr gfx::color interior_contour_color = knob_interior_contour_color;
////   scxpr gfx::color background_ring_color = knob_background_ring_color;
////   scxpr gfx::color marker_color = 0x979797FF;
////   scxpr gfx::color ring_color = knob_ring_color;
////   scxpr gfx::color cursor_color = 0xDDDDDDFF;
////   scxpr gfx::size knob_size = { 25, 25 };
////   scxpr float start_angle = atk::degree_to_radian(-140.0f);
////   scxpr float end_angle = atk::degree_to_radian(140.0f);
////   scxpr float ring_width = 1.5f;
////   scxpr float ring_offset = 2.5f;
////   scxpr float cursor_width = 1.5f;
////   scxpr float cursor_radius_percent_base = 0.3f;
////   scxpr float cursor_radius_percent_tip = 0.7f;
////   scxpr float marker_middle_diameter = 2.0f;
////   scxpr bool marker_is_round = true;
////   scxpr bool is_bipolar = _IsBipolar;
////   scxpr bool is_middle_marked = _IsBipolar;
////   scxpr bool is_marked = _IsBipolar;
////   scxpr bool has_ring = _HasRing;
////   scxpr bool is_highlightable = true;
//// };
//
// DNFT_BEGIN_NAMESPACE
//
// struct knob_attributes {
//  _VDNFT::color background_color = 0x6E6E6EFF;
//  _VDNFT::color contour_color = 0x131313FF;
//  _VDNFT::color interior_contour_color = 0x7B7B7B99;
//  _VDNFT::color background_ring_color = 0x808080FF;
//  _VDNFT::color ring_color = 0x888888FF;
//  _VDNFT::color cursor_color = 0xFFFFFFFF;
//  _VDNFT::fsize knob_size = { 30.0f, 30.0f };
//  _VDNFT::fpadding padding = { 5, 5, 5, 5 };
//  _VDNFT::frange angle_range = { _VDNFT::math::degree_to_radian(-140.0f), _VDNFT::math::degree_to_radian(140.0f) };
//
//  float contour_width = 1.5f;
//  float interior_contour_width = 1.5f;
//  float cursor_width = 2.0f;
//  float cursor_radius_percent_base = 0.25f;
//  float cursor_radius_percent_tip = 0.65f;
//};
//
// class knob : public _VDNFT::view {
// public:
//  knob();
//
//  virtual ~knob() override;
//
//  void set_normalized_value(float value);
//
// protected:
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void mouse_left_down(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_drag(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_up(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_enter(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_leave() override;
//  virtual void mouse_wheel(const _VDNFT::mouse_event& evt, const _VDNFT::scroll_event& sevt) override;
//
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  std::string _text_value;
//  float _normalized_value = 0;
//
//  float _last_mouse_y = 0.0f;
//  bool _knob_dragging = false;
//  bool _is_discrete = false;
//  int _n_steps = 0;
//  std::size_t _default_value_index = 0;
//
//  _VDNFT::frect _knob_rect;
//  _VDNFT::fpoint _knob_center;
//  float _knob_radius;
//
//  //  static constexpr gfx::frect knob_rect
//  //      = { style::padding.left, style::padding.top, style::knob_size.width, style::knob_size.height };
//  //  static constexpr gfx::fpoint knob_center
//  //      = gfx::frect(style::padding.left, style::padding.top, style::knob_size.width,
//  //      style::knob_size.width).middle();
//  //  static constexpr float knob_radius = style::knob_size.width * 0.5f;
//
//  knob_attributes _style;
//
//  _VDNFT::popup_value _popup_value;
//
//  //  _VDNFT::weak_managed_ptr<_VDNFT::popup_window> _popup_value;
//  //  dnft::managed_ptr<_VDNFT::popup_window> _popup_value;
//
//  //  std::unique_ptr<popup_value_type> _popup_value;
//  //  std::unique_ptr<text_box_type> _text_box;
//  //  std::function<void(juce::PopupMenu&)> _right_click_callback;
//  //  bool _show_popup_value_on_left_size = false;
//};
// DNFT_END_NAMESPACE
