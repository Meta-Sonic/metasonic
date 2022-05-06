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
#include "mts/assert.h"
#include "mts/pointer.h"
#include "mts/traits.h"
#include "mts/memory_range.h"
#include "mts/graphics/context.h"
#include <memory>
#include <vector>

MTS_BEGIN_NAMESPACE
class geometry {
public:
  ~geometry() = default;

  inline bool intersect(const mts::rect<float>& rect) const { return _intersect(this, rect); }
  inline bool intersect(const mts::point<float>& pos) const {
    return _intersect(this, mts::rect<float>(pos, { 1, 1 }));
  }
  inline void draw(context& g) const { _draw(this, g); }
  inline void draw(context& g, const mts::rect<float>& rect) const { _draw_dirty(this, g, rect); }
  inline void free() { _free(this); }

protected:
  using intersect_function = bool (*)(const geometry*, const mts::rect<float>&);
  using draw_function = void (*)(const geometry*, context&);
  using draw_dirty_function = void (*)(const geometry*, context&, const mts::rect<float>&);
  using free_function = void (*)(geometry*);

  inline void set_intersect(intersect_function fct) { _intersect = fct; }
  inline void set_draw(draw_function fct) { _draw = fct; }
  inline void set_draw_dirty(draw_dirty_function fct) { _draw_dirty = fct; }
  inline void set_free(free_function fct) { _free = fct; }

  template <typename T, typename... Args>
  void init(T* obj, Args&&... args) {
    using content_type = typename T::content;
    mts_assert(reinterpret_cast<void*>(obj->_content) == reinterpret_cast<void*>(obj + 1), "You fucked up.");

    new (obj->_content) content_type(std::forward<Args>(args)...);

    obj->set_intersect([](const geometry* g, const mts::rect<float>& rect) {
      const T* fr = static_cast<const T*>(g);
      return fr->_content->intersect(rect);
    });

    obj->set_draw([](const geometry* g, context& context) {
      const T* fr = static_cast<const T*>(g);
      return fr->_content->draw(context);
    });

    if constexpr (has_draw_dirty<content_type>::value) {
      obj->set_draw_dirty([](const geometry* g, context& context, const mts::rect<float>& rect) {
        const T* fr = static_cast<const T*>(g);
        return fr->_content->draw(context, rect);
      });
    }
    else {
      obj->set_draw_dirty([](const geometry* g, context& context, const mts::rect<float>& rect) {
        mts::unused(rect);
        const T* fr = static_cast<const T*>(g);
        return fr->_content->draw(context);
      });
    }

    obj->set_free([](geometry* g) {
      T* fr_ptr = reinterpret_cast<T*>(g);
      mts_assert(reinterpret_cast<void*>(fr_ptr->_content) == reinterpret_cast<void*>(fr_ptr + 1), "You fucked up.");
      fr_ptr->_content->~content();
      std::free(fr_ptr);
    });
  }

  template <typename T>
  static void* allocate() {
    using content_type = typename T::content;
    void* ret = std::malloc(sizeof(T) + sizeof(content_type));
    T* obj = reinterpret_cast<T*>(ret);
    obj->_content = reinterpret_cast<content_type*>(obj + 1);
    return ret;
  }

private:
  intersect_function _intersect = nullptr;
  draw_function _draw = nullptr;
  draw_dirty_function _draw_dirty = nullptr;
  free_function _free = nullptr;

  template <class T>
  using draw_dirty_t
      = decltype(std::declval<T>().draw(std::declval<context&>(), std::declval<const mts::rect<float>&>()));

  template <typename T>
  using has_draw_dirty = mts::is_detected<draw_dirty_t, T>;
};

class geometry_buffer {
public:
  struct allocator_tag {};

  geometry_buffer() = default;
  ~geometry_buffer() = default;

  template <typename T, typename... Args>
  inline T* add(Args&&... args) {
    return static_cast<T*>(
        _geometries.emplace_back(data_type(new (allocator_tag{}) T(std::forward<Args>(args)...))).get());
  }

  inline std::size_t size() const { return _geometries.size(); }
  inline bool is_empty() const { return _geometries.empty(); }

  inline void clear() { _geometries.clear(); }
  inline void reset() { _geometries = vector_type(); }

  inline geometry* operator[](std::size_t index) { return _geometries[index].get(); }
  inline const geometry* operator[](std::size_t index) const { return _geometries[index].get(); }

  inline void erase(std::size_t index) { _geometries.erase(_geometries.begin() + index); }

  inline void erase(const geometry* obj) {
    for (std::size_t i = 0; i < _geometries.size(); i++) {
      if (obj == _geometries[i].get()) {
        erase(i);
        return;
      }
    }
  }

  inline void draw(context& g) const {
    for (const auto& n : _geometries) {
      n->draw(g);
    }
  }

