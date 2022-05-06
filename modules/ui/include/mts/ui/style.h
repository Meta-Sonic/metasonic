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
#include "mts/ui/styles.h"
#include <any>
#include <map>

MTS_BEGIN_NAMESPACE

#define MTS_STYLE_LIST(_)                                                                                              \
  _(global, COMMA)                                                                                                     \
  _(styled_view, COMMA)                                                                                                \
  _(button, COMMA)                                                                                                     \
  _(toggle, COMMA)                                                                                                     \
  _(combo_box, EMPTY)

#define COMMA ,
#define EMPTY

#define MTS_STYLE_TYPE(NAME, ENDING) MTS_CONCAT(NAME, _style) ENDING
#define MTS_STYLE_ENUM(NAME, ENDING) using NAME = MTS_CONCAT(NAME, _style)::enum_type;

class style : public detail::style_object_descriptor<MTS_STYLE_LIST(MTS_STYLE_TYPE)> {
public:
  MTS_STYLE_LIST(MTS_STYLE_ENUM)
};

#undef MTS_STYLE_LIST
#undef MTS_STYLE_TYPE
#undef MTS_STYLE_ENUM
#undef COMMA
#undef EMPTY

class style_map_base {
public:
  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline void set(const style::value_t<_EnumValue>& v) {
    _properties[get_id<_EnumValue>()] = [value = v]() { return (void*)&value; };
    notify(style::group_name<_EnumValue>(), style::get_name<_EnumValue>());
  }

  template <typename _Style, std::enable_if_t<std::is_base_of_v<mts::style_descriptor_base, _Style>, bool> = true>
  inline void set(const _Style& v) {
    static_assert(std::is_base_of_v<mts::style_descriptor_base, _Style>, "");

    v.iterate([this](auto ev, const auto& v) { set<decltype(ev)::value>(v); });
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline void erase() {
    if (auto it = _properties.find(get_id<_EnumValue>()); it != _properties.end()) {
      _properties.erase(it);
      notify(style::group_name<_EnumValue>(), style::get_name<_EnumValue>());
    }
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline const style::value_t<_EnumValue>& get() const {
    using value_type = style::value_t<_EnumValue>;
    auto it = _properties.find(get_id<_EnumValue>());
    return it == _properties.end() ? default_style().get<_EnumValue>()
                                   : *reinterpret_cast<const value_type*>(it->second());
  }

protected:
  virtual const style& default_style() const = 0;
  virtual void notify(std::string_view group, std::string_view name) { mts::unused(group, name); }

private:
  std::map<std::size_t, std::function<void*()>> _properties;

  struct style_id {
    static inline std::size_t gen_id() {
      static std::size_t __id = 0;
      return ++__id;
    }
  };

  template <auto _EnumValue>
  struct enum_id {
    static inline std::size_t id() {
      static std::size_t __id = style_id::gen_id();
      return __id;
    }
  };

  template <auto _EnumValue>
  inline static std::size_t get_id() {
    return enum_id<_EnumValue>::id();
  }
};

MTS_END_NAMESPACE
