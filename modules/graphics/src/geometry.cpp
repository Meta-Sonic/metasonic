#include "mts/graphics/geometry.h"

MTS_BEGIN_NAMESPACE
namespace shape {
//
// Rectangle.
//
class rectangle::content {
public:
  content(const mts::rect<float>& rect, const mts::color& color)
      : _rect(rect)
      , _color(color) {}

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_fill_color(_color);
    g.fill_rect(_rect);
  }

  inline void draw(context& g, const mts::rect<float>& rect) const {
    g.set_fill_color(_color);
    g.fill_rect(_rect.get_intersection(rect));
  }

  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_color(const mts::color& color) { _color = color; }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_color() const { return _color; }

private:
  mts::rect<float> _rect;
  mts::color _color;
};

rectangle::rectangle(const mts::rect<float>& rect, const mts::color& color) { geometry::init(this, rect, color); }

void* rectangle::operator new(std::size_t, geometry_buffer::allocator_tag) { return geometry::allocate<rectangle>(); }

void rectangle::set(const mts::rect<float>& rect, const mts::color& color) {
  _content->set_rect(rect);
  _content->set_color(color);
}

void rectangle::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void rectangle::set_color(const mts::color& color) { _content->set_color(color); }

mts::rect<float> rectangle::get_rect() const { return _content->get_rect(); }
mts::color rectangle::get_color() const { return _content->get_color(); }

//
// Rectangle contour.
//
class rectangle_contour::content {
public:
  content(const mts::rect<float>& rect, const mts::color& color, float line_width)
      : _rect(rect)
      , _color(color)
      , _line_width(line_width) {}

  ~content() = default;

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_stroke_color(_color);
    g.stroke_rect(_rect, _line_width);
  }

  inline void draw(context& g, const mts::rect<float>& rect) const {
    g.set_stroke_color(_color);
    g.stroke_rect(_rect.get_intersection(rect), _line_width);
  }

  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_color(const mts::color& color) { _color = color; }
  inline void set_line_width(float w) { _line_width = w; }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_color() const { return _color; }
  inline float get_line_width() const { return _line_width; }

private:
  mts::rect<float> _rect;
  mts::color _color;
  float _line_width;
};

rectangle_contour::rectangle_contour(const mts::rect<float>& rect, const mts::color& color, float line_width) {
  geometry::init(this, rect, color, line_width);
}

void* rectangle_contour::operator new(std::size_t, geometry_buffer::allocator_tag) {
  return geometry::allocate<rectangle_contour>();
}

void rectangle_contour::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void rectangle_contour::set_color(const mts::color& color) { _content->set_color(color); }
void rectangle_contour::set_line_width(float w) { _content->set_line_width(w); }

mts::rect<float> rectangle_contour::get_rect() const { return _content->get_rect(); }
mts::color rectangle_contour::get_color() const { return _content->get_color(); }
float rectangle_contour::get_line_width() const { return _content->get_line_width(); }

//
// Rectangle filled with contour.
//
class filled_rectangle_with_contour::content {
public:
  content(const mts::rect<float>& rect, const mts::color& fill_color, const mts::color& contour_color, float line_width)
      : _rect(rect)
      , _fill_color(fill_color)
      , _contour_color(contour_color)
      , _line_width(line_width) {}

  ~content() = default;

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_fill_color(_fill_color);
    g.fill_rect(_rect);

    g.set_stroke_color(_contour_color);
    g.stroke_rect(_rect, _line_width);
  }

  inline void draw(context& g, const mts::rect<float>& rect) const {
    mts::rect<float> irect = _rect.get_intersection(rect);
    g.set_fill_color(_fill_color);
    g.fill_rect(irect);

    g.set_stroke_color(_contour_color);
    g.stroke_rect(irect, _line_width);
  }

  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_fill_color(const mts::color& color) { _fill_color = color; }
  inline void set_contour_color(const mts::color& color) { _contour_color = color; }
  inline void set_line_width(float w) { _line_width = w; }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_fill_color() const { return _fill_color; }
  inline const mts::color& get_contour_color() const { return _contour_color; }
  inline float get_line_width() const { return _line_width; }

private:
  mts::rect<float> _rect;
  mts::color _fill_color;
  mts::color _contour_color;
  float _line_width;
};

filled_rectangle_with_contour::filled_rectangle_with_contour(
    const mts::rect<float>& rect, const mts::color& fill_color, const mts::color& contour_color, float line_width) {
  geometry::init(this, rect, fill_color, contour_color, line_width);
}

