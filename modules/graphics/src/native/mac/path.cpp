#include "mts/config.h"

#ifdef MTS_MACOS

  #include "mts/graphics/path.h"
  #include "mts/assert.h"
  #include "helpers.h"

MTS_BEGIN_NAMESPACE
path::path() { _native_path = CGPathCreateMutable(); }

path::path(native_path np) { _native_path = np; }

path::path(const path& p) {
  mts_assert(p._native_path != nullptr, "Invalid path.");
  _native_path = CGPathCreateMutableCopy((CGPathRef)p._native_path);
}

path::path(path&& p) {
  _native_path = p._native_path;
  p._native_path = nullptr;
}

path::~path() {
  if (_native_path) {
    CGPathRelease((CGPathRef)_native_path);
  }
}

path& path::operator=(const path& p) {
  if (_native_path) {
    CGPathRelease((CGPathRef)_native_path);
    _native_path = nullptr;
  }

  mts_assert(p._native_path != nullptr, "Invalid path.");
  _native_path = CGPathCreateMutableCopy((CGPathRef)p._native_path);
  return *this;
}

path& path::operator=(path&& p) {
  if (_native_path) {
    CGPathRelease((CGPathRef)_native_path);
    _native_path = nullptr;
  }

  _native_path = p._native_path;
  p._native_path = nullptr;
  return *this;
}

bool path::operator==(const path& p) const {
  mts_assert(_native_path != nullptr, "Invalid path.");
  mts_assert(p._native_path != nullptr, "Invalid path.");
  return CGPathEqualToPath((CGPathRef)_native_path, (CGPathRef)p._native_path);
}

bool path::operator!=(const path& p) const { return !operator==(p); }

bool path::is_empty() const { return CGPathIsEmpty((CGPathRef)_native_path); }

mts::point<float> path::get_current_point() const {
  return from_cg_point<mts::point<float>>(CGPathGetCurrentPoint((CGPathRef)_native_path));
}

mts::rect<float> path::get_bounding_box() const {
  mts_assert(_native_path != nullptr, "Invalid path.");
  return from_cg_rect<mts::rect<float>>(CGPathGetBoundingBox((CGPathRef)_native_path));
}

mts::rect<float> path::get_path_bounding_box() const {
  mts_assert(_native_path != nullptr, "Invalid path.");
  return from_cg_rect<mts::rect<float>>(CGPathGetPathBoundingBox((CGPathRef)_native_path));
}

bool path::contains(const mts::point<float>& p) const {
  mts_assert(_native_path != nullptr, "Invalid path.");
  return CGPathContainsPoint((CGPathRef)_native_path, nullptr, to_cg_point(p), false);
}

void path::start_subpath(const mts::point<float>& p) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathMoveToPoint((CGMutablePathRef)_native_path, nullptr, p.x, p.y);
}

void path::close_subpath() {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathCloseSubpath((CGMutablePathRef)_native_path);
}

void path::line_to(const mts::point<float>& p) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddLineToPoint((CGMutablePathRef)_native_path, nullptr, p.x, p.y);
}

void path::quadratic_to(const mts::point<float>& c, const mts::point<float>& p) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddQuadCurveToPoint((CGMutablePathRef)_native_path, nullptr, c.x, c.y, p.x, p.y);
}

void path::curve_to(const mts::point<float>& c1, const mts::point<float>& c2, const mts::point<float>& p) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddCurveToPoint((CGMutablePathRef)_native_path, nullptr, c1.x, c1.y, c2.x, c2.y, p.x, p.y);
}

void path::add_line(const mts::point<float>& p0, const mts::point<float>& p1) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathMoveToPoint((CGMutablePathRef)_native_path, nullptr, p0.x, p0.y);
  CGPathAddLineToPoint((CGMutablePathRef)_native_path, nullptr, p1.x, p1.y);
}

void path::add_rect(const mts::rect<float>& r) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddRect((CGMutablePathRef)_native_path, nullptr, to_cg_rect(r));
}

void path::add_rounded_rect(const mts::rect<float>& r, float radius) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  float m_side = mts::minimum(r.width, r.height);
  CGFloat cradius = mts::minimum(m_side * 0.5f, radius);
  CGPathAddRoundedRect((CGMutablePathRef)_native_path, nullptr, to_cg_rect(r), cradius, cradius);
}

