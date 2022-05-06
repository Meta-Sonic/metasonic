#include "mts/config.h"

#ifdef MTS_MACOS

#include "mts/graphics/font.h"
#include "mts/final_action.h"
#include "mts/utf8.h"
#include "mts/assert.h"
#include "mts/print.h"

#include "mts/detail/mac_cf_helpers.h"
#include <algorithm>

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>
#include "helpers.h"

MTS_BEGIN_NAMESPACE
font::font(const mts::filesystem::path& filepath, float font_size)
    : _font_size(font_size) {

  CFStringRef cfStringRef
      = CFStringCreateWithCString(kCFAllocatorDefault, filepath.string().c_str(), kCFStringEncodingUTF8);
  CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfStringRef, kCFURLPOSIXPathStyle, false);
  CFRelease(cfStringRef);

  // This function returns a retained reference to a CFArray of CTFontDescriptorRef objects, or NULL on error.
  // The caller is responsible for releasing the array.
  CFArrayRef arr = CTFontManagerCreateFontDescriptorsFromURL(url);
  //  fst::print("FONNFONF", arr);

  CFRelease(url);

  if (arr == nullptr) {
    return;
  }

  mts::final_action release_array([arr]() { CFRelease(arr); });

  CFIndex size = CFArrayGetCount(arr);
  //  fst::print("FONNFONF", size);

  if (size == 0) {
    return;
  }

  CTFontDescriptorRef desc = (CTFontDescriptorRef)CFArrayGetValueAtIndex(arr, 0);

  _font = (native_font)CTFontCreateWithFontDescriptor(desc, (CGFloat)font_size, nullptr);

  //  fst::print("GG COUNT", CTFontGetGlyphCount((CTFontRef)_font));
}

font::font(mts::byte_view data, float font_size)
    : _font_size(font_size) {

  if (data.empty()) {
    _font_size = 0;
    _font = nullptr;
    return;
  }

  CFDataRef data_ref = CFDataCreate(nullptr, data.data(), (CFIndex)data.size());
  if (!data_ref) {
    _font_size = 0;
    _font = nullptr;
    return;
  }

  CTFontDescriptorRef desc = CTFontManagerCreateFontDescriptorFromData(data_ref);
  CFRelease(data_ref);

  if (!desc) {
    _font_size = 0;
    _font = nullptr;

    return;
  }

  _font = (native_font)CTFontCreateWithFontDescriptor(desc, (CGFloat)font_size, nullptr);
  CFRelease(desc);
}

font::font(const font& f) {
  if (f._font) {
    _font = f._font;
    CFRetain(_font);
  }
  else {
    _font = nullptr;
  }

  _font_size = f._font_size;
}

font::font(font&& f) {
  _font = f._font;
  f._font = nullptr;
  _font_size = f._font_size;
}

font::~font() {
  if (_font) {
    CFRelease(_font);
  }
}

font& font::operator=(const font& f) {
  if (_font) {
    CFRelease(_font);
  }

  if (f._font) {
    _font = f._font;
    CFRetain(_font);
  }
  else {
    _font = nullptr;
  }

  _font_size = f._font_size;
  return *this;
}

font& font::operator=(font&& f) {
  if (_font) {
    CFRelease(_font);
  }

  _font = f._font;
  f._font = nullptr;
  _font_size = f._font_size;
  return *this;
}

font font::make_copy(float font_size) const {
  if (!_font) {
    return font();
  }

  font f;
  f._font = (native_font)CTFontCreateCopyWithAttributes(CTFontRef(_font), font_size, nullptr, nullptr);
  f._font_size = font_size;
  CFRetain(f._font);
  return f;
}

bool font::is_valid() const { return _font; }

float font::get_height() const {
  //  // This will return the design bounding box of the font, which is the rectangle defined by
  //  // xMin, yMin, xMax, and yMax values for the font.
  //  CGRect rect = CTFontGetBoundingBox(_font);
  //  return rect.size.height - rect.origin.y;

  //  return CTFontGetXHeight(_font);
  return CTFontGetCapHeight((CTFontRef)_font);
}

struct Advances {
  Advances(CTRunRef run, CFIndex numGlyphs)
      : advances(CTRunGetAdvancesPtr(run)) {

    if (advances == nullptr) {
      local.resize(numGlyphs);
      CTRunGetAdvances(run, CFRangeMake(0, 0), local.data());
      advances = local.data();
    }
  }

  const CGSize* advances;
  std::vector<CGSize> local;
};

struct Glyphs {
  Glyphs(CTRunRef run, CFIndex numGlyphs)
      : glyphs(CTRunGetGlyphsPtr(run)) {
    if (glyphs == nullptr) {
      local.resize(numGlyphs);
      CTRunGetGlyphs(run, CFRangeMake(0, 0), local.data());
      glyphs = local.data();
    }
  }