void* filled_rectangle_with_contour::operator new(std::size_t, geometry_buffer::allocator_tag) {
  return geometry::allocate<filled_rectangle_with_contour>();
}

void filled_rectangle_with_contour::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void filled_rectangle_with_contour::set_fill_color(const mts::color& color) { _content->set_fill_color(color); }
void filled_rectangle_with_contour::set_contour_color(const mts::color& color) { _content->set_contour_color(color); }
void filled_rectangle_with_contour::set_line_width(float w) { _content->set_line_width(w); }

mts::rect<float> filled_rectangle_with_contour::get_rect() const { return _content->get_rect(); }
mts::color filled_rectangle_with_contour::get_fill_color() const { return _content->get_fill_color(); }
mts::color filled_rectangle_with_contour::get_contour_color() const { return _content->get_contour_color(); }
float filled_rectangle_with_contour::get_line_width() const { return _content->get_line_width(); }

//
// Filled rounded rectangle.
//
class filled_rounded_rectangle::content {
public:
  content(const mts::rect<float>& rect, const mts::color& color, float radius)
      : _rect(rect)
      , _color(color)
      , _radius(radius) {}

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_fill_color(_color);
    mts::path p;
    p.add_rounded_rect(_rect, _radius);
    g.fill_path(p);
  }

  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_color(const mts::color& color) { _color = color; }
  inline void set_radius(float radius) { _radius = radius; }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_color() const { return _color; }
  inline float get_radius() const { return _radius; }

private:
  mts::rect<float> _rect;
  mts::color _color;
  float _radius;
};

filled_rounded_rectangle::filled_rounded_rectangle(
    const mts::rect<float>& rect, const mts::color& color, float radius) {
  geometry::init(this, rect, color, radius);
}

void* filled_rounded_rectangle::operator new(std::size_t, geometry_buffer::allocator_tag) {
  return geometry::allocate<filled_rounded_rectangle>();
}

void filled_rounded_rectangle::set(const mts::rect<float>& rect, const mts::color& color, float radius) {
  _content->set_rect(rect);
  _content->set_color(color);
  _content->set_radius(radius);
}

void filled_rounded_rectangle::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void filled_rounded_rectangle::set_color(const mts::color& color) { _content->set_color(color); }
void filled_rounded_rectangle::set_radius(float radius) { _content->set_radius(radius); }

mts::rect<float> filled_rounded_rectangle::get_rect() const { return _content->get_rect(); }
mts::color filled_rounded_rectangle::get_color() const { return _content->get_color(); }
float filled_rounded_rectangle::get_radius() const { return _content->get_radius(); }

//
// Rounded rectangle filled with contour.
//
class filled_rounded_rectangle_with_contour::content {
public:
  content(const mts::rect<float>& rect, const mts::color& fill_color, const mts::color& contour_color, float line_width,
      float radius)
      : _rect(rect)
      , _fill_color(fill_color)
      , _contour_color(contour_color)
      , _line_width(line_width)
      , _radius(radius) {

    update_path();
  }

  ~content() = default;

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_fill_color(_fill_color);
    g.fill_path(_path);

    g.set_stroke_color(_contour_color);
    g.set_line_width(_line_width);
    g.stroke_path(_path);
  }

  inline void set_rect(const mts::rect<float>& rect) {
    _rect = rect;
    update_path();
  }

  inline void set_fill_color(const mts::color& color) { _fill_color = color; }
  inline void set_contour_color(const mts::color& color) { _contour_color = color; }
  inline void set_line_width(float w) { _line_width = w; }
  inline void set_radius(float radius) {
    _radius = radius;
    update_path();
  }

  inline void update_path() {
    if (!_path.is_empty()) {
      _path = mts::path();
    }

    _path.add_rounded_rect(_rect, _radius);
  }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_fill_color() const { return _fill_color; }
  inline const mts::color& get_contour_color() const { return _contour_color; }
  inline float get_line_width() const { return _line_width; }
  inline float get_radius() const { return _radius; }

private:
  mts::path _path;
  mts::rect<float> _rect;
  mts::color _fill_color;
  mts::color _contour_color;
  float _line_width;
  float _radius;
};

filled_rounded_rectangle_with_contour::filled_rounded_rectangle_with_contour(const mts::rect<float>& rect,
    const mts::color& fill_color, const mts::color& contour_color, float line_width, float radius) {
  geometry::init(this, rect, fill_color, contour_color, line_width, radius);
}

