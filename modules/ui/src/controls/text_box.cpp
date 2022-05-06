//#include "dragon-fruit/controls/text_box.h"
//#include "dragon-fruit/controls/popup_menu.h"
//#include "dragon-fruit/core/final_action.h"
////#include <unordered_set>
//
// DNFT_BEGIN_NAMESPACE
//
// class text_box::logic : public _VDNFT::text_box {
// public:
//  friend class text_box;
//
//  class update_status {
//  public:
//    inline void update() noexcept { _carret = _text = _selection = true; }
//    inline void update_carret() noexcept { _carret = true; }
//    inline void update_text() noexcept { _text = true; }
//    inline void update_selection() noexcept { _selection = true; }
//
//    explicit inline operator bool() const noexcept { return _carret || _text || _selection; }
//    inline bool carret_changed() const noexcept { return _carret; }
//    inline bool text_changed() const noexcept { return _text; }
//    inline bool selection_changed() const noexcept { return _selection; }
//    //    inline bool carret_changed() const noexcept { return _carret || _text; }
//    //    inline bool text_changed() const noexcept { return _text; }
//    //    inline bool selection_changed() const noexcept { return _selection || _text; }
//
//    inline void reset() noexcept {
//      _carret = false;
//      _text = false;
//      _selection = false;
//    }
//
//    inline void reset_carret() noexcept { _carret = false; }
//
//  private:
//    bool _carret : 1 = false;
//    bool _text : 1 = false;
//    bool _selection : 1 = false;
//  };
//
//  inline update_status& get_status() { return *(update_status*)&_status; }
//
//  inline const update_status& get_status() const { return *(const update_status*)&_status; }
//
//  //  inline void on_line_changed() {
//  //    get_status().update_line();
//  //  }
//  //
//  //  inline float get_carret_position() const {
//  //    return _carret_pos;
//  //  }
//  //
//
//  inline void update_line_offset() {
//    _line_offset
//        = _VDNFT::fpoint(_line.get_aligned_position(_text_frame.with_position({ 0, 0 }), _current_alignment).x, 0);
//    ;
//  }
//  inline _VDNFT::fpoint get_line_offset() const { return _line_offset; }
//
//  inline _VDNFT::fpoint relative_to_line(const _VDNFT::fpoint& pos) const {
//    return pos - _scroll_offset;
//    //    return pos - (_line.get_aligned_position(_text_frame, _current_alignment) + _text_frame.position) -
//    //    _scroll_offset;
//  }
//  //  _VDNFT::fpoint mpos = evt.get_position() - _scroll_offset;
//  //  _VDNFT::fpoint mpos = _text_frame.middle() - _scroll_offset;
//
//  inline float get_maximum_offset() const { return _text_frame.width - _line_width; }
//
//  //  inline float get_maximum_offset() const { return _text_frame.width - (_line_width - get_line_offset().x); }
//
//  inline bool has_overflow() const { return get_maximum_offset() < 0; }
//
//  inline void clip_scroll_offset() {
//    const float max_offset = get_maximum_offset();
//
//    if (max_offset >= 0) {
//      _scroll_offset.x = 0;
//    }
//    else if (_scroll_offset.x < max_offset) {
//      _scroll_offset.x = max_offset;
//    }
//  }
//
//  _VDNFT::frect get_selection_frame() const { return _text_frame.reduced_vertically(3) + get_line_offset(); }
//
//  //  _VDNFT::frect get_selection_frame() const {
//  //    return _text_frame.reduced_vertically(3); // - _offset;
//  //  }
//
//  _VDNFT::frect get_selection_rect() const {
//    _VDNFT::frect sframe = get_selection_frame() + _scroll_offset;
//    return _VDNFT::frect(
//        sframe.x + _selection_positions.rmin(), sframe.y, _selection_positions.rlength(), sframe.height)
//        .rounded();
//  }
//
//  _VDNFT::frect get_carret_rect() const {
//    _VDNFT::frect sframe = get_selection_frame() + _scroll_offset;
//    return _VDNFT::frect(sframe.x + _carret_pos, sframe.y, 1.0f, sframe.height).rounded();
//  }
//
//  _VDNFT::frect get_text_rect() const { return _text_frame + _scroll_offset; }
//
//  inline void update_line_width() {
//    _line_width = _line.get_width();
//
//    if (_current_alignment != _alignment && has_overflow()) {
//      _current_alignment = _VDNFT::tl::alignment::left_centered();
//    }
//    else {
//      _current_alignment = _alignment;
//    }
//  }
//  //  inline void imp_insert_char(char32_t c) {
//  ////    imp_erase_selection(false);
//  //
//  //    if (is_selected()) {
//  //      _line.modify_text(
//  //          [this](std::u32string& str) {
//  //            str.erase(str.begin() + _selection_indexes.min, str.begin() + _selection_indexes.max);
//  //          },
//  //          false);
//  //
//  //      get_status().update_text();
//  //
//  //      _carret_index = _selection_indexes.min;
//  //      _selection_indexes = _VDNFT::range<long>{ 0, 0 };
//  //
//  //    }
//  //
//  //
//  //
//  //
//  //    _line.modify_text([this, c](std::u32string& str) { str.insert(_carret_index, 1, c); });
//  ////    _line_width = _line.get_width();
//  //
//  //    update_line_width();
//  //    imp_move_carret(1, false);
//  //
//  //    get_status().update_text();
//  //
//  //    repaint();
//  //  }
//
//  inline void imp_insert_char(char32_t c) {
//    imp_erase_selection(false);
//
//    _line.modify_text([this, c](std::u32string& str) { str.insert(_carret_index, 1, c); });
//
//    update_line_width();
//    imp_move_carret(1, false);
//
//    get_status().update_text();
//
//    repaint();
//  }
//
//  inline bool imp_erase_selection(bool do_update) {
//    if (!is_selected()) {
//      return false;
//    }
//
//    _line.modify_text(
//        [this](std::u32string& str) {
//          str.erase(str.begin() + _selection_indexes.min, str.begin() + _selection_indexes.max);
//        },
//        do_update);
//
//    get_status().update_text();
//    imp_set_carret_index(_selection_indexes.min, false);
//
//    if (do_update) {
//      update_line_width();
//      repaint();
//    }
//    return true;
//  }
//
//  inline void imp_erase(bool do_update) {
//    _line.modify_text([this](std::u32string& str) { str.erase(str.begin() + _carret_index); }, do_update);
//
//    get_status().update_text();
//
//    if (do_update) {
//      update_line_width();
//      repaint();
//    }
//  }
//
//  inline void imp_delete() {
//    if (imp_erase_selection(true)) {
//      return;
//    }
//
//    if (_carret_index >= get_text_size()) {
//      return;
//    }
//
//    imp_erase(true);
//  }
//
//  inline void imp_backspace() {
//    if (imp_erase_selection(true)) {
//      return;
//    }
//
//    if (_carret_index == 0) {
//      return;
//    }
//
//    imp_move_carret(-1, false);
//    imp_erase(true);
//  }
//
//  inline void imp_set_carret_index(long x, bool do_repaint) {
//    bool needs_repaint = false;
//    if (_VDNFT::assign(_carret_index, std::clamp(x, 0L, (long)get_text_size()))) {
//      get_status().update_carret();
//      needs_repaint = true;
//    }
//
//    if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ 0, 0 })) {
//      needs_repaint = true;
//    }
//
//    if (do_repaint && needs_repaint) {
//      repaint();
//    }
//  }
//
//  inline void imp_move_carret(long dx, bool do_repaint) {
//    if (_VDNFT::assign(_carret_index, std::clamp(_carret_index + dx, 0L, (long)get_text_size()))) {
//      get_status().update_carret();
//
//      _VDNFT::final_action repaint_action = [this, do_repaint]() {
//        if (do_repaint) {
//          repaint();
//        }
//      };
//
//      const float max_offset = get_maximum_offset();
//
//      // No scrolling required if line width fits in text frame.
//      if (max_offset >= 0) {
//        return;
//      }
//
//      // Going right.
//      if (dx > 0) {
//        // Can't scroll anymore to the right.
//        if (_scroll_offset.x == max_offset) {
//          return;
//        }
//
//        _carret_pos = _line.get_offset_for_char_index(_carret_index);
//        get_status().reset_carret();
//
//        if (_carret_pos + _scroll_offset.x > _text_frame.width) {
//          _VDNFT::fpoint mpos = _text_frame.middle() - _scroll_offset;
//          const std::size_t index = _line.get_char_index_at_position(mpos, _text_frame, _current_alignment);
//
//          if (index == _VDNFT::tl::line::invalid_index) {
//            return;
//          }
//
//          long mid_char_index = std::clamp((long)index, 0L, (long)get_text_size());
//          const float mid_char_offset = _line.get_offset_for_char_index(mid_char_index);
//          _scroll_offset.x = _VDNFT::maximum(-mid_char_offset, max_offset);
//        }
//        return;
//      }
//
//      //
//      // Going left.
//      //
//
//      // Can't scroll anymore to the left.
//      if (_scroll_offset.x == 0) {
//        return;
//      }
//
//      _carret_pos = _line.get_offset_for_char_index(_carret_index);
//      get_status().reset_carret();
//
//      if (_scroll_offset.x + _carret_pos < 0) {
//        _VDNFT::fpoint mpos = _text_frame.middle() - _scroll_offset - _VDNFT::fpoint{ _text_frame.width, 0 };
//        const std::size_t index = _line.get_char_index_at_position(mpos, _text_frame, _current_alignment);
//
//        if (index == _VDNFT::tl::line::invalid_index) {
//          return;
//        }
//
//        long mid_char_index = std::clamp((long)index, 0L, (long)get_text_size());
//        const float mid_char_offset = _line.get_offset_for_char_index(mid_char_index);
//        _scroll_offset.x = _VDNFT::maximum(-mid_char_offset, max_offset);
//      }
//    }
//  }
//
//  inline void imp_reveal_carret() {
//    const float max_offset = get_maximum_offset();
//
//    // No scrolling required if line width fits in text frame.
//    if (max_offset >= 0) {
//      return;
//    }
//
//    _carret_pos = _line.get_offset_for_char_index(_carret_index);
//    get_status().reset_carret();
//
//    // Check if carrent is already shown.
//    if (_text_frame.intersect(_scroll_offset + _VDNFT::fpoint{ _carret_pos, 0 })) {
//      return;
//    }
//
//    if (_carret_pos + _scroll_offset.x > _text_frame.width) {
//      _scroll_offset.x = _VDNFT::maximum(-_carret_pos, max_offset);
//      return;
//    }
//
//    //   Going left.
//
//    // Can't scroll anymore to the left.
//    if (_scroll_offset.x == 0) {
//      return;
//    }
//
//    _scroll_offset.x = _VDNFT::minimum(-_carret_pos, 0);
//  }
//
//  inline void imp_arrow_left(_VDNFT::key_modifiers mods) {
//    bool shift_down = mods.is_shift_down();
//
//    if (is_selected()) {
//      if (shift_down) {
//        //        long smax = _selection_indexes.max;
//        //        imp_set_carret_index(_selection_indexes.min - 1, true);
//        //        _selection_indexes = _VDNFT::range<long>{ _carret_index, smax };
//        //        imp_reveal_carret();
//
//        imp_move_carret(_selection_indexes.min - _carret_index - 1, true);
//        _selection_indexes.min = _carret_index;
//        imp_reveal_carret();
//
//        get_status().update_selection();
//        return;
//      }
//
//      imp_set_carret_index(_selection_indexes.min, true);
//      imp_reveal_carret();
//      return;
//    }
//
//    if (shift_down) {
//      //        long smax = _carret_index;
//      //        imp_set_carret_index(_selection_indexes.min - 1, true);
//      //        _selection_indexes = _VDNFT::range<long>{_carret_index, smax};
//      //        imp_reveal_carret();
//      _selection_indexes.max = _carret_index;
//      imp_move_carret(-1, true);
//      _selection_indexes.min = _carret_index;
//      get_status().update_selection();
//      return;
//    }
//
//    imp_move_carret(-1, true);
//  }
//
//  inline void imp_arrow_right(_VDNFT::key_modifiers mods) {
//    bool shift_down = mods.is_shift_down();
//
//    if (is_selected()) {
//      if (shift_down) {
//        //        long smin = _selection_indexes.min;
//        //        imp_set_carret_index(_selection_indexes.max + 1, true);
//        //        _selection_indexes = _VDNFT::range<long>{ smin, _carret_index };
//        //        imp_reveal_carret();
//
//        imp_move_carret(_selection_indexes.max - _carret_index + 1, true);
//        _selection_indexes.max = _carret_index;
//
//        imp_reveal_carret();
//
//        //        imp_move_carret(1, true);
//        //        _selection_indexes.max = _carret_index;
//        get_status().update_selection();
//        return;
//      }
//
//      imp_set_carret_index(_selection_indexes.max, true);
//      imp_reveal_carret();
//      return;
//    }
//
//    if (shift_down) {
//      _selection_indexes.min = _carret_index;
//      imp_move_carret(1, true);
//      _selection_indexes.max = _carret_index;
//      get_status().update_selection();
//      return;
//    }
//
//    imp_move_carret(1, true);
//  }
//  //  inline void imp_arrow_left(_VDNFT::key_event::modifier mods) {
//  //    bool shift_down = _VDNFT::has_flag(_VDNFT::key_event::modifier::shift, mods);
//  //
//  //    if (shift_down) {
//  //
//  //      imp_set_carret_index(_selection_indexes.min - 1, true);
//  //      _selection_indexes.min = _carret_index;
//  //      imp_reveal_carret();
//  //
//  //
//  ////      if (!is_selected()) {
//  ////        _selection_indexes.max = _carret_index;
//  ////      }
//  ////      imp_move_carret(-1, true);
//  ////      _selection_indexes.min = _carret_index;
//  //      get_status().update_selection();
//  //      return;
//  //    }
//  //
//  //    if (is_selected()) {
//  //      imp_set_carret_index(_selection_indexes.min, true);
//  //      imp_reveal_carret();
//  //      return;
//  //    }
//  //
//  //    imp_move_carret(-1, true);
//  //  }
//  //  inline void imp_arrow_right(_VDNFT::key_event::modifier mods) {
//  //    bool shift_down = _VDNFT::has_flag(_VDNFT::key_event::modifier::shift, mods);
//  //
//  //    if (is_selected()) {
//  //      if (shift_down) {
//  //        imp_move_carret(1, true);
//  //        _selection_indexes.max = _carret_index;
//  //        get_status().update_selection();
//  //        return;
//  //      }
//  //
//  //      imp_set_carret_index(_selection_indexes.max, true);
//  //      imp_reveal_carret();
//  //      return;
//  //    }
//  //
//  //    if (shift_down) {
//  //      _selection_indexes.min = _carret_index;
//  //      imp_move_carret(1, true);
//  //      _selection_indexes.max = _carret_index;
//  //      get_status().update_selection();
//  //      return;
//  //    }
//  //
//  //    imp_move_carret(1, true);
//  //  }
//  inline void imp_arrow(_VDNFT::key_event::arrow_direction dir, _VDNFT::key_modifiers mods) {
//    using ad = _VDNFT::key_event::arrow_direction;
//    switch (dir) {
//    case ad::left:
//      imp_arrow_left(mods);
//      break;
//    case ad::right:
//      imp_arrow_right(mods);
//      break;
//    case ad::up:
//      if (is_selected()) {
//        imp_scroll_left();
//        return imp_set_carret_index(0, true);
//      }
//      else {
//        imp_scroll_left();
//        imp_set_carret_index(0, true);
//      }
//
//      break;
//    case ad::down:
//      if (is_selected()) {
//        imp_scroll_right();
//        return imp_set_carret_index(get_text_size(), true);
//      }
//      else {
//        imp_scroll_right();
//        imp_set_carret_index(get_text_size(), true);
//      }
//
//      break;
//    }
//  }
//
//  //  inline void imp_arrow(_VDNFT::key_event::arrow_direction dir, _VDNFT::key_event::modifier mods) {
//  //    using ad = _VDNFT::key_event::arrow_direction;
//  //    if (is_selected()) {
//  //      switch (dir) {
//  //      case ad::left:
//  //        imp_set_carret_index(_selection_indexes.min, true);
//  //        imp_reveal_carret();
//  //        return;
//  //
//  //      case ad::right:
//  //        imp_set_carret_index(_selection_indexes.max, true);
//  //        imp_reveal_carret();
//  //        return;
//  //
//  //      case ad::up:
//  //        imp_scroll_left();
//  //        return imp_set_carret_index(0, true);
//  //
//  //      case ad::down:
//  //        imp_scroll_right();
//  //        return imp_set_carret_index(get_text_size(), true);
//  //      }
//  //      return;
//  //    }
//  //
//  //    switch (dir) {
//  //    case ad::left:
//  //      imp_move_carret(-1, true);
//  //      break;
//  //    case ad::right:
//  //      imp_move_carret(1, true);
//  //      break;
//  //    case ad::up:
//  //      imp_scroll_left();
//  //      imp_set_carret_index(0, true);
//  //      break;
//  //    case ad::down:
//  //      imp_scroll_right();
//  //      imp_set_carret_index(get_text_size(), true);
//  //      break;
//  //    }
//  //  }
//
//  inline void imp_scroll_left() { _scroll_offset.x = 0; }
//
//  inline void imp_scroll_right() {
//    const float max_offset = get_maximum_offset();
//
//    // If has overflow.
//    if (max_offset < 0) {
//      _scroll_offset.x = max_offset;
//    }
//  }
//
//  inline std::size_t get_next_space(std::size_t index) {
//    const std::u32string& str = _line.get_text_utf32();
//
//    if (str.empty()) {
//      return 0;
//    }
//
//    index = std::clamp(index, (std::size_t)0, get_text_size() - 1);
//
//    //
//    //    if(str[index] == ' ') {
//    //      std::size_t l_index = 0;
//    //      for(std::size_t i = index; i > 0; i--) {
//    //        if(str[i] != ' ') {
//    //          l_index = i + 1;
//    //          break;
//    //        }
//    //      }
//    //
//    //      std::size_t r_index = str.size();
//    //      for(std::size_t i = index; i < str.size(); i++) {
//    //        if(str[i] != ' ') {
//    //          r_index = i;
//    //          break;
//    //        }
//    //      }
//    //
//    //      if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ (long)l_index, (long)r_index })) {
//    //        get_logic()->get_status().update_selection();
//    //        repaint();
//    //      }
//    //
//    //      return;
//    //    }
//
//    return 0;
//  }
//
//  inline void imp_select_word(const _VDNFT::fpoint& cpos) {
//    const std::u32string& str = _line.get_text_utf32();
//
//    if (str.empty()) {
//      return;
//    }
//
//    std::size_t index = _line.get_char_index_at_position(cpos, _text_frame, _current_alignment);
//    if (index == _VDNFT::tl::line::invalid_index) {
//      dnft::print("INVALID INDEX");
//      return;
//    }
//
//    index = std::clamp(index, (std::size_t)0, get_text_size() - 1);
//    //
//    if (str[index] == ' ') {
//      std::size_t l_index = 0;
//      for (std::size_t i = index; i > 0; i--) {
//        if (str[i] != ' ') {
//          l_index = i + 1;
//          break;
//        }
//      }
//
//      std::size_t r_index = str.size();
//      for (std::size_t i = index; i < str.size(); i++) {
//        if (str[i] != ' ') {
//          r_index = i;
//          break;
//        }
//      }
//
//      if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ (long)l_index, (long)r_index })) {
//        get_logic()->get_status().update_selection();
//        repaint();
//      }
//
//      return;
//    }
//
//    //
//    std::size_t l_index = 0;
//    for (std::size_t i = index; i > 0; i--) {
//      if (str[i] == ' ') {
//        l_index = i + 1;
//        break;
//      }
//    }
//
//    std::size_t r_index = str.size();
//    for (std::size_t i = index; i < str.size(); i++) {
//      if (str[i] == ' ') {
//        r_index = i;
//        break;
//      }
//    }
//
//    if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ (long)l_index, (long)r_index })) {
//      get_logic()->get_status().update_selection();
//      repaint();
//    }
//  }
//};
//
////
////
////
////
////
// text_box::logic* text_box::get_logic() { return static_cast<logic*>(this); }
// const text_box::logic* text_box::get_logic() const { return static_cast<const logic*>(this); }
//
// text_box::text_box() {
//   set_size({ 100, 25 });
//
//   //  dnft::print("text_box::sizeof(update_status)", sizeof(update_status));
//
//   _line.set_font(_VDNFT::get_assets()->fonts.get("roboto-regular-12"));
//   //  _line.set_password(true);
//   set_drawing_margin({ 5, 5, 5, 5 });
// }
//
// text_box::~text_box() {}
//
// void text_box::set_text(std::string_view text) {
//   _line.set_text(text, true);
//   get_logic()->get_status().update_text();
//   repaint();
// }
//
// void text_box::set_text(std::u16string_view text) {
//   _line.set_text(text, true);
//   get_logic()->get_status().update_text();
//   repaint();
// }
//
// void text_box::set_text(const std::u32string& text) {
//   _line.set_text(text, true);
//   get_logic()->get_status().update_text();
//   repaint();
// }
// void text_box::set_text(std::u32string_view text) {
//   _line.set_text(text, true);
//   get_logic()->get_status().update_text();
//   repaint();
// }
//
// std::string text_box::get_text() const { return _line.get_text(); }
//
// std::string text_box::get_text_utf8() const { return _line.get_text_utf8(); }
//
// std::u16string text_box::get_text_utf16() const { return _line.get_text_utf16(); }
//
// const std::u32string& text_box::get_text_utf32() const { return _line.get_text_utf32(); }
//
// void text_box::set_font(const _VDNFT::font& f) {
//   _line.set_font(f);
//   get_logic()->get_status().update_text();
//   repaint();
// }
//
// void text_box::set_placeholder(std::string_view text) {
//   if (text.empty()) {
//     _placeholder.reset();
//     return;
//   }
//
//   if (!_placeholder) {
//     _placeholder = std::make_unique<_VDNFT::tl::line>();
//   }
//
//   _placeholder->set_text(text, true);
//   repaint();
// }
//
// void text_box::set_placeholder(std::u16string_view text) {
//   if (text.empty()) {
//     _placeholder.reset();
//     return;
//   }
//
//   if (!_placeholder) {
//     _placeholder = std::make_unique<_VDNFT::tl::line>();
//   }
//
//   _placeholder->set_text(text, true);
//   repaint();
// }
//
// void text_box::set_placeholder(const std::u32string& text) {
//   if (text.empty()) {
//     _placeholder.reset();
//     return;
//   }
//
//   if (!_placeholder) {
//     _placeholder = std::make_unique<_VDNFT::tl::line>();
//   }
//
//   _placeholder->set_text(text, true);
//   repaint();
// }
//
// void text_box::set_placeholder(std::u32string_view text) {
//   if (text.empty()) {
//     _placeholder.reset();
//     return;
//   }
//
//   if (!_placeholder) {
//     _placeholder = std::make_unique<_VDNFT::tl::line>();
//   }
//
//   _placeholder->set_text(text, true);
//   repaint();
// }
//
// void text_box::set_carret_index(long x) {
//   if (_VDNFT::assign(_carret_index, x)) {
//     get_logic()->get_status().update_carret();
//     repaint();
//   }
//
//   if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ 0, 0 })) {
//     get_logic()->get_status().update_selection();
//     repaint();
//   }
// }
//
// void text_box::move_carret(long dx) {
//   if (_VDNFT::assign(_carret_index, std::clamp(_carret_index + dx, 0L, (long)get_text_size()))) {
//     get_logic()->get_status().update_carret();
//     repaint();
//   }
// }
//
// void text_box::select_all() {
//   bool was_selected = is_selected();
//
//   if (_VDNFT::assign(_selection_indexes, _VDNFT::range<long>{ 0L, (long)get_text_size() })) {
//     if (was_selected != is_selected() || is_selected()) {
//       get_logic()->get_status().update_selection();
//       repaint();
//     }
//   }
// }
//
// bool text_box::key_pressed(const _VDNFT::key_event& kevt) {
//   using ke = _VDNFT::key_event;
//   dnft::print("text_box::key_pressed", kevt.get_key_code(), (int)kevt.get_char(), kevt.get_char());
//
//   ke::key_code kc = kevt.get_key_code();
//
//   if (kevt.is_arrow()) {
//     get_logic()->imp_arrow(kevt.get_arrow_direction(), kevt.get_modifiers());
//     return true;
//   }
//
//   if (kc == ke::escapeKey) {
//     ungrab_focus();
//   }
//   else if (kc == ke::returnKey) {
//     ungrab_focus();
//   }
//   else if (kc == ke::tabKey) {
//     ungrab_focus();
//   }
//   else if (kc == ke::deleteKey) {
//     get_logic()->imp_delete();
//     repaint();
//   }
//   else if (kc == ke::backspaceKey) {
//     get_logic()->imp_backspace();
//   }
//   else if (kevt.get_char() != 0) {
//     get_logic()->imp_insert_char(kevt.get_char());
//     repaint();
//   }
//   return true;
// }
//
// void text_box::focus_gained() {
//   dnft::print("text_box::focus_gained");
//   _flags.is_focused = true;
//   repaint();
// }
//
// void text_box::focus_lost() {
//   dnft::print("text_box::focus_lost");
//
//   _flags.is_focused = false;
//   repaint();
// }
//
// void text_box::mouse_right_down(const _VDNFT::mouse_event& evt) {
//   dnft::popup_menu menu;
//   menu.add_item("Cut", []() { dnft::print("Cut"); });
//   menu.add_item(
//       "Copy", []() { dnft::print("Copy"); }, false, false);
//   menu.add_item("Paste", []() { dnft::print("Paste"); });
//
//   //  menu.add_item("bingo 3", []() { dnft::print("BINFO 3"); });
//   //  menu.add_item("bingo 4", []() { dnft::print("BINFO 4"); });
//   //  menu.add_item("bingo 5", []() { dnft::print("BINFO 5"); });
//   //  menu.add_item(
//   //      "bingo 6", []() { dnft::print("BINFO 6"); }, true);
//   //  menu.add_item("bingo 7", []() { dnft::print("BINFO 7"); });
//
//   menu.show_menu(*this, get_screen_position() + evt.get_position());
// }
//
// void text_box::mouse_left_down(const _VDNFT::mouse_event& evt) {
//
//   if (!_flags.is_focused) {
//     grab_focus();
//   }
//
//   if (evt.get_click_count() == 2) {
//     if (_line.is_password()) {
//       select_all();
//       return;
//     }
//     get_logic()->imp_select_word(evt.get_position() - _scroll_offset);
//     return;
//   }
//
//   if (evt.get_click_count() > 2) {
//     select_all();
//     return;
//   }
//
//   _VDNFT::fpoint mpos = evt.get_position() - _scroll_offset;
//   const std::size_t index = _line.get_char_index_at_position(mpos, _text_frame, _current_alignment);
//   if (index == _VDNFT::tl::line::invalid_index) {
//     dnft::print("INVALID INDEX");
//     return;
//   }
//
//   _last_mouse_position = evt.get_position();
//
//   if (evt.is_shift_down()) {
//
//     _selection_indexes
//         = is_selected() ? _selection_indexes.extended(index) : _VDNFT::range<long>::make_ordered(_carret_index,
//         index);
//
//     _carret_index = index;
//     get_logic()->get_status().update_selection();
//     get_logic()->get_status().update_carret();
//     repaint();
//   }
//   else {
//     get_logic()->imp_set_carret_index(index, true);
//   }
//
//   _flags.is_down = true;
// }
//
// void text_box::mouse_left_drag(const _VDNFT::mouse_event& evt) {
//   if (!_flags.is_down) {
//     return;
//   }
//
//   _flags.is_dragging = true;
//
//   const float dx = evt.get_position().x - _last_mouse_position.x;
//   _last_mouse_position = evt.get_position();
//
//   _VDNFT::fpoint mpos = evt.get_position() - _scroll_offset;
//   const std::size_t index = _line.get_char_index_at_position(mpos, _text_frame, _current_alignment);
//
//   if (index == _VDNFT::tl::line::invalid_index) {
//     return;
//   }
//
//   _VDNFT::final_action repaint_action = [this]() { repaint(); };
//   _selection_indexes = _VDNFT::range<long>::make_ordered(_carret_index, (long)index);
//   get_logic()->get_status().update_selection();
//
//   const float max_offset = get_logic()->get_maximum_offset();
//
//   // No scrolling required if line width fits in text frame.
//   if (max_offset >= 0) {
//     return;
//   }
//
//   //
//   // Going right.
//   //
//   if (dx > 0) {
//     // Can't scroll anymore to the right.
//     if (_scroll_offset.x == max_offset) {
//       return;
//     }
//
//     const float char_offset = _line.get_offset_for_char_index(index);
//
//     if (char_offset + _scroll_offset.x > _text_frame.width) {
//       _scroll_offset.x = _VDNFT::maximum(_scroll_offset.x - dx, max_offset);
//     }
//     //    if (char_offset > _scroll_offset.x + _text_frame.width) {
//     //      _scroll_offset.x = _VDNFT::maximum(_scroll_offset.x - dx, max_offset);
//     //    }
//     return;
//   }
//
//   //
//   // Going left.
//   //
//
//   // Can't scroll anymore to the left.
//   if (_scroll_offset.x == 0) {
//     return;
//   }
//
//   const float char_offset = _line.get_offset_for_char_index(index);
//   if (_scroll_offset.x + char_offset < 0) {
//     _scroll_offset.x = _VDNFT::minimum(_scroll_offset.x - dx, 0);
//   }
// }
//
// void text_box::mouse_left_up(const _VDNFT::mouse_event& evt) {
//
//   if (_flags.is_dragging) {
//     _VDNFT::fpoint mpos = evt.get_position() - _scroll_offset;
//
//     const std::size_t index = _line.get_char_index_at_position(mpos, _text_frame, _current_alignment);
//     if (index != _VDNFT::tl::line::invalid_index) {
//
//       _selection_indexes = _VDNFT::range<long>::make_ordered(_carret_index, (long)index);
//       get_logic()->get_status().update_selection();
//       get_logic()->get_status().update_carret();
//       repaint();
//     }
//   }
//
//   _flags.is_down = false;
//   _flags.is_dragging = false;
// }
//
// void text_box::mouse_enter(const _VDNFT::mouse_event& evt) {
//
//   _VDNFT::unused(evt);
//   set_cursor(dnft::cursor_type::text);
// }
//
// void text_box::mouse_leave() { set_cursor(dnft::cursor_type::normal); }
//
// void text_box::bounds_changed(const _VDNFT::view::bounds_status& bs) {
//   if (bs.size_changed()) {
//     _VDNFT::frect local_bounds = get_local_fbounds();
//     _text_frame = local_bounds.reduced({ 4, 0 });
//     get_logic()->get_status().update_text();
//
//     const float border_radius = 5.0f;
//     _bg_path = _VDNFT::path::create_rounded_rect(local_bounds, border_radius);
//     repaint();
//   }
// }
//
// void text_box::paint(_VDNFT::context& gc, const _VDNFT::frect& dirty_rect) {
//
//   logic* logic = get_logic();
//   logic::update_status& status = logic->get_status();
//
//   if (status) {
//     if (status.text_changed()) {
//       _line_width = _line.get_width();
//
//       logic->update_line_offset();
//       logic->clip_scroll_offset();
//     }
//
//     if (is_carret_visible() && status.carret_changed()) {
//       _carret_pos = _line.get_offset_for_char_index(_carret_index);
//     }
//
//     if (is_selected() && status.selection_changed()) {
//       _selection_positions = _line.get_offset_for_char_index(_selection_indexes.cast<std::size_t>());
//       //      _selection_positions.min = _line.get_offset_for_char_index(_selection_indexes.min);
//       //      _selection_positions.max = _line.get_offset_for_char_index(_selection_indexes.max);
//     }
//
//     status.reset();
//   }
//
//   _VDNFT::unused(dirty_rect);
//
//   // Draw background.
//   gc.set_shadow(2.0f, 0x0000003C, { 0, -0.5 });
//   gc.set_fill_color(0xFFFFFFFF);
//   gc.fill_path(_bg_path);
//   gc.set_shadow(0.0f, 0);
//
//   // Draw contour.
//   gc.set_line_width(0.5f);
//   gc.set_stroke_color(0xD3D4D4FF);
//   gc.stroke_path(_bg_path);
//
//   if (!is_focused()) {
//     // Draw text.
//     gc.save_state();
//     gc.clip_to_rect(_text_frame);
//
//     if (get_text_size()) {
//       gc.set_fill_color(0x262626FF);
//       _line.draw(gc, logic->get_text_rect(), _current_alignment);
//     }
//     else if (_placeholder) {
//       gc.set_fill_color(0x26262644);
//       _placeholder->draw(gc, _text_frame, _current_alignment);
//     }
//     gc.restore_state();
//     return;
//   }
//
//   // Draw focus contour.
//   gc.set_stroke_color(0x297DFF88);
//   gc.set_line_width(2);
//   gc.stroke_path(_bg_path);
//
//   gc.save_state();
//   gc.clip_to_rect(_text_frame.with_width(_text_frame.width + 1));
//   //  gc.clip_to_rect(_text_frame.expanded({ 1, 0 }));
//
//   // Draw selection.
//   if (is_selected()) {
//     gc.set_fill_color(0x297DFF4C);
//     gc.fill_rect(logic->get_selection_rect());
//   }
//
//   // Draw text.
//   gc.set_fill_color(0x262626FF);
//   _line.draw(gc, logic->get_text_rect(), _current_alignment);
//
//   // Draw cursor.
//   if (is_carret_visible()) {
//     gc.set_fill_color(0x000000FF);
//     gc.fill_rect(logic->get_carret_rect());
//   }
//
//   gc.restore_state();
// }
//
// DNFT_END_NAMESPACE
