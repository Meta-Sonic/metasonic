#include "mts/config.h"

#ifdef MTS_MACOS

  #include "mts/graphics/text_layout.h"
  #include "mts/assert.h"
  #include "mts/utf8.h"
  #include "mts/pointer.h"
  #include "mts/final_action.h"

  #include "mts/detail/mac_cf_helpers.h"
  #include "helpers.h"

//#include "../../../native/mac/mac_helpers.h"

  #include <CoreGraphics/CoreGraphics.h>
  #include <CoreText/CoreText.h>

//#include <CoreFoundation/CoreFoundation.h>
//#include <CoreFoundation/CFBase.h>
//#include <CoreFoundation/CFString.h>

MTS_BEGIN_NAMESPACE
namespace tl {
// inline CTLineRef get_line(CTFontRef font, std::u32string_view text) {
//   mts::cf_unique_ptr<CFDictionaryRef> attributes
//       = mts::create_cf_dictionary({ kCTFontAttributeName, kCTLigatureAttributeName }, { font, kCFBooleanTrue });
//
//   mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(text);
//
//   mts::cf_unique_ptr<CFAttributedStringRef> attr_str
//       = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);
//
//   return CTLineCreateWithAttributedString(attr_str);
// }

// float font::get_string_width(std::string_view text) const {
//   if (text.empty() || !is_valid()) {
//     return 0;
//   }
//
//   mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
//       { kCTFontAttributeName, kCTLigatureAttributeName }, { (CTFontRef)_font, kCFBooleanTrue });
//
//   dnft::print("lkasjdjlakjdsa", text);
//   mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string_view(text);
//
//   mts::cf_unique_ptr<CFAttributedStringRef> attr_str
//       = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);
//
//   mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);
//   CFArrayRef run_array = CTLineGetGlyphRuns(line);
//
//   float x = 0;
//
//   for (CFIndex i = 0; i < CFArrayGetCount(run_array); ++i) {
//     CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
//     CFIndex length = CTRunGetGlyphCount(run);
//
//     const Advances advances(run, length);
//
//     for (CFIndex j = 0; j < length; j++) {
//       x += (float)advances.advances[j].width;
//     }
//   }
//
//   return x;
// }

struct FAdvances {
  FAdvances(std::vector<CGSize>& sizes, CTRunRef run, CFIndex numGlyphs)
      : advances(CTRunGetAdvancesPtr(run)) {

    if (advances == nullptr) {
      sizes.resize(numGlyphs);
      CTRunGetAdvances(run, CFRangeMake(0, 0), sizes.data());
      advances = sizes.data();
    }
  }

  const CGSize* advances;
};

static inline CGContextRef get_gc_context(context::native_context nc) { return (CGContextRef)nc; }

class line::native_line_impl {
public:
  native_line_impl() = delete;
  native_line_impl(const native_line_impl&) = delete;
  native_line_impl(native_line_impl&&) = delete;

  native_line_impl& operator=(const native_line_impl&) = delete;
  native_line_impl& operator=(native_line_impl&&) = delete;

  native_line_impl(mts::not_null<line*> l)
      : _line(l) {
    update();
  }

  ~native_line_impl() { reset_ct_line(); }

  inline void set_line(mts::not_null<line*> l) {
    _line = l;
    update();
  }

  inline void update() {
    mts::final_action notify = _line->_change_callback;

    reset_ct_line();

    const std::u32string& text = _line->get_text_utf32();
    if (text.empty()) {
      return;
    }

    create_ct_line();
  }

  float get_width() const {
    if (!_ct_line) {
      return 0;
    }

    if (_line_width < 0) {
      _line_width = (float)CTLineGetTypographicBounds(_ct_line, nullptr, nullptr, nullptr);
    }

    return _line_width;
  }

  // Top - The maximum distance above the baseline for the tallest glyph in the font at a given text size.
  // Ascent - The recommended distance above the baseline for singled spaced text.
  // Descent - The recommended distance below the baseline for singled spaced text.
  // Bottom - The maximum distance below the baseline for the lowest glyph in the font at a given text size.
  // Leading - The recommended additional space to add between lines of text.

  std::pair<float, float> get_ascent_descent() const {
    if (!_ct_line) {
      return { 0, 0 };
    }

    CGFloat ascent;
    CGFloat descent;
    CTLineGetTypographicBounds(_ct_line, &ascent, &descent, nullptr);

    return { (float)ascent, (float)descent };
  }

  inline float get_line_height() const {
    if (!_ct_line) {
      return 0;
    }

    CGFloat ascent;
    CGFloat descent;
    CGFloat leading;
    CTLineGetTypographicBounds(_ct_line, &ascent, &descent, &leading);
    return (float)(ascent - descent + leading);
  }

