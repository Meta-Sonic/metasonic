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
//#include "dragon-fruit/core/range.h"
//#include "dragon-fruit/graphics/text_layout.h"
//
// DNFT_BEGIN_NAMESPACE
// class text_box : public _VDNFT::view {
// public:
//  text_box();
//
//  virtual ~text_box() override;
//
//  void set_text(std::string_view text);
//  void set_text(std::u16string_view text);
//  void set_text(const std::u32string& text);
//  void set_text(std::u32string_view text);
//
//  std::string get_text() const;
//  std::string get_text_utf8() const;
//  std::u16string get_text_utf16() const;
//  const std::u32string& get_text_utf32() const;
//
//  inline std::size_t get_text_size() const noexcept { return get_text_utf32().size(); }
//
//  void set_font(const _VDNFT::font& f);
//
//  void set_placeholder(std::string_view text);
//  void set_placeholder(std::u16string_view text);
//  void set_placeholder(const std::u32string& text);
//  void set_placeholder(std::u32string_view text);
//
//  inline bool is_focused() const noexcept { return _flags.is_focused; }
//  inline bool is_selected() const noexcept { return _selection_indexes.length(); }
//  inline bool is_carret_visible() const noexcept { return is_focused() && !is_selected(); }
//
//  void set_carret_index(long x);
//  void move_carret(long dx);
//  void select_all();
//
// protected:
//  virtual void bounds_changed(const _VDNFT::view::bounds_status& bs) override;
//  virtual void mouse_right_down(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_down(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_drag(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_left_up(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_enter(const _VDNFT::mouse_event& evt) override;
//  virtual void mouse_leave() override;
//  virtual bool key_pressed(const _VDNFT::key_event& kevt) override;
//  virtual void focus_gained() override;
//  virtual void focus_lost() override;
//  virtual void paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) override;
//
// private:
//  _VDNFT::path _bg_path;
//  _VDNFT::tl::line _line;
//  std::unique_ptr<_VDNFT::tl::line> _placeholder;
//
//  _VDNFT::frect _text_frame;
//  _VDNFT::fpoint _scroll_offset = { 0, 0 };
//
//  _VDNFT::fpoint _last_mouse_position;
//  _VDNFT::fpoint _line_offset;
//
//  _VDNFT::range<long> _selection_indexes = { 0, 0 };
//  _VDNFT::range<float> _selection_positions = { 0, 0 };
//
//  long _carret_index = 0;
//  float _carret_pos = 0;
//  float _line_width = 0;
//
//  _VDNFT::tl::alignment _alignment = _VDNFT::tl::alignment::left_centered();
//  _VDNFT::tl::alignment _current_alignment = _VDNFT::tl::alignment::left_centered();
//
//  struct flags {
//    bool is_focused : 1 = false;
//    bool is_down : 1 = false;
//    bool is_dragging : 1 = false;
//  };
//
//  flags _flags;
//  std::uint8_t _status = 0;
//
//  class logic;
//  logic* get_logic();
//  const logic* get_logic() const;
//};
//
// DNFT_END_NAMESPACE