void path::add_rounded_rect(const mts::rect<float>& r, const mts::size<float>& radius, bool top_left, bool top_right,
    bool bottom_left, bool bottom_right) {
  mts::size<float> rads = { mts::minimum(radius.width, r.width * 0.5f), mts::minimum(radius.height, r.height * 0.5f) };
  float cs45x = rads.width * 0.45f;
  float cs45y = rads.height * 0.45f;
  float x2 = r.right();
  float y2 = r.bottom();

  if (top_left) {
    start_subpath({ r.x, r.y + rads.height });
    curve_to({ r.x, r.y + cs45y }, { r.x + cs45x, r.y }, { r.x + rads.width, r.y });
  }
  else {
    start_subpath(r.position);
  }

  if (top_right) {
    line_to({ x2 - rads.width, r.y });
    curve_to({ x2 - cs45x, r.y }, { x2, r.y + cs45y }, { x2, r.y + rads.height });
  }
  else {
    line_to({ x2, r.y });
  }

  if (bottom_right) {
    line_to({ x2, y2 - rads.height });
    curve_to({ x2, y2 - cs45y }, { x2 - cs45x, y2 }, { x2 - rads.width, y2 });
  }
  else {
    line_to({ x2, y2 });
  }

  if (bottom_left) {
    line_to({ r.x + rads.width, y2 });
    curve_to({ r.x + cs45x, y2 }, { r.x, y2 - cs45y }, { r.x, y2 - rads.height });
  }
  else {
    line_to({ r.x, y2 });
  }

  close_subpath();
}

/* Add an ellipse (an oval) inside `rect' to `path'. The ellipse is
   approximated by a sequence of Bézier curves. The center of the ellipse is
   the midpoint of `rect'. If `rect' is square, then the ellipse will be
   circular with radius equal to one-half the width (equivalently, one-half
   the height) of `rect'. If `rect' is rectangular, then the major- and
   minor-axes will be the `width' and `height' of rect. The ellipse forms a
   complete subpath of `path' --- that is, it begins with a "move to" and
   ends with a "close subpath" --- oriented in the clockwise direction. If
   `m' is non-NULL, then the constructed Bézier curves representing the
   ellipse will be transformed by `m' before they are added to `path'. */

// CG_EXTERN void CGPathAddEllipseInRect(CGMutablePathRef cg_nullable path,
//     const CGAffineTransform * __nullable m, CGRect rect)
//     CG_AVAILABLE_STARTING(10.4, 2.0);

void path::add_ellipse(const mts::rect<float>& r) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddEllipseInRect((CGMutablePathRef)_native_path, nullptr, to_cg_rect(r));
}
/* Add an arc of a circle to `path', possibly preceded by a straight line
   segment. The arc is approximated by a sequence of Bézier curves. The
   center of the arc is `(x,y)'; `radius' is its radius. `startAngle' is the
   angle to the first endpoint of the arc, measured counter-clockwise from
   the positive x-axis. `startAngle + delta' is the angle to the second
   endpoint of the arc. If `delta' is positive, then the arc is drawn
   counter-clockwise; if negative, clockwise. `startAngle' and `delta' are
   measured in radians. If `matrix' is non-NULL, then the constructed Bézier
   curves representing the arc will be transformed by `matrix' before they
   are added to the path. */

// CG_EXTERN void CGPathAddRelativeArc(CGMutablePathRef cg_nullable path,
//     const CGAffineTransform * __nullable matrix, CGFloat x, CGFloat y,
//     CGFloat radius, CGFloat startAngle, CGFloat delta)
//     CG_AVAILABLE_STARTING(10.7, 5.0);

void path::add_relative_arc(const mts::point<float>& center, float radius, float start_angle, float angle_delta) {
  mts_assert(_native_path != nullptr, "Invalid path.");
  CGPathAddRelativeArc((CGMutablePathRef)_native_path, nullptr, (CGFloat)center.x, (CGFloat)center.y, (CGFloat)radius,
      (CGFloat)start_angle, (CGFloat)angle_delta);
}

