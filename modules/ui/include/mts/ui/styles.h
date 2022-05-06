///
/// BSD 3-Clause License
///
/// Copyright (c) 2022, Alexandre Arsenault
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// * Redistributions of source code must retain the above copyright notice, this
///   list of conditions and the following disclaimer.
///
/// * Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// * Neither the name of the copyright holder nor the names of its
///   contributors may be used to endorse or promote products derived from
///   this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#pragma once
#include "mts/config.h"
#include "mts/traits.h"
#include "mts/graphics/color.h"
#include "mts/graphics/rect.h"
#include "mts/graphics/alignment.h"
#include "mts/ui/style_traits.h"

MTS_BEGIN_NAMESPACE

enum class global_attributes {
  background_color,
  primary_color,
  primary_variant_color,
  secondary_color,
  secondary_variant_color,
  text_color,
  font_name
};

enum class styled_view_attributes {
  background_color,
  hover_background_color,
  pressed_background_color,
  text_color,
  hover_text_color,
  pressed_text_color,
  border_color,
  border_radius,
  border_width,
  text_alignment,
  text_offset,
  font_name
};

enum class button_attributes {
  background_color,
  hover_background_color,
  pressed_background_color,
  text_color,
  hover_text_color,
  pressed_text_color,
  border_color,
  border_radius,
  border_width,
  font_name
};

enum class toggle_attributes {
  background_color,
  hover_background_color,
  pressed_background_color,
  text_color,
  hover_text_color,
  pressed_text_color,
  border_color,
  border_radius,
  border_width,
  font_name
};

enum class combo_box_attributes {
  background_color,
  text_color,
  border_color,
  shadow_color,
  border_radius,
  border_width,
  font_name
};

template <>
struct is_style_enum<global_attributes> : std::true_type {};

template <>
struct is_style_enum<styled_view_attributes> : std::true_type {};

template <>
struct is_style_enum<button_attributes> : std::true_type {};

template <>
struct is_style_enum<toggle_attributes> : std::true_type {};

template <>
struct is_style_enum<combo_box_attributes> : std::true_type {};

//
// MARK: Styles.
//

#define ATTRIBUTE(NAME, TYPE) declare_attribute<ATTRIBUTE_ENUM::NAME, TYPE, #NAME>

///
/// Global.
///
#undef ATTRIBUTE_ENUM
#define ATTRIBUTE_ENUM global_attributes
struct global_style : style_descriptor<"global", //
                          ATTRIBUTE(background_color, mts::color), //
                          ATTRIBUTE(primary_color, mts::color), //
                          ATTRIBUTE(primary_variant_color, mts::color), //
                          ATTRIBUTE(secondary_color, mts::color), //
                          ATTRIBUTE(secondary_variant_color, mts::color), //
                          ATTRIBUTE(text_color, mts::color), //
                          ATTRIBUTE(font_name, std::string)> {

  inline global_style()
      : style_descriptor({
          0xEBEBEBFF, // background_color.
          0xFFFFFFFF, // primary_color
          0xFFFFFFFF, // primary_variant_color
          0xFFFFFFFF, // secondary_color
          0xFFFFFFFF, // secondary_variant_color
          0x000000FF, // text_color.
          "Roboto" // font_name.
      }) {}
};

///
/// Styled view.
///
#undef ATTRIBUTE_ENUM
#define ATTRIBUTE_ENUM styled_view_attributes
struct styled_view_style : style_descriptor<"style_view", //
                               ATTRIBUTE(background_color, mts::color), //
                               ATTRIBUTE(hover_background_color, mts::color), //
                               ATTRIBUTE(pressed_background_color, mts::color), //
                               ATTRIBUTE(text_color, mts::color), //
                               ATTRIBUTE(hover_text_color, mts::color), //
                               ATTRIBUTE(pressed_text_color, mts::color), //
                               ATTRIBUTE(border_color, mts::color), //
                               ATTRIBUTE(border_radius, float), //
                               ATTRIBUTE(border_width, float), //
                               ATTRIBUTE(text_alignment, mts::alignment), //
                               ATTRIBUTE(text_offset, mts::point<float>), //
                               ATTRIBUTE(font_name, std::string)> {

  inline styled_view_style()
      : style_descriptor({
          mts::color(0x444444FF), // background_color
          mts::color(0x444444FF), // hover_background_color
          mts::color(0x444444FF), // pressed_background_color
          0xFFFFFFFF, // text_color
          0xFFFFFFFF, // hover_text_color
          0xFFFFFFFF, // pressed_text_color
          0x000000FF, // border_color
          0.0f, // border_radius
          0.5f, // border_width
          mts::alignment(), mts::point<float>(0, 0),
          "Roboto" // font_name
      }) {}
};