  const CGGlyph* glyphs;
  std::vector<CGGlyph> local;
};

inline CTLineRef get_line(CTFontRef font, std::u32string_view text) {
  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTLigatureAttributeName }, { font, kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  return CTLineCreateWithAttributedString(attr_str);
}

float font::get_string_width(std::string_view text) const {
  //  if (text.empty() || !is_valid()) {
  //    return 0;
  //  }

  if (text.empty()) {
    return 0;
  }

  mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
      { kCTFontAttributeName, kCTLigatureAttributeName }, { (CTFontRef)_font, kCFBooleanTrue });

  //  dnft::print("lkasjdjlakjdsa", text);
  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string_view(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);
  CFArrayRef run_array = CTLineGetGlyphRuns(line);

  float x = 0;

  for (CFIndex i = 0; i < CFArrayGetCount(run_array); ++i) {
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
    CFIndex length = CTRunGetGlyphCount(run);

    const Advances advances(run, length);

    for (CFIndex j = 0; j < length; j++) {
      x += (float)advances.advances[j].width;
    }
  }

  return x;
}

// float font::get_string_width(std::u32string_view text) const {
//   if (text.empty() || !is_valid()) {
//     return 0;
//   }
//
//   mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
//       { kCTFontAttributeName, kCTLigatureAttributeName }, { (CTFontRef)_font, kCFBooleanTrue });
//
////  dnft::print("lkasjdjlakjdsa", text);
////  std::string u8str = mts::utf8::utf32to8(text);
////  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string_view(u8str);
//
//  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(text);
//
//  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
//      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);
//
//  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);
//  CFArrayRef run_array = CTLineGetGlyphRuns(line);
//
//  float x = 0;
//
//  for (CFIndex i = 0; i < CFArrayGetCount(run_array); ++i) {
//    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
//    CFIndex length = CTRunGetGlyphCount(run);
//
//    const Advances advances(run, length);
//
//    for (CFIndex j = 0; j < length; j++) {
//      x += (float)advances.advances[j].width;
//    }
//  }
//
//  return x;
//}

float font::get_string_width(std::u32string_view text) const {
  //  if (text.empty() || !is_valid()) {
  //    return 0;
  //  }

  if (text.empty()) {
    return 0;
  }

  mts::cf_unique_ptr<CTLineRef> line = get_line((CTFontRef)_font, text);
  CFArrayRef run_array = CTLineGetGlyphRuns(line);

  float x = 0;

  for (CFIndex i = 0; i < CFArrayGetCount(run_array); ++i) {
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
    CFIndex length = CTRunGetGlyphCount(run);

    const Advances advances(run, length);

    for (CFIndex j = 0; j < length; j++) {
      x += (float)advances.advances[j].width;
    }
  }

  return x;
}