  inline float get_ascent() const {
    if (!_ct_line) {
      return 0;
    }

    CGFloat ascent;
    CTLineGetTypographicBounds(_ct_line, &ascent, nullptr, nullptr);
    return (float)ascent;
  }

  inline float get_descent() const {
    if (!_ct_line) {
      return 0;
    }

    CGFloat descent;
    CTLineGetTypographicBounds(_ct_line, nullptr, &descent, nullptr);
    return (float)descent;
  }

  struct font_info {
    float ascent;
    float descent;
    float leading;
  };

  font_info get_info() const {
    if (!_ct_line) {
      return { 0, 0, 0 };
    }

    CGFloat ascent;
    CGFloat descent;
    CGFloat leading;
    CTLineGetTypographicBounds(_ct_line, &ascent, &descent, &leading);
    return font_info{ (float)ascent, (float)descent, (float)leading };
  }

  mts::rect<float> get_bounds() const {
    if (!_ct_line) {
      return mts::rect<float>{ 0, 0, 0, 0 };
    }

    return from_cg_rect<mts::rect<float>>(CTLineGetBoundsWithOptions(_ct_line, 0));
    //    return mts::from_cg_rect<float>(CTLineGetBoundsWithOptions(_ct_line, kCTLineBoundsUseGlyphPathBounds));
  }

  std::size_t get_char_index_at_position(const mts::point<float>& pos) const {
    if (!_ct_line) {
      return mts::tl::line::invalid_index;
    }

    CFIndex index = CTLineGetStringIndexForPosition(_ct_line, mts::to_cg_point(pos));
    return index == kCFNotFound ? mts::tl::line::invalid_index : (std::size_t)index;
    //    return 0;
  }

  float get_offset_for_char_index(std::size_t index) const {
    if (!_ct_line) {
      return 0;
    }

    return CTLineGetOffsetForStringIndex(_ct_line, (CFIndex)index, nullptr);
  }

  inline mts::range<float> get_offset_for_char_index(mts::range<std::size_t> indexes) const {
    if (!_ct_line) {
      return { 0, 0 };
    }

    return { (float)CTLineGetOffsetForStringIndex(_ct_line, (CFIndex)indexes.min, nullptr),
      (float)CTLineGetOffsetForStringIndex(_ct_line, (CFIndex)indexes.max, nullptr) };
  }

  inline mts::point<float> get_aligned_position(const mts::rect<float>& rect, mts::alignment align) const {
    switch (align.get_justification()) {
    case mts::alignment::justification::left_top: {
      return rect.position + mts::point<float>(0, get_line_height());
    } break;
    case mts::alignment::justification::left_center: {
      return rect.position + mts::point<float>(0, rect.height + get_line_height()) * 0.5f;
    } break;
    case mts::alignment::justification::left_bottom: {
      return rect.position + mts::point<float>(0, rect.height - get_descent());
    } break;
    case mts::alignment::justification::center_top: {
      return rect.position + mts::point<float>((rect.width - get_width()) * 0.5f, get_line_height());
    } break;
    case mts::alignment::justification::center_center: {
      return rect.in_middle({ get_width(), -get_line_height() });
    } break;
    case mts::alignment::justification::center_bottom: {
      return rect.position + mts::point<float>((rect.width - get_width()) * 0.5f, rect.height - get_descent());
    } break;
    case mts::alignment::justification::right_top: {
      return rect.position + mts::point<float>(rect.width - get_width(), get_line_height());
    } break;
    case mts::alignment::justification::right_center: {
      return rect.position + mts::point<float>(rect.width - get_width(), (rect.height + get_line_height()) * 0.5f);
    } break;
    case mts::alignment::justification::right_bottom: {
      return rect.position + mts::point<float>(rect.width - get_width(), rect.height - get_descent());
    } break;
    }

    return rect.position;
  }

  inline mts::rect<float> get_aligned_bounds(const mts::rect<float>& rect, mts::alignment alignment) const {
    return mts::rect<float>(get_aligned_position(rect, alignment), get_bounds().size);
  }

  //  inline void iterate() {
  //    dnft::print("iterate-----------------");
  //    CTLineEnumerateCaretOffsets(_ct_line, ^(double offset, CFIndex charIndex, bool leadingEdge, bool* stop) {
  //      dnft::print("iterate", offset, charIndex, leadingEdge);
  //    });
  ////    ;
  //  }

  void draw(mts::context& gc, const mts::point<float>& pos) {
    CGContextRef g = get_gc_context(gc.get_native_context());
    CGContextSetTextDrawingMode(g, kCGTextFill);
    CGContextSetTextMatrix(g, CGAffineTransformMakeScale(1.0f, -1.0f));
    CGContextSetTextPosition(g, pos.x, pos.y);
    CTLineDraw(_ct_line, g);
  }

