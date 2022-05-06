#include "mts/config.h"

#ifdef MTS_MACOS

#include "mts/graphics/context.h"
#include "mts/utf8.h"
#include "mts/detail/mac_cf_helpers.h"
#include "helpers.h"

#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

MTS_BEGIN_NAMESPACE
static inline CGContextRef get_gc_context(context::native_context nc) { return (CGContextRef)nc; }

static inline void flip(CGContextRef c, float flip_height) {
  CGContextConcatCTM(c, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, flip_height));
}

void context::save_state() { CGContextSaveGState(get_gc_context(_native_context)); }

void context::restore_state() { CGContextRestoreGState(get_gc_context(_native_context)); }

void context::begin_transparent_layer(float alpha) {
  save_state();
  CGContextSetAlpha(get_gc_context(_native_context), alpha);
  CGContextBeginTransparencyLayer(get_gc_context(_native_context), nullptr);
}

void context::end_transparent_layer() {
  CGContextEndTransparencyLayer(get_gc_context(_native_context));
  restore_state();
}

void context::translate(const mts::point<float>& pos) {
  CGContextTranslateCTM(get_gc_context(_native_context), pos.x, pos.y);
}

void context::clip_to_rect(const mts::rect<float>& rect) {
  CGContextClipToRect(get_gc_context(_native_context), CGRectMake(rect.x, rect.y, rect.width, rect.height));
}

void context::clip_to_path(const path& p) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextBeginPath(g);
  CGContextAddPath(g, (CGPathRef)p.get_native_path());
  //  CGContextReplacePathWithStrokedPath(g);
  //  CGContextClosePath(g);
  CGContextClip(g);
  //  CGContextEOClip(g);
}

mts::rect<float> context::get_clipping_rect() {
  CGRect r = CGContextGetClipBoundingBox(get_gc_context(_native_context));
  return mts::rect<float>(r.origin.x, r.origin.y, r.size.width, r.size.height);
}

void context::set_line_width(float width) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextSetLineWidth(g, width);
}

void context::set_line_join(line_join lj) {
  switch (lj) {
  case line_join::miter:
    CGContextSetLineJoin(get_gc_context(_native_context), kCGLineJoinMiter);
    break;

  case line_join::round:
    CGContextSetLineJoin(get_gc_context(_native_context), kCGLineJoinRound);
    break;

  case line_join::bevel:
    CGContextSetLineJoin(get_gc_context(_native_context), kCGLineJoinBevel);
    break;
  }
}

void context::set_line_cap(line_cap lc) {
  switch (lc) {
  case line_cap::butt:
    CGContextSetLineCap(get_gc_context(_native_context), kCGLineCapButt);
    break;

  case line_cap::round:
    CGContextSetLineCap(get_gc_context(_native_context), kCGLineCapRound);
    break;

  case line_cap::square:
    CGContextSetLineCap(get_gc_context(_native_context), kCGLineCapSquare);
    break;
  }
}

void context::set_line_style(float width, line_join lj, line_cap lc) {
  set_line_width(width);
  set_line_join(lj);
  set_line_cap(lc);
}

void context::set_fill_color(const mts::color& c) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextSetRGBFillColor(g, c.f_red(), c.f_green(), c.f_blue(), c.f_alpha());
  CGContextSetAlpha(g, 1);
}

void context::set_stroke_color(const mts::color& c) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextSetRGBStrokeColor(g, c.f_red(), c.f_green(), c.f_blue(), c.f_alpha());
  CGContextSetAlpha(g, 1);
}

void context::fill_rect(const mts::rect<float>& r) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextFillRect(g, CGRectMake(r.x, r.y, r.width, r.height));
}

void context::stroke_rect(const mts::rect<float>& r, float line_width) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextStrokeRectWithWidth(g, CGRectMake(r.x, r.y, r.width, r.height), (CGFloat)line_width);
}

void context::stroke_line(const mts::point<float>& p0, const mts::point<float>& p1) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextMoveToPoint(g, p0.x, p0.y);
  CGContextAddLineToPoint(g, p1.x, p1.y);
  CGContextStrokePath(g);
}

void context::fill_ellipse(const mts::rect<float>& r) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextAddEllipseInRect(g, to_cg_rect(r));
  CGContextFillPath(g);
}

void context::stroke_ellipse(const mts::rect<float>& r) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextAddEllipseInRect(g, to_cg_rect(r));
  CGContextStrokePath(g);
}

void context::fill_quad(const mts::quad<float>& q) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextBeginPath(g);
  CGContextMoveToPoint(g, q.top_left.x, q.top_left.y);
  CGContextAddLineToPoint(g, q.top_right.x, q.top_right.y);
  CGContextAddLineToPoint(g, q.bottom_right.x, q.bottom_right.y);
  CGContextAddLineToPoint(g, q.bottom_left.x, q.bottom_left.y);
  CGContextClosePath(g);
  CGContextFillPath(g);
}