void* filled_rounded_rectangle_with_contour::operator new(std::size_t, geometry_buffer::allocator_tag) {
  return geometry::allocate<filled_rounded_rectangle_with_contour>();
}

void filled_rounded_rectangle_with_contour::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void filled_rounded_rectangle_with_contour::set_fill_color(const mts::color& color) { _content->set_fill_color(color); }
void filled_rounded_rectangle_with_contour::set_contour_color(const mts::color& color) {
  _content->set_contour_color(color);
}
void filled_rounded_rectangle_with_contour::set_line_width(float w) { _content->set_line_width(w); }
void filled_rounded_rectangle_with_contour::set_radius(float radius) { _content->set_radius(radius); }

mts::rect<float> filled_rounded_rectangle_with_contour::get_rect() const { return _content->get_rect(); }
mts::color filled_rounded_rectangle_with_contour::get_fill_color() const { return _content->get_fill_color(); }
mts::color filled_rounded_rectangle_with_contour::get_contour_color() const { return _content->get_contour_color(); }
float filled_rounded_rectangle_with_contour::get_line_width() const { return _content->get_line_width(); }
float filled_rounded_rectangle_with_contour::get_radius() const { return _content->get_radius(); }

//
// Centered text.
//
class centered_text::content {
public:
  content(
      mts::not_null<mts::font*> font, const std::string& label, const mts::rect<float>& rect, const mts::color& color)
      : _font(font)
      , _label(label)
      , _rect(rect)
      , _color(color) {}

  ~content() = default;

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const {
    g.set_fill_color(_color);
    g.draw_text_centered(*_font, _label, _rect);
  }

  inline void set_font(mts::not_null<mts::font*> font) { _font = font; }
  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_color(const mts::color& color) { _color = color; }
  inline void set_label(const std::string& label) { _label = label; }

  inline mts::font* get_font() const { return _font; }
  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::color& get_color() const { return _color; }
  inline const std::string& get_label() const { return _label; }

private:
  mts::font* _font;
  std::string _label;
  mts::rect<float> _rect;
  mts::color _color;
};

centered_text::centered_text(
    mts::not_null<mts::font*> font, const std::string& label, const mts::rect<float>& rect, const mts::color& color) {
  geometry::init(this, font, label, rect, color);
}

void* centered_text::operator new(std::size_t, geometry_buffer::allocator_tag) {
  return geometry::allocate<centered_text>();
}

void centered_text::set_font(mts::not_null<mts::font*> f) { _content->set_font(f); }
void centered_text::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void centered_text::set_color(const mts::color& color) { _content->set_color(color); }
void centered_text::set_label(const std::string& label) { _content->set_label(label); }

mts::font* centered_text::get_font() const { return _content->get_font(); }
mts::rect<float> centered_text::get_rect() const { return _content->get_rect(); }
mts::color centered_text::get_color() const { return _content->get_color(); }
const std::string& centered_text::get_label() const { return _content->get_label(); }

//
// Image.
//
class image::content {
public:
  content(const mts::rect<float>& rect, const mts::image& img)
      : _rect(rect)
      , _img(img) {}

  inline bool intersect(const mts::rect<float>& rect) const { return _rect.intersect(rect); }

  inline void draw(context& g) const { g.draw_image(_img, _rect); }

  inline void draw(context& g, const mts::rect<float>& rect) const { g.draw_image(_img, _rect, rect); }

  inline void set_rect(const mts::rect<float>& rect) { _rect = rect; }
  inline void set_image(const mts::image& img) { _img = img; }

  inline const mts::rect<float>& get_rect() const { return _rect; }
  inline const mts::image& get_image() const { return _img; }

private:
  mts::rect<float> _rect;
  mts::image _img;
};

image::image(const mts::rect<float>& rect, const mts::image& img) { geometry::init(this, rect, img); }

void* image::operator new(std::size_t, geometry_buffer::allocator_tag) { return geometry::allocate<image>(); }

void image::set(const mts::rect<float>& rect, const mts::image& img) {
  _content->set_rect(rect);
  _content->set_image(img);
}

void image::set_rect(const mts::rect<float>& rect) { _content->set_rect(rect); }
void image::set_image(const mts::image& img) { _content->set_image(img); }

mts::rect<float> image::get_rect() const { return _content->get_rect(); }
const mts::image& image::get_image() const { return _content->get_image(); }
} // namespace shape.
MTS_END_NAMESPACE
