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
#include "mts/exception.h"
#include "mts/pointer.h"
#include "mts/graphics/color.h"
#include "mts/graphics/rect.h"
#include <iostream>
#include <string>
#include <string_view>

MTS_BEGIN_NAMESPACE
class object;

class property {
public:
  enum class type : std::uint8_t {
    empty,
    real,
    integer,
    uinteger,
    string_view,
    point,
    fpoint,
    size,
    fsize,
    rect,
    frect,
    color,
    pointer,
    object
  };

  inline property() noexcept
      : _type(type::empty) {}

  inline property(std::nullptr_t) noexcept
      : _type(type::empty) {}

  inline property(std::string_view s) noexcept
      : _type(type::string_view) {
    _content.svalue.data = s.data();
    _content.svalue.size = s.size();
  }

  inline property(float v) noexcept
      : _type(type::real) {
    _content.dvalue = v;
  }

  inline property(double v) noexcept
      : _type(type::real) {
    _content.dvalue = v;
  }

  inline property(int v) noexcept
      : _type(type::integer) {
    _content.ivalue = v;
  }

  inline property(long v) noexcept
      : _type(type::integer) {
    _content.ivalue = v;
  }

  inline property(unsigned int v) noexcept
      : _type(type::uinteger) {
    _content.uvalue = v;
  }

  inline property(unsigned long v) noexcept
      : _type(type::uinteger) {
    _content.uvalue = v;
  }

  inline property(void* p) noexcept
      : _type(type::pointer) {
    _content.pvalue = p;
  }

  inline property(object* o) noexcept
      : _type(type::object) {
    _content.ovalue = o;
  }

  inline property(const mts::point<int>& p) noexcept
      : _type(type::point) {
    _content.ptvalue = { p.x, p.y };
  }

  inline property(const mts::point<float>& p) noexcept
      : _type(type::fpoint) {
    _content.ptfvalue = { p.x, p.y };
  }

  inline property(const mts::size<int>& s) noexcept
      : _type(type::size) {
    _content.szvalue = { s.width, s.height };
  }

  inline property(const mts::size<float>& s) noexcept
      : _type(type::fsize) {
    _content.szfvalue = { s.width, s.height };
  }

  inline property(const mts::rect<int>& r) noexcept
      : _type(type::rect) {
    _content.rvalue = { r.x, r.y, r.width, r.height };
  }

  inline property(const mts::rect<float>& r) noexcept
      : _type(type::frect) {
    _content.rfvalue = { r.x, r.y, r.width, r.height };
  }

  inline property(const mts::color& c) noexcept
      : _type(type::color) {
    _content.cvalue = c.rgba();
  }

  static inline property create_int(int v) noexcept { return property((long)v); }
  static inline property create_int(long v) noexcept { return property(v); }
  static inline property create_uint(unsigned int v) noexcept { return property((unsigned long)v); }
  static inline property create_uint(unsigned long v) noexcept { return property(v); }
  static inline property create_float(float v) noexcept { return property(v); }
  static inline property create_float(double v) noexcept { return property(v); }
  static inline property create_pointer(void* v) noexcept { return property((void*)v); }
  static inline property create_object(object* v) noexcept { return property(v); }
  static inline property create_point(const mts::point<int>& p) noexcept { return property(p); }
  static inline property create_point(const mts::point<float>& p) noexcept { return property(p); }
  static inline property create_size(const mts::size<int>& s) noexcept { return property(s); }
  static inline property create_size(const mts::size<float>& s) noexcept { return property(s); }
  static inline property create_rect(const mts::rect<int>& r) noexcept { return property(r); }
  static inline property create_rect(const mts::rect<float>& r) noexcept { return property(r); }
  static inline property create_color(const mts::color& c) noexcept { return property(c); }
  static inline property create_string(std::string_view s) noexcept { return property(s); }
  static inline property create_string(const char* s) noexcept { return property(std::string_view(s)); }

  template <typename T = void>
  static inline property create_pointer(T* v) noexcept {
    return property((void*)v);
  }

  ~property() noexcept = default;

  property(const property&) noexcept = default;
  property(property&&) noexcept = default;

  property& operator=(const property&) noexcept = default;
  property& operator=(property&&) noexcept = default;

  inline enum type type() const noexcept { return _type; }

  template <typename T = void>
  inline T value() const;

  template <typename T = void>
  inline bool is_convertible() const noexcept;