  void draw(mts::context& gc, const mts::rect<float>& rect, mts::alignment align) {
    mts::point<float> pos = get_aligned_position(rect, align);
    CGContextRef g = get_gc_context(gc.get_native_context());
    CGContextSetTextDrawingMode(g, kCGTextFill);
    CGContextSetTextMatrix(g, CGAffineTransformMakeScale(1.0f, -1.0f));
    CGContextSetTextPosition(g, pos.x, pos.y);
    CTLineDraw(_ct_line, g);
  }

  static inline native_line_impl* to_ct_native_line(void* l) { return (native_line_impl*)l; }
  static inline const native_line_impl* to_ct_native_line(const void* l) { return (const native_line_impl*)l; }
  static inline native_line_impl* get_ct_native_line(line& l) { return (native_line_impl*)l.get_native_line(); }
  static inline const native_line_impl* get_ct_native_line(const line& l) {
    return (const native_line_impl*)l.get_native_line();
  }

private:
  line* _line = nullptr;
  CTLineRef _ct_line = nullptr;
  mutable float _line_width = -1;

  //  friend struct native_line_proxy;

  inline void reset_ct_line() {
    if (_ct_line) {
      CFRelease(_ct_line);
      _ct_line = nullptr;
    }

    _line_width = -1;
  }

  //    mts::cf_unique_ptr<CFDictionaryRef> attributes
  //        = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
  //            { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  void create_ct_line() {
    if (_line->_is_password) {

      mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
          { kCTFontAttributeName, kCTLigatureAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)_line->get_font().get_native_font(), kCFBooleanTrue, kCFBooleanTrue });

      //      std::u32string ss = _line->get_text_utf32();
      std::u32string ss;
      ss.resize(_line->get_text_utf32().size(), 'a');
      //      ss.resize(_line->get_text_utf32().size(), L'•');

      //      std::replace(ss.begin(), ss.end(), '\t', ' ');
      mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(ss);

      mts::cf_unique_ptr<CFAttributedStringRef> attr_str
          = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

      _ct_line = CTLineCreateWithAttributedString(attr_str);
      _line_width = -1;

      return;
    }

    mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
        { kCTFontAttributeName, kCTLigatureAttributeName, kCTForegroundColorFromContextAttributeName },
        { (CTFontRef)_line->get_font().get_native_font(), kCFBooleanTrue, kCFBooleanTrue });

    mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(_line->get_text_utf32());

    mts::cf_unique_ptr<CFAttributedStringRef> attr_str = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

    _ct_line = CTLineCreateWithAttributedString(attr_str);
    _line_width = -1;
  }
};

// inline ct_native_line* to_ct_native_line(void* l) { return (ct_native_line*)l; }
//
// inline const ct_native_line* to_ct_native_line(const void* l) { return (const ct_native_line*)l; }
//
// inline ct_native_line* get_ct_native_line(line& l) { return (ct_native_line*)l.get_native_line(); }
//
// inline const ct_native_line* get_ct_native_line(const line& l) { return (const ct_native_line*)l.get_native_line();
//}

line::line() {
  _line = new native_line_impl(this);
  //  dnft::font fff;
  //  dnft::print("<ANBSNABSNMABSMNABSMNABSMNBASMNBAMNSBMNASSA", fff.get_string_width("ABC"));
}

line::line(const line& l)
    : _font(l._font)
    , _text(l._text) {
  _line = new native_line_impl(this);
}

line::line(line&& l)
    : _font(std::move(l._font))
    , _text(std::move(l._text))
    , _line(l._line) {
  l._line = nullptr;

  if (_line) {
    native_line_impl::get_ct_native_line(*this)->set_line(this);
  }
}

line::line(const mts::font& f, const std::u32string& text)
    : _font(f)
    , _text(text) {
  _line = new native_line_impl(this);
}

line::line(const mts::font& f, std::u32string&& text)
    : _font(f)
    , _text(std::move(text)) {
  _line = new native_line_impl(this);
}

// line::line(const mts::font& f, const std::string& text)
//     : _font(f)
//     , _text(mts::utf8::utf8to32(text)) {
//   _line = new ct_native_line(this);
// }

line::line(const mts::font& f, std::u32string_view text)
    : _font(f)
    , _text(text) {
  _line = new native_line_impl(this);
}

line::line(const mts::font& f, std::string_view text)
    : _font(f)
    , _text(mts::utf8::utf8to32(text)) {
  _line = new native_line_impl(this);
}

line::~line() {
  if (_line) {
    delete native_line_impl::to_ct_native_line(_line);
    _line = nullptr;
  }
}

line& line::operator=(const line& l) {
  if (_line) {
    delete native_line_impl::to_ct_native_line(_line);
    _line = nullptr;
  }

  _font = l._font;
  _text = l._text;
  _line = new native_line_impl(this);

  return *this;
}