void context::stroke_quad(const mts::quad<float>& q) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextBeginPath(g);
  CGContextMoveToPoint(g, q.top_left.x, q.top_left.y);
  CGContextAddLineToPoint(g, q.top_right.x, q.top_right.y);
  CGContextAddLineToPoint(g, q.bottom_right.x, q.bottom_right.y);
  CGContextAddLineToPoint(g, q.bottom_left.x, q.bottom_left.y);
  CGContextClosePath(g);
  CGContextStrokePath(g);
}

// CG_EXTERN void CGContextBeginPath(CGContextRef cg_nullable c)
//     CG_AVAILABLE_STARTING(10.0, 2.0);
//
///* Start a new subpath at point `(x, y)' in the context's path. */
//
// CG_EXTERN void CGContextMoveToPoint(CGContextRef cg_nullable c,
//    CGFloat x, CGFloat y)
//    CG_AVAILABLE_STARTING(10.0, 2.0);
//
///* Append a straight line segment from the current point to `(x, y)'. */
//
// CG_EXTERN void CGContextAddLineToPoint(CGContextRef cg_nullable c,
//    CGFloat x, CGFloat y)
//    CG_AVAILABLE_STARTING(10.0, 2.0);

//
// MARK: Path.
//

void context::fill_path(const path& p) {

  CGContextRef g = get_gc_context(_native_context);
  CGContextAddPath(g, (CGPathRef)p.get_native_path());
  CGContextFillPath(g);
}

void context::fill_path(const path& p, const mts::rect<float>& rect) {
  save_state();
  translate(rect.position);

  CGContextRef g = get_gc_context(_native_context);

  CGContextScaleCTM(g, rect.width, rect.height);
  CGContextAddPath(g, (CGPathRef)p.get_native_path());
  CGContextFillPath(g);

  restore_state();
}

void context::fill_path_with_shadow(
    const path& p, float blur, const mts::color& shadow_color, const mts::size<float>& offset) {

  save_state();
  CGContextRef g = get_gc_context(_native_context);
  CGColorRef scolor
      = CGColorCreateSRGB(shadow_color.f_red(), shadow_color.f_green(), shadow_color.f_blue(), shadow_color.f_alpha());
  CGContextSetShadowWithColor(g, to_cg_size(offset), blur, scolor);
  CGColorRelease(scolor);
  fill_path(p);

  restore_state();
}

void context::set_shadow(float blur, const mts::color& shadow_color, const mts::size<float>& offset) {
  CGContextRef g = get_gc_context(_native_context);
  CGColorRef scolor
      = CGColorCreateSRGB(shadow_color.f_red(), shadow_color.f_green(), shadow_color.f_blue(), shadow_color.f_alpha());
  CGContextSetShadowWithColor(g, to_cg_size(offset), blur, scolor);
  CGColorRelease(scolor);
}

void context::stroke_path(const path& p) {
  CGContextRef g = get_gc_context(_native_context);
  CGContextAddPath(g, (CGPathRef)p.get_native_path());
  CGContextStrokePath(g);
}

//
// MARK: Image.
//

void context::draw_image(const image& img, const mts::point<float>& pos) {
  CGContextRef g = get_gc_context(_native_context);
  mts::rect<float> rect = { pos, img.get_size() };

  //    save_state();
  CGContextTranslateCTM(g, rect.x, rect.y);
  flip(g, rect.height);
  CGContextDrawImage(g, to_cg_rect(rect.with_position({ 0.0f, 0.0f })), (CGImageRef)img.get_native_image());
  //    CGContextDrawImage(g, CGRectMake(0, 0, rect.width, rect.height), (CGImageRef)img.get_native_image());

  //    restore_state();
  flip(g, rect.height);
  CGContextTranslateCTM(g, -rect.x, -rect.y);
}

void context::draw_image(const mts::image& img, const mts::rect<float>& r) {
  CGContextRef g = get_gc_context(_native_context);
  //    mts::rect<float> rect = {pos, img.get_size()};

  //  save_state();
  CGContextTranslateCTM(g, r.x, r.y);
  flip(g, r.height);
  CGContextDrawImage(g, to_cg_rect(r.with_position({ 0.0f, 0.0f })), (CGImageRef)img.get_native_image());

  //    restore_state();
  flip(g, r.height);
  CGContextTranslateCTM(g, -r.x, -r.y);
}