  inline bool is_valid() const noexcept { return _type != type::empty; }
  inline bool is_empty() const noexcept { return _type == type::empty; }
  inline bool is_real() const noexcept { return _type == type::real; }
  inline bool is_integer() const noexcept { return _type == type::integer; }
  inline bool is_uinteger() const noexcept { return _type == type::uinteger; }
  inline bool is_string() const noexcept { return _type == type::string_view; }
  inline bool is_point() const noexcept { return _type == type::point; }
  inline bool is_fpoint() const noexcept { return _type == type::fpoint; }
  inline bool is_size() const noexcept { return _type == type::size; }
  inline bool is_fsize() const noexcept { return _type == type::fsize; }
  inline bool is_rect() const noexcept { return _type == type::rect; }
  inline bool is_frect() const noexcept { return _type == type::frect; }
  inline bool is_color() const noexcept { return _type == type::color; }
  inline bool is_pointer() const noexcept { return _type == type::pointer; }
  inline bool is_object() const noexcept { return _type == type::object; }

  inline std::string_view get_string() const noexcept {
    return is_string() ? std::string_view(_content.svalue.data, _content.svalue.size) : std::string_view();
  }

  inline object* check_object() const noexcept { return is_object() ? value<object*>() : nullptr; }

  inline mts::point<int> get_point() const noexcept { return { _content.ptvalue.x, _content.ptvalue.y }; }
  inline mts::point<float> get_fpoint() const noexcept { return { _content.ptfvalue.x, _content.ptfvalue.y }; }

  inline mts::size<int> get_size() const noexcept { return { _content.szvalue.width, _content.szvalue.height }; }
  inline mts::size<float> get_fsize() const noexcept { return { _content.szfvalue.width, _content.szfvalue.height }; }

  inline mts::rect<int> get_rect() const noexcept {
    return { _content.rvalue.x, _content.rvalue.y, _content.rvalue.width, _content.rvalue.height };
  }

  inline mts::rect<float> get_frect() const noexcept {
    return { _content.rfvalue.x, _content.rfvalue.y, _content.rfvalue.width, _content.rfvalue.height };
  }

  inline mts::color get_color() const noexcept { return _content.cvalue; }

  friend std::ostream& operator<<(std::ostream& stream, const property& msg);

private:
  union content {
    double dvalue; // floating_point.
    long ivalue; // integer.
    unsigned long uvalue; // uinteger.
    std::uint32_t cvalue; // color.
    void* pvalue; // pointer.
    object* ovalue; // object.

    struct point {
      int x, y;
    } ptvalue; // point.

    struct fpoint {
      float x, y;
    } ptfvalue; // point.

    struct size {
      int width, height;
    } szvalue; // size.

    struct fsize {
      float width, height;
    } szfvalue; // size.

    struct rect {
      int x, y, width, height;
    } rvalue; // rect.

    struct frect {
      float x, y, width, height;
    } rfvalue; // rect.

    struct string {
      const char* data;
      std::size_t size;
    } svalue; // string.

  } _content;

  enum type _type;
  [[maybe_unused]] std::uint8_t _reserved_1;
  [[maybe_unused]] std::uint8_t _reserved_2;
  [[maybe_unused]] std::uint8_t _reserved_3;
};