  inline void draw(context& g, const mts::rect<float>& dirty_rect) const {
    for (const auto& n : _geometries) {
      if (n->intersect(dirty_rect)) {
        n->draw(g, dirty_rect);
      }
    }
  }

  inline void draw(context& g, const mts::memory_range<const mts::rect<float>>& dirty_rects) const {
    for (const auto& n : _geometries) {
      for (const auto& r : dirty_rects) {
        if (n->intersect(r)) {
          n->draw(g);
          break;
        }
      }
    }
  }

private:
  struct deleter {
    deleter() = default;
    deleter(const deleter&) = default;
    deleter(deleter&&) = default;
    deleter& operator=(const deleter&) = default;
    deleter& operator=(deleter&&) = default;
    inline void operator()(geometry* g) const { g->free(); };
  };

  using data_type = std::unique_ptr<geometry, deleter>;
  using vector_type = std::vector<data_type>;

  vector_type _geometries;
};

#define MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS                                                          \
  class content;                                                                                                       \
  content* _content;                                                                                                   \
  friend class mts::geometry;                                                                                          \
  friend class mts::geometry_buffer;                                                                                   \
  static void* operator new(std::size_t size, mts::geometry_buffer::allocator_tag);                                    \
  static void* operator new(size_t) = delete;                                                                          \
  static void operator delete(void* ptr) = delete

namespace shape {
/// Rectangle.
class rectangle : public geometry {
public:
  void set(const mts::rect<float>& rect, const mts::color& color);
  void set_rect(const mts::rect<float>& rect);
  void set_color(const mts::color& color);

  mts::rect<float> get_rect() const;
  mts::color get_color() const;

private:
  rectangle(const mts::rect<float>& rect, const mts::color& color);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Rectangle contour.
class rectangle_contour : public geometry {
public:
  void set_rect(const mts::rect<float>& rect);
  void set_color(const mts::color& color);
  void set_line_width(float w);

  mts::rect<float> get_rect() const;
  mts::color get_color() const;
  float get_line_width() const;

private:
  rectangle_contour(const mts::rect<float>& rect, const mts::color& color, float line_width);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Filled rectangle with contour.
class filled_rectangle_with_contour : public geometry {
public:
  void set_rect(const mts::rect<float>& rect);
  void set_fill_color(const mts::color& color);
  void set_contour_color(const mts::color& color);
  void set_line_width(float w);

  mts::rect<float> get_rect() const;
  mts::color get_fill_color() const;
  mts::color get_contour_color() const;
  float get_line_width() const;

private:
  filled_rectangle_with_contour(
      const mts::rect<float>& rect, const mts::color& fill_color, const mts::color& contour_color, float line_width);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Filled rounded rectangle.
class filled_rounded_rectangle : public geometry {
public:
  void set(const mts::rect<float>& rect, const mts::color& color, float radius);
  void set_rect(const mts::rect<float>& rect);
  void set_color(const mts::color& color);
  void set_radius(float radius);

  mts::rect<float> get_rect() const;
  mts::color get_color() const;
  float get_radius() const;

private:
  filled_rounded_rectangle(const mts::rect<float>& rect, const mts::color& color, float radius);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Filled rounded rectangle with contour.
class filled_rounded_rectangle_with_contour : public geometry {
public:
  void set_rect(const mts::rect<float>& rect);
  void set_fill_color(const mts::color& color);
  void set_contour_color(const mts::color& color);
  void set_line_width(float w);
  void set_radius(float radius);

  mts::rect<float> get_rect() const;
  mts::color get_fill_color() const;
  mts::color get_contour_color() const;
  float get_line_width() const;
  float get_radius() const;

private:
  filled_rounded_rectangle_with_contour(const mts::rect<float>& rect, const mts::color& fill_color,
      const mts::color& contour_color, float line_width, float radius);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Centered text.
class centered_text : public geometry {
public:
  void set_font(mts::not_null<mts::font*> font);
  void set_rect(const mts::rect<float>& rect);
  void set_color(const mts::color& color);
  void set_label(const std::string& label);

  mts::font* get_font() const;
  mts::rect<float> get_rect() const;
  mts::color get_color() const;
  const std::string& get_label() const;

private:
  centered_text(
      mts::not_null<mts::font*> font, const std::string& label, const mts::rect<float>& rect, const mts::color& color);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};

/// Image.
class image : public geometry {
public:
  void set(const mts::rect<float>& rect, const mts::image& img);
  void set_rect(const mts::rect<float>& rect);
  void set_image(const mts::image& img);

  mts::rect<float> get_rect() const;
  const mts::image& get_image() const;

private:
  image(const mts::rect<float>& rect, const mts::image& img);
  MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS;
};
} // namespace shape.

#undef MTS_GEOMETRY_DECLARE_CONTENT_AND_ALLOCATION_OPERATORS
MTS_END_NAMESPACE