void context::draw_image(const mts::image& img, const mts::rect<float>& r, const mts::rect<float>& clip_rect) {
  CGContextRef g = get_gc_context(_native_context);

  save_state();
  CGContextTranslateCTM(g, r.x, r.y);
  clip_to_rect(clip_rect);
  flip(g, r.height);
  CGContextDrawImage(g, to_cg_rect(r.with_position({ 0.0f, 0.0f })), (CGImageRef)img.get_native_image());
  restore_state();
}

//
// MARK: Text.
//
inline constexpr float k_default_mac_font_height = 11.0f;

void context::draw_text(mts::font& f, const std::string& text, const mts::point<float>& pos) {
  const float font_height = f.is_valid() ? f.get_height() : k_default_mac_font_height;

  CGContextRef g = get_gc_context(_native_context);

  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CGContextSetTextDrawingMode(g, kCGTextFill);
  CGContextSetTextMatrix(g, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, font_height));
  CGContextSetTextPosition(g, pos.x, pos.y + font_height);
  CTLineDraw(line, g);
}

void context::draw_text(
    font& f, const std::string& text, const mts::rect<float>& rect, text_alignment alignment) {
  switch (alignment) {
  case text_alignment::left:
    return draw_text_left_aligned(f, text, rect);
  case text_alignment::center:
    return draw_text_centered(f, text, rect);
  case text_alignment::right:
    return draw_text_right_aligned(f, text, rect);
  }

  return draw_text_centered(f, text, rect);
}

void context::draw_text_left_aligned(font& f, const std::u32string& text, const mts::rect<float>& rect) {
  const float font_height = f.is_valid() ? f.get_height() : k_default_mac_font_height;

  CGContextRef g = get_gc_context(_native_context);

  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  //  dnft::print("NMNMNMMN", text, mts::utf8::utf32to8(text));
  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_u32string(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CGContextSetTextDrawingMode(g, kCGTextFill);

  CGContextSetTextMatrix(g, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, font_height));
  CGContextSetTextPosition(g, rect.x, rect.y + (rect.height + font_height) * 0.5);
  CTLineDraw(line, g);
}
void context::draw_text_left_aligned(font& f, const std::string& text, const mts::rect<float>& rect) {
  const float font_height = f.is_valid() ? f.get_height() : k_default_mac_font_height;

  CGContextRef g = get_gc_context(_native_context);

  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CGContextSetTextDrawingMode(g, kCGTextFill);

  CGContextSetTextMatrix(g, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, font_height));
  CGContextSetTextPosition(g, rect.x, rect.y + (rect.height + font_height) * 0.5);
  CTLineDraw(line, g);
}

void context::draw_text_right_aligned(font& f, const std::string& text, const mts::rect<float>& rect) {
  const float font_height = f.is_valid() ? f.get_height() : k_default_mac_font_height;

  CGContextRef g = get_gc_context(_native_context);

  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CGContextSetTextDrawingMode(g, kCGTextFill);

  float text_width = CTLineGetTypographicBounds(line, nullptr, nullptr, nullptr);
  CGContextSetTextMatrix(g, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, font_height));

  mts::point<float> pos = rect.position + mts::point<float>(rect.width - text_width, (rect.height + font_height) * 0.5f);
  CGContextSetTextPosition(g, pos.x, pos.y);

  CTLineDraw(line, g);
}

void context::draw_text_centered(font& f, const std::string& text, const mts::rect<float>& rect) {
  const float font_height = f.is_valid() ? f.get_height() : k_default_mac_font_height;

  CGContextRef g = get_gc_context(_native_context);

  mts::cf_unique_ptr<CFDictionaryRef> attributes
      = mts::create_cf_dictionary({ kCTFontAttributeName, kCTForegroundColorFromContextAttributeName },
          { (CTFontRef)f.get_native_font(), kCFBooleanTrue });

  mts::cf_unique_ptr<CFStringRef> str = mts::create_cf_string_from_string(text);

  mts::cf_unique_ptr<CFAttributedStringRef> attr_str
      = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);

  mts::cf_unique_ptr<CTLineRef> line = CTLineCreateWithAttributedString(attr_str);

  CGContextSetTextDrawingMode(g, kCGTextFill);

  float text_width = CTLineGetTypographicBounds(line, nullptr, nullptr, nullptr);
  CGContextSetTextMatrix(g, CGAffineTransformMake(1.0f, 0.0f, 0.0f, -1.0f, 0.0f, font_height));

  mts::point<float> pos = rect.position + mts::point<float>(rect.width - text_width, rect.height + font_height) * 0.5f;
  CGContextSetTextPosition(g, pos.x, pos.y);
  CTLineDraw(line, g);
}
MTS_END_NAMESPACE

#endif // MTS_MACOS
