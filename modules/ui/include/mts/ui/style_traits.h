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
#include "mts/graphics/color.h"
#include "mts/graphics/rect.h"
#include "mts/traits.h"
#include "mts/string_literal.h"
#include <string_view>

MTS_BEGIN_NAMESPACE

template <class T>
struct is_style_enum : public std::false_type {};

template <class T>
inline constexpr bool is_style_enum_v = is_style_enum<T>::value;

template <auto _EnumValue, typename T, string_literal Name>
struct enum_pair {
  using value_type = T;
  static constexpr auto value = _EnumValue;
  static constexpr string_literal name = Name;
};

template <auto _EnumValue, typename T, string_literal Name>
using declare_attribute = enum_pair<_EnumValue, T, Name>;

namespace detail {
template <class T, class Tuple>
struct tuple_index_inf {
  static constexpr std::size_t value = 0;
};

template <class T, class... Types>
struct tuple_index_inf<T, std::tuple<T, Types...>> {
  static constexpr std::size_t value = 0;
};

template <class T, class U, class... Types>
struct tuple_index_inf<T, std::tuple<U, Types...>> {
  static constexpr std::size_t value = 1 + tuple_index_inf<T, std::tuple<Types...>>::value;
};

template <class... Types>
inline constexpr size_t tuple_index_inf_v = tuple_index_inf<Types...>::value;

template <auto _EnumValue>
struct enum_obj {
  using enum_type = decltype(_EnumValue);
  using underlying_type = std::underlying_type_t<enum_type>;
  static constexpr auto value = _EnumValue;
};

template <string_literal Name>
struct name_obj {
  static constexpr auto value = Name;
};

template <typename T>
struct enum_pair_value_type {
  using value_type = typename T::value_type;
};

template <typename T>
using enum_pair_value_type_t = typename enum_pair_value_type<T>::value_type;

template <typename T>
struct enum_pair_enum_value {
  static constexpr auto value = T::value;
};

template <typename T>
inline constexpr auto enum_pair_enum_value_v = enum_pair_enum_value<T>::value;

template <typename T>
struct enum_pair_name_value {
  static constexpr auto value = T::name;
};

template <typename T>
inline constexpr auto enum_pair_name_value_v = enum_pair_name_value<T>::value;

template <typename... Args>
struct style_meta {
  using descriptor = std::tuple<Args...>;
  static constexpr std::size_t count = std::tuple_size<descriptor>::value;
  using values = std::tuple<enum_pair_value_type_t<Args>...>;
  using indexes = std::tuple<enum_obj<enum_pair_enum_value_v<Args>>...>;
  using enum_type = typename std::tuple_element_t<0, indexes>::enum_type;
  using names = std::tuple<name_obj<enum_pair_name_value_v<Args>>...>;
};

template <typename T>
struct style_obj_enum_value {
  using enum_type = typename T::enum_type;
};

template <typename T>
using style_obj_enum_value_t = typename style_obj_enum_value<T>::enum_type;

template <typename... Args>
struct style_object_descriptor {
  using descriptor = std::tuple<Args...>;
  using indexes = std::tuple<style_obj_enum_value_t<Args>...>;

  template <auto _EnumValue>
  static inline constexpr size_t get_index() {
    return (size_t)tuple_index_v<decltype(_EnumValue), indexes>;
  }

  template <auto _EnumValue>
  using value_t = typename std::tuple_element_t<get_index<_EnumValue>(), descriptor>::template value_t<_EnumValue>;

  template <auto _EnumValue, typename T>
  void set(const T& v) {
    std::get<get_index<_EnumValue>()>(objs).template set<_EnumValue>(v);
  }

  template <typename _EnumType, string_literal Name, typename T>
  void set(const T& v) {
    std::get<tuple_index_v<_EnumType, indexes>>(objs).template set<Name>(v);
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  const auto& get() const {
    return std::get<get_index<_EnumValue>()>(objs).template get<_EnumValue>();
  }

  template <typename _EnumType, string_literal Name>
  const auto& get() const {
    return std::get<(size_t)tuple_index_v<_EnumType, indexes>>(objs).template get<Name>();
  }

  template <auto _EnumValue>
  static constexpr std::string_view get_name() {
    return std::tuple_element_t<get_index<_EnumValue>(), descriptor>::template get_name<_EnumValue>();
  }

  template <auto _EnumValue>
  static constexpr std::string_view group_name() {
    return std::tuple_element_t<get_index<_EnumValue>(), descriptor>::group.value;
  }

  descriptor objs;
};
} // namespace detail.

struct style_descriptor_base {};

template <string_literal _GroupName, typename... Atts>
class style_descriptor : public style_descriptor_base {
public:
  using meta = detail::style_meta<Atts...>;
  using enum_type = typename meta::enum_type;
  using values = typename meta::values;
  using indexes = typename meta::indexes;
  using names = typename meta::names;
  static constexpr string_literal group = _GroupName;

  // private:
  template <auto _EnumValue>
  static inline constexpr size_t get_index() {
    return (size_t)tuple_index_v<detail::enum_obj<_EnumValue>, indexes>;
  }

  template <string_literal Name>
  static inline constexpr size_t get_name_index() {
    return (size_t)tuple_index_v<detail::name_obj<Name>, names>;
  }

  template <size_t K>
  static constexpr bool find_name(std::string_view name) {
    if constexpr (K == meta::count) {
      return false;
    }
    else {
      return std::tuple_element_t<K, names>::value.value == name || find_name<K + 1>(name);
    }
  }

public:
  template <auto _EnumValue>
  using value_t = std::tuple_element_t<get_index<_EnumValue>(), values>;

  template <typename Fct>
  void iterate(Fct&& fct) const {
    iterate_impl<0>(std::move(fct));
  }

  template <string_literal Name>
  static constexpr bool has() {
    return detail::tuple_index_inf_v<detail::name_obj<Name>, names> < meta::count;
  }

  static constexpr bool has(std::string_view name) { return find_name<0>(name); }

  template <auto _EnumValue>
  static constexpr std::string_view get_name() {
    return std::string_view(std::tuple_element_t<get_index<_EnumValue>(), names>::value.value);
  }

  template <string_literal Name>
  static constexpr decltype(std::tuple_element_t<get_name_index<Name>(), indexes>::value) get_uid() {
    return std::tuple_element_t<get_name_index<Name>(), indexes>::value;
  }

  template <auto _EnumValue, typename T, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  void set(const T& v) {
    std::get<get_index<_EnumValue>()>(attributes) = v;
  }

  template <string_literal Name, typename T>
  void set(const T& v) {
    std::get<get_name_index<Name>()>(attributes) = v;
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  const auto& get() const {
    return std::get<get_index<_EnumValue>()>(attributes);
  }

  template <string_literal Name>
  const auto& get() const {
    return std::get<get_name_index<Name>()>(attributes);
  }

  template <typename... Ts>
  style_descriptor(Ts&&... ts)
      : attributes{ std::forward<Ts>(ts)... } {}

  template <size_t K, typename Fct>
  void iterate_impl(Fct&& fct) const {
    if constexpr (K < meta::count) {
      constexpr auto EnumValue = std::tuple_element_t<K, indexes>::value;
      fct(std::tuple_element_t<K, indexes>{}, get<EnumValue>());
      iterate_impl<K + 1>(std::move(fct));
    }
  }

  values attributes;
};

MTS_END_NAMESPACE