///
/// Button.
///
#undef ATTRIBUTE_ENUM
#define ATTRIBUTE_ENUM button_attributes
struct button_style : style_descriptor<"button", //
                          ATTRIBUTE(background_color, mts::color), //
                          ATTRIBUTE(hover_background_color, mts::color), //
                          ATTRIBUTE(pressed_background_color, mts::color), //
                          ATTRIBUTE(text_color, mts::color), //
                          ATTRIBUTE(hover_text_color, mts::color), //
                          ATTRIBUTE(pressed_text_color, mts::color), //
                          ATTRIBUTE(border_color, mts::color), //
                          ATTRIBUTE(border_radius, float), //
                          ATTRIBUTE(border_width, float), //
                          ATTRIBUTE(font_name, std::string)> {

  inline button_style()
      : style_descriptor({
          mts::color(0x444444FF), // background_color
          mts::color(0x444444FF).brighter(0.2f), // hover_background_color
          mts::color(0x444444FF).darker(0.2f), // pressed_background_color
          0xFFFFFFFF, // text_color
          0xFFFFFFFF, // hover_text_color
          0xFFFFFFFF, // pressed_text_color
          0x000000FF, // border_color
          5.0f, // border_radius
          0.5f, // border_width
          "Roboto" // font_name
      }) {}
};

///
/// Toggle.
///
#undef ATTRIBUTE_ENUM
#define ATTRIBUTE_ENUM toggle_attributes
struct toggle_style : style_descriptor<"toggle", //
                          ATTRIBUTE(background_color, mts::color), //
                          ATTRIBUTE(hover_background_color, mts::color), //
                          ATTRIBUTE(pressed_background_color, mts::color), //
                          ATTRIBUTE(text_color, mts::color), //
                          ATTRIBUTE(hover_text_color, mts::color), //
                          ATTRIBUTE(pressed_text_color, mts::color), //
                          ATTRIBUTE(border_color, mts::color), //
                          ATTRIBUTE(border_radius, float), //
                          ATTRIBUTE(border_width, float), //
                          ATTRIBUTE(font_name, std::string)> {

  inline toggle_style()
      : style_descriptor({
          mts::color(0x444444FF), // background_color
          mts::color(0x444444FF).brighter(0.2f), // hover_background_color
          mts::color(0x444444FF).darker(0.2f), // pressed_background_color
          0xFFFFFFFF, // text_color
          0xFFFFFFFF, // hover_text_color
          0xFFFFFFFF, // pressed_text_color
          0x000000FF, // border_color
          5.0f, // border_radius
          0.5f, // border_width
          "Roboto" // font_name
      }) {}
};

///
/// Combo box.
///
#undef ATTRIBUTE_ENUM
#define ATTRIBUTE_ENUM combo_box_attributes
struct combo_box_style : style_descriptor<"combo_box", //
                             ATTRIBUTE(background_color, mts::color), //
                             ATTRIBUTE(text_color, mts::color), //
                             ATTRIBUTE(border_color, mts::color), //
                             ATTRIBUTE(shadow_color, mts::color), //
                             ATTRIBUTE(border_radius, float), //
                             ATTRIBUTE(border_width, float), //
                             ATTRIBUTE(font_name, std::string)> {

  inline combo_box_style()
      : style_descriptor({
          0xFFFFFFFF, // background_color
          0x262626FF, // text_color
          0xD3D4D4FF, // border_color
          0x0000003C, // shadow_color
          5.0f, // border_radius
          0.5f, // border_width
          "roboto-regular-12" // font_name
      }) {}
};

#undef ATTRIBUTE_ENUM
#undef ATTRIBUTE

MTS_END_NAMESPACE