bool path::load_data(mts::byte_view data) {

  std::size_t index = 0;
  while (index < data.size()) {
    std::uint8_t c = data[index++];

    switch (c) {
    case 'm': {
      float x = data.as<float>(index);
      index += sizeof(float);
      float y = data.as<float>(index);
      index += sizeof(float);

      //      mts::print("start", mts::point<float>{ x, y });
      start_subpath({ x, y });
    } break;

    case 'l': {
      float x = data.as<float>(index);
      index += sizeof(float);
      float y = data.as<float>(index);
      index += sizeof(float);
      //      mts::print("line_to", mts::point<float>{ x, y });
      line_to({ x, y });
    } break;

    case 'q': {
      float x = data.as<float>(index);
      index += sizeof(float);
      float y = data.as<float>(index);
      index += sizeof(float);

      float x2 = data.as<float>(index);
      index += sizeof(float);
      float y2 = data.as<float>(index);
      index += sizeof(float);

      //      mts::print("quadratic_to", mts::point<float>{ x, y }, mts::point<float>{ x2, y2 });
      quadratic_to({ x, y }, { x2, y2 });
    } break;

    case 'b': {
      float x1 = data.as<float>(index);
      index += sizeof(float);
      float y1 = data.as<float>(index);
      index += sizeof(float);

      float x2 = data.as<float>(index);
      index += sizeof(float);
      float y2 = data.as<float>(index);
      index += sizeof(float);

      float x3 = data.as<float>(index);
      index += sizeof(float);
      float y3 = data.as<float>(index);
      index += sizeof(float);

      //      mts::print("curve_to", mts::point<float>{ x1, y1 }, mts::point<float>{ x2, y2 }, mts::point<float>{ x3, y3
      //      });
      curve_to({ x1, y1 }, { x2, y2 }, { x3, y3 });

    } break;

    case 'c': {
      //      mts::print("close");
      close_subpath();
    } break;

    case 'n': {
      //      mts::print("N");
      //        close_subpath();
    } break;
      //
    case 'z': {
      //      mts::print("Z");
      //        close_subpath();
    } break;
      //
    case 'e': {
      //      mts::print("E");
      return true;
    } break;

    default:
      mts_error("Path data error");
      return false;
    }
  }
  return true;
}

path path::get_normalized_path() const {
  mts::rect<float> bounds = get_bounding_box();
  CGAffineTransform transform = CGAffineTransformTranslate(
      CGAffineTransformMakeScale(1.0f / bounds.width, 1.0f / bounds.height), -bounds.x, -bounds.y);
  //
  CGMutablePathRef mp = CGPathCreateMutableCopyByTransformingPath((CGPathRef)_native_path, &transform);
  if (mp) {
    return path(mp);
  }

  return path();
}

path path::create_rounded_rect(const mts::rect<float>& r, float radius) {
  path p;
  p.add_rounded_rect(r, radius);
  return p;
}
path path::create_check_mark() {
  path p;

  static constexpr std::array points = { mts::point<float>{ 0, 0.46474 }, mts::point<float>{ 0.192582, 0.39422 },
    mts::point<float>{ 0.461723, 0.788439 }, mts::point<float>{ 0.269142, 0.788439 }, mts::point<float>{ 0.807418, 0 },
    mts::point<float>{ 1, 0.0705201 }, mts::point<float>{ 0.365433, 1 }, mts::point<float>{ 0, 0.46474 } };

  p.start_subpath(points[0]);
  for (std::size_t i = 1; i < points.size(); i++) {
    p.line_to(points[i]);
  }
  p.close_subpath();

  return p;
}

path path::create_down_arrow() {
  path p;

  static constexpr std::array points
      = { mts::point<float>{ 0.8825, 0 }, mts::point<float>{ 0.5, 0.618084 }, mts::point<float>{ 0.1175, 0 },
          mts::point<float>{ 0, 0.190283 }, mts::point<float>{ 0.5, 1 }, mts::point<float>{ 1, 0.190283 } };

  p.start_subpath(points[0]);
  for (std::size_t i = 1; i < points.size(); i++) {
    p.line_to(points[i]);
  }
  p.close_subpath();

  return p;
}
MTS_END_NAMESPACE

#endif // MTS_MACOS