int font::get_char_index(std::u32string_view text, float x) const {
  //  if (text.empty() || !is_valid()) {
  //    return 0;
  //  }

  if (text.empty()) {
    return 0;
  }

  if (x < 0) {
    return 0;
  }

  mts::cf_unique_ptr<CTLineRef> line = get_line((CTFontRef)_font, text);

  CFArrayRef run_array = CTLineGetGlyphRuns(line);

  float w = 0;
  float last_lw = 0;

  for (CFIndex i = 0; i < CFArrayGetCount(run_array); ++i) {
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
    CFIndex length = CTRunGetGlyphCount(run);

    const Advances advances(run, length);

    for (CFIndex j = 0; j < length; j++) {
      float lw = (float)advances.advances[j].width;
      float nw = w + lw;

      if (x >= w - last_lw * 0.25f && x < nw - lw * 0.25f) {
        return (int)j;
      }
      last_lw = lw;

      w = nw;
    }
  }

  return text.size();
}
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
  //  std::vector<CGSize> local;
};
void font::get_characters_positions(std::u32string_view text, std::vector<float>& offsets) const {
  offsets.clear();
  offsets.push_back(0);

  //  if (text.empty() || !is_valid()) {
  //    return;
  //  }

  if (text.empty()) {
    return;
  }

  mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
      { kCTFontAttributeName, kCTLigatureAttributeName }, { (CTFontRef)_font, kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string_view(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CFArrayRef run_array = CTLineGetGlyphRuns(line);

  float x = 0;
  std::vector<CGSize> sizes;
  for (CFIndex i = 0; i < CFArrayGetCount(run_array); i++) {
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
    CFIndex length = CTRunGetGlyphCount(run);

    std::size_t start_index = offsets.size();
    offsets.resize(offsets.size() + length);

    const FAdvances advances(sizes, run, length);
    for (CFIndex j = 0; j < length; j++) {
      x += (float)sizes[j].width;
      offsets[start_index + j] = x;
      //      offsets.push_back(x);
    }

    //    for (CFIndex j = 0; j < length; j++) {
    //      x += (float)advances.advances[j].width;
    //      offsets.push_back(x);
    //    }
  }
}
void font::get_glyph_positions(
    std::string_view text, std::vector<glyph_index>& glyphs, std::vector<float>& offsets) const {
  offsets.push_back(0);

  //  if (text.empty() || !is_valid()) {
  //    return;
  //  }

  if (text.empty()) {
    return;
  }

  mts::cf_unique_ptr<CFDictionaryRef> attributes = mts::create_cf_dictionary(
      { kCTFontAttributeName, kCTLigatureAttributeName }, { (CTFontRef)_font, kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string_view(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CFArrayRef run_array = CTLineGetGlyphRuns(line);

  float x = 0;

  for (CFIndex i = 0; i < CFArrayGetCount(run_array); i++) {
    CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(run_array, i);
    CFIndex length = CTRunGetGlyphCount(run);

    const Advances advances(run, length);
    const Glyphs r_glyphs(run, length);

    for (CFIndex j = 0; j < length; j++) {
      x += (float)advances.advances[j].width;
      offsets.push_back(x);
      glyphs.push_back(r_glyphs.glyphs[j]);
    }
  }
}

path font::get_glyph_outline(glyph_index index) const {
  CGGlyph glyphs = index;
  CGSize offset;
  CTFontGetVerticalTranslationsForGlyphs((CTFontRef)_font, &glyphs, &offset, 1);

  CGAffineTransform transform = CGAffineTransformTranslate(CGAffineTransformMakeScale(1, -1), 0, offset.height);

  CGPathRef p = CTFontCreatePathForGlyph((CTFontRef)_font, (CGGlyph)glyphs, &transform);
  if (!p) {
    return path();
  }

  CGMutablePathRef mp = CGPathCreateMutableCopy(p);
  CFRelease(p);

  if (!mp) {
    return path();
  }

  return path(mp);
}

std::vector<font::glyph_index> font::get_glyph_indexes(std::string_view text) const {
  std::u16string chars = mts::utf8::utf8to16(text);
  std::vector<CGGlyph> glyphs;
  glyphs.resize(chars.size(), invalid_glyph_index);

  bool ret = CTFontGetGlyphsForCharacters((CTFontRef)_font, (const UniChar*)chars.data(), glyphs.data(), chars.size());

  if (ret) {
    std::vector<glyph_index> rglyphs;
    std::copy(glyphs.begin(), glyphs.end(), std::back_inserter(rglyphs));

    return rglyphs;
  }
  return std::vector<glyph_index>();
}

font::glyph_index font::get_glyph_index(char c) const {
  UniChar u16 = c;
  CGGlyph glyph;

  if (CTFontGetGlyphsForCharacters((CTFontRef)_font, (const UniChar*)&u16, &glyph, 1)) {
    return glyph;
  }
  return invalid_glyph_index;
}

font::glyph_index font::get_glyph_index(char16_t c) const {
  CGGlyph glyph;

  if (CTFontGetGlyphsForCharacters((CTFontRef)_font, (const UniChar*)&c, &glyph, 1)) {
    return glyph;
  }
  return invalid_glyph_index;
}

font::glyph_index font::get_glyph_index(char32_t c) const {
  std::string u8 = mts::utf8::utf32to8(std::u32string_view(&c, 1));
  std::u16string chars = mts::utf8::utf8to16(mts::utf8::utf32to8(std::u32string_view(&c, 1)));

  if (chars.empty()) {
    return invalid_glyph_index;
  }

  mts_assert(chars.size() <= 2, "Wrong char");

  CGGlyph glyphs[2];

  if (CTFontGetGlyphsForCharacters((CTFontRef)_font, (const UniChar*)chars.data(), glyphs, chars.size())) {
    return glyphs[0];
  }
  return invalid_glyph_index;
}

font::glyph_index font::get_glyph_index(std::string_view c) const {
  std::u16string chars = mts::utf8::utf8to16(c);
  if (chars.empty()) {
    return invalid_glyph_index;
  }

  if (chars.size() != 1) {
    chars.resize(1);
  }
  std::vector<CGGlyph> glyphs;
  glyphs.resize(1, -1);

  bool ret = CTFontGetGlyphsForCharacters((CTFontRef)_font, (const UniChar*)chars.data(), glyphs.data(), chars.size());
  if (ret) {
    return glyphs[0];
  }
  return invalid_glyph_index;
}

MTS_END_NAMESPACE

#endif // MTS_MACOS