template <typename T>
inline T property::value() const {
#define CHECK_TYPE(...)                                                                                                \
  mts_assert(mts::is_one_of(_type, __VA_ARGS__), "property Wrong value type.");                                        \
  if (!mts::is_one_of(_type, __VA_ARGS__)) {                                                                           \
    mts::throw_exception(std::invalid_argument("property::value wrong value type"));                                   \
  }

  if constexpr (std::is_same_v<T, void>) {
    return;
  }

  else if constexpr (std::is_floating_point_v<T>) {
    CHECK_TYPE(type::real, type::integer, type::uinteger)
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::integer:
      return (T)_content.ivalue;
    case type::uinteger:
      return (T)_content.uvalue;
    default:
      return (T)_content.dvalue;
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_integral_v<T>) {
    CHECK_TYPE(type::real, type::integer, type::uinteger);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::real:
      return (T)_content.dvalue;
    case type::uinteger:
      return (T)_content.uvalue;
    default:
      return (T)_content.ivalue;
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_unsigned_v<T>) {
    CHECK_TYPE(type::real, type::integer, type::uinteger);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::real:
      return (T)_content.dvalue;
    case type::integer:
      return (T)_content.ivalue;
    default:
      return (T)_content.uvalue;
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, object*>) {
    CHECK_TYPE(type::object);
    return _content.ovalue;
  }

  else if constexpr (std::is_same_v<T, mts::point<int>>) {
    CHECK_TYPE(type::point, type::fpoint);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::fpoint:
      return mts::point<int>(_content.ptfvalue.x, _content.ptfvalue.y);
    default:
      return mts::point<int>(_content.ptvalue.x, _content.ptvalue.y);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, mts::point<float>>) {
    CHECK_TYPE(type::point, type::fpoint);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::point:
      return mts::point<float>(_content.ptvalue.x, _content.ptvalue.y);
    default:
      return mts::point<float>(_content.ptfvalue.x, _content.ptfvalue.y);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, mts::size<int>>) {
    CHECK_TYPE(type::size, type::fsize);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::fsize:
      return mts::size<int>(_content.szfvalue.width, _content.szfvalue.height);
    default:
      return mts::size<int>(_content.szvalue.width, _content.szvalue.height);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, mts::size<float>>) {
    CHECK_TYPE(type::size, type::fsize);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::size:
      return mts::size<float>(_content.szvalue.width, _content.szvalue.height);
    default:
      return mts::size<float>(_content.szfvalue.width, _content.szfvalue.height);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }
  else if constexpr (std::is_same_v<T, mts::rect<int>>) {
    CHECK_TYPE(type::rect, type::frect);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::frect:
      return mts::rect<int>(_content.rfvalue.x, _content.rfvalue.y, _content.rfvalue.width, _content.rfvalue.height);
    default:
      return mts::rect<int>(_content.rvalue.x, _content.rvalue.y, _content.rvalue.width, _content.rvalue.height);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, mts::rect<float>>) {
    CHECK_TYPE(type::rect, type::frect);
    MTS_BEGIN_DISABLE_ENUM_WARNING
    switch (_type) {
    case type::rect:
      return mts::rect<float>(_content.rvalue.x, _content.rvalue.y, _content.rvalue.width, _content.rvalue.height);
    default:
      return mts::rect<float>(_content.rfvalue.x, _content.rfvalue.y, _content.rfvalue.width, _content.rfvalue.height);
    }
    MTS_END_DISABLE_ENUM_WARNING
  }

  else if constexpr (std::is_same_v<T, mts::color>) {
    CHECK_TYPE(type::color);
    return mts::color(_content.cvalue);
  }

  else if constexpr (std::is_pointer_v<T>) {
    CHECK_TYPE(type::pointer);
    return (T)_content.pvalue;
  }

  else if constexpr (std::is_same_v<T, std::string>) {
    CHECK_TYPE(type::string_view);
    return std::string(_content.svalue.data, _content.svalue.size);
  }

  else if constexpr (std::is_same_v<T, std::string_view>) {
    CHECK_TYPE(type::string_view);
    return std::string_view(_content.svalue.data, _content.svalue.size);
  }

  mts::throw_exception(std::invalid_argument("property::value wrong value type"));
  return T();

#undef CHECK_TYPE
}

template <typename T>
inline bool property::is_convertible() const noexcept {

  if constexpr (std::is_floating_point_v<T> || std::is_integral_v<T> || std::is_unsigned_v<T>) {
    return mts::is_one_of(_type, type::real, type::integer, type::uinteger);
  }

  else if constexpr (std::is_same_v<T, object*>) {
    return _type == type::object;
  }

  else if constexpr (std::is_same_v<T, mts::point<int>> || std::is_same_v<T, mts::point<float>>) {
    return mts::is_one_of(_type, type::point, type::fpoint);
  }

  else if constexpr (std::is_same_v<T, mts::size<int>> || std::is_same_v<T, mts::size<float>>) {
    return mts::is_one_of(_type, type::size, type::fsize);
  }

  else if constexpr (std::is_same_v<T, mts::rect<int>> || std::is_same_v<T, mts::rect<float>>) {
    return mts::is_one_of(_type, type::rect, type::frect);
  }

  else if constexpr (std::is_same_v<T, mts::color>) {
    return _type == type::color;
  }

  else if constexpr (std::is_pointer_v<T>) {
    return _type == type::pointer;
  }

  else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
    return _type == type::string_view;
  }

  return false;
}
MTS_END_NAMESPACE