line& line::operator=(line&& l) {
  _font = std::move(l._font);
  _text = std::move(l._text);

  if (_line) {
    delete native_line_impl::to_ct_native_line(_line);
    _line = nullptr;
  }

  _line = l._line;
  l._line = nullptr;

  if (_line) {
    native_line_impl::get_ct_native_line(*this)->set_line(this);
  }

  return *this;
}

void line::update() {
  if (_line) {
    native_line_impl::to_ct_native_line(_line)->update();
  }
}

void line::set_font(const mts::font& f) {
  _font = f;
  native_line_impl::to_ct_native_line(_line)->update();
}

void line::set_text(const std::u32string& text, bool replace_tabs) {
  _text = text;
  if (replace_tabs) {
    std::replace(_text.begin(), _text.end(), '\t', ' ');
  }
  native_line_impl::to_ct_native_line(_line)->update();
}

void line::set_text(std::u32string_view text, bool replace_tabs) {
  _text.clear();
  _text.insert(0, text.data(), text.size());

  if (replace_tabs) {
    std::replace(_text.begin(), _text.end(), '\t', ' ');
  }

  native_line_impl::to_ct_native_line(_line)->update();
}

void line::set_text(std::u16string_view text, bool replace_tabs) {
  _text = mts::utf8::utf8to32(mts::utf8::utf16to8(text));

  if (replace_tabs) {
    std::replace(_text.begin(), _text.end(), '\t', ' ');
  }
  native_line_impl::to_ct_native_line(_line)->update();
}

void line::set_text(std::string_view text, bool replace_tabs) {
  _text = mts::utf8::utf8to32(text);

  if (replace_tabs) {
    std::replace(_text.begin(), _text.end(), '\t', ' ');
  }

  native_line_impl::to_ct_native_line(_line)->update();
}

std::u16string line::get_text_utf16() const { return mts::utf8::utf8to16(get_text_utf8()); }
std::string line::get_text_utf8() const { return mts::utf8::utf32to8(_text); }
std::string line::get_text() const { return mts::utf8::utf32to8(_text); }

void line::set_password(bool is_password) { _is_password = is_password; }

mts::rect<float> line::get_bounds() const {
  if (!_line) {
    return { 0, 0, 0, 0 };
  }
  return native_line_impl::to_ct_native_line(_line)->get_bounds();
}

mts::size<float> line::get_size() const {
  if (!_line) {
    return { 0, 0 };
  }
  return native_line_impl::to_ct_native_line(_line)->get_bounds().size;
}

float line::get_width() const {
  if (!_line) {
    return 0;
  }
  return native_line_impl::to_ct_native_line(_line)->get_width();
}

mts::point<float> line::get_aligned_position(const mts::rect<float>& rect, mts::alignment alignment) const {
  if (!_line) {
    return { 0, 0 };
  }
  return native_line_impl::to_ct_native_line(_line)->get_aligned_position(rect, alignment);
}

mts::rect<float> line::get_aligned_bounds(const mts::rect<float>& rect, mts::alignment alignment) const {
  if (!_line) {
    return { 0, 0, 0, 0 };
  }
  return native_line_impl::to_ct_native_line(_line)->get_aligned_bounds(rect, alignment);
}

std::size_t line::get_char_index_at_position(const mts::point<float>& pos) const {
  if (!_line) {
    return mts::tl::line::invalid_index;
  }
  return native_line_impl::to_ct_native_line(_line)->get_char_index_at_position(pos);
}

std::size_t line::get_char_index_at_position(
    const mts::point<float>& pos, const mts::rect<float>& rect, mts::alignment alignment) const {
  if (!_line) {
    return mts::tl::line::invalid_index;
  }
  return native_line_impl::to_ct_native_line(_line)->get_char_index_at_position(
      pos - native_line_impl::to_ct_native_line(_line)->get_aligned_position(rect, alignment));
}

float line::get_offset_for_char_index(std::size_t index) const {
  if (!_line) {
    return 0;
  }
  return native_line_impl::to_ct_native_line(_line)->get_offset_for_char_index(index);
}

mts::range<float> line::get_offset_for_char_index(mts::range<std::size_t> indexes) const {
  if (!_line) {
    return { 0, 0 };
  }
  return native_line_impl::to_ct_native_line(_line)->get_offset_for_char_index(indexes);
}

void line::draw(mts::context& gc, const mts::point<float>& pos) {
  if (!_line) {
    return;
  }

  native_line_impl::to_ct_native_line(_line)->draw(gc, pos);
}

void line::draw(mts::context& gc, const mts::rect<float>& rect, mts::alignment alignment) {
  if (!_line) {
    return;
  }

  native_line_impl::to_ct_native_line(_line)->draw(gc, rect, alignment);
}
} // namespace tl.
MTS_END_NAMESPACE

#endif // MTS_MACOS
