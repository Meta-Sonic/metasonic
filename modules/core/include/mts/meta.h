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
#include "mts/ascii.h"
#include "mts/pointer.h"
#include "mts/traits.h"
#include "mts/string_literal.h"
#include <optional>

MTS_BEGIN_NAMESPACE
struct meta_object_base_tag {};

template <typename T>
using is_meta_object = std::is_base_of<meta_object_base_tag, T>;

template <typename T>
inline constexpr bool is_meta_object_v = is_meta_object<T>::value;

namespace meta::detail {
template <typename T>
struct is_discoverable_base : mts::false_t {};
} // namespace meta::detail.

template <typename T>
struct is_discoverable : meta::detail::is_discoverable_base<mts::remove_cvref_t<T>> {};

template <typename T>
inline constexpr bool is_discoverable_v = is_discoverable<T>::value;

template <typename T>
using is_reflectable = std::disjunction<is_meta_object<T>, is_discoverable<T>>;

template <typename T>
inline constexpr bool is_reflectable_v = is_reflectable<T>::value;

#define __MTS_DECLARE_DISCOVERABLE_VALUE(X, TYPE)                                                                      \
  , mts::meta::discoverable_property<#X, decltype(TYPE::X), offsetof(TYPE, X)>

#define MTS_DECLARE_DISCOVERABLE(TYPE, ...)                                                                            \
  template <>                                                                                                          \
  struct mts::meta::detail::is_discoverable_base<TYPE> : mts::true_t {                                                 \
    template <typename _Tp>                                                                                            \
    using types = mts::meta::discoverable_object<_Tp MTS_FOR_EACH_PARAM(                                               \
        __MTS_DECLARE_DISCOVERABLE_VALUE, MTS_VA_LIST(TYPE), __VA_ARGS__)>;                                            \
  }

namespace meta {
template <typename... Args>
class object;

template <string_literal Key, typename T>
struct property {
  static constexpr string_literal key = Key;
  using type = T;
};

template <string_literal Key, typename T, std::size_t Offset>
struct discoverable_property {
  static constexpr string_literal key = Key;
  using type = T;
  static constexpr std::size_t offset = Offset;
};

template <typename D, bool IsConst>
using discoverable_property_to_property_t = property<D::key,
    std::conditional_t<IsConst, std::add_lvalue_reference_t<std::add_const_t<typename D::type>>,
        std::add_lvalue_reference_t<typename D::type>>>;

template <typename T>
struct discoverable_traits {
  using base_type = mts::remove_cvref_t<T>;
  using type = typename detail::is_discoverable_base<base_type>::template types<T>;
};

template <typename T>
using discoverable_t = typename discoverable_traits<T>::type;

namespace detail {
  template <string_literal Key, typename T>
  struct meta_property_ref {
    static constexpr string_literal key = Key;
    using type = T;
    type& value;
  };

  template <string_literal Key, typename T>
  struct meta_property_const_ref {
    static constexpr string_literal key = Key;
    using type = T;
    const type& value;
  };

  template <string_literal Key>
  struct meta_property_key {
    static constexpr auto key = Key;
  };

  ///
  template <typename T>
  struct extract_key_from_meta_property {
    static constexpr auto value = T::key;
  };

  template <typename T>
  inline constexpr auto extract_key_from_meta_property_v = extract_key_from_meta_property<T>::value;

  ///
  template <typename T>
  struct extract_type_from_meta_property {
    using type = typename T::type;
  };

  template <typename T>
  using extract_type_from_meta_property_t = typename extract_type_from_meta_property<T>::type;

  template <typename T>
  struct extract_offset_from_discoverable_property {
    static constexpr std::size_t value = T::offset;
  };

  template <typename T>
  inline constexpr std::size_t extract_offset_from_discoverable_property_v
      = extract_offset_from_discoverable_property<T>::value;

  template <typename Values, std::size_t... __indices>
  inline constexpr auto make_meta_offset_array(std::index_sequence<__indices...>) {
    return std::array{ mts::tuple_offset_of_v<__indices, Values>... };
  }

  template <typename Values, std::size_t N>
  inline constexpr auto meta_offset_array() {
    return make_meta_offset_array<Values>(std::make_index_sequence<N>());
  }

  template <mts::string_literal Key>
  inline constexpr bool check_property_string() {

    if constexpr (!Key.size()) {
      return false;
    }

    if (!mts::is_letter_or_underscore(Key.value[0])) {
      return false;
    }

    for (char c : std::string_view(Key.value)) {
      if (!mts::is_alphanumeric_or_underscore(c)) {
        return false;
      }
    }

    return true;
  }

  template <std::size_t K, typename Keys>
  inline constexpr bool check_meta_property_strings() {
    if constexpr (K == std::tuple_size_v<Keys>) {
      return true;
    }
    else {
      return check_property_string<std::tuple_element_t<K, Keys>::key>() && check_meta_property_strings<K + 1, Keys>();
    }
  }

  template <typename... Args>
  class meta_object_base : public meta_object_base_tag {
    using descriptor = std::tuple<Args...>;
    static constexpr std::size_t count = std::tuple_size<descriptor>::value;

    template <class... Ts>
    friend class meta_object_base;

    template <class... Ts>
    friend class object;

  public:
    using types = std::tuple<mts::remove_cvref_t<extract_type_from_meta_property_t<Args>>...>;
    using keys = std::tuple<meta_property_key<extract_key_from_meta_property_v<Args>>...>;

  private:
    static constexpr bool has_reference
        = std::disjunction<std::is_reference<extract_type_from_meta_property_t<Args>...>>::value;

  protected:
    using value_types = std::tuple<extract_type_from_meta_property_t<Args>...>;

    static constexpr std::array<std::size_t, count> offsets = meta_offset_array<value_types, count>();

    static_assert(check_meta_property_strings<0, keys>(),
        "meta_object property strings must be a valid c++ identifier (i.e. alphanumeric with underscore)");

  public:
    static constexpr std::size_t size() { return count; }

    template <string_literal Key>
    inline static constexpr std::size_t index_of() {
      return (std::size_t)tuple_index_v<meta_property_key<Key>, keys>;
    }

    template <string_literal Key>
    using type_for_key = std::tuple_element_t<index_of<Key>(), types>;

  protected:
    template <string_literal Key, string_literal... Keys>
    inline static constexpr bool contains_impl() noexcept {
      if constexpr (sizeof...(Keys) == 0) {
        return mts::tuple_index_inf_v<meta_property_key<Key>, keys> < count;
      }
      else {
        if constexpr (contains<Key>() && mts::is_reflectable_v<type_for_key<Key>>) {
          if constexpr (mts::is_discoverable_v<type_for_key<Key>>) {
            return discoverable_t<type_for_key<Key>>::template contains<Keys...>();
          }
          else {
            return type_for_key<Key>::template contains<Keys...>();
          }
        }
        else {
          return false;
        }
      }
    }

    template <size_t K, typename Fct>
    inline constexpr void iterate_impl(Fct&& fct) {
      if constexpr (K < count) {
        fct(meta_property_ref<std::tuple_element_t<K, keys>::key, std::tuple_element_t<K, types>>{
            std::get<K>(_values) });

        iterate_impl<K + 1>(std::move(fct));
      }
    }

    template <size_t K, typename Fct>
    inline constexpr void iterate_impl(Fct&& fct) const {
      if constexpr (K < count) {
        fct(meta_property_const_ref<std::tuple_element_t<K, keys>::key, std::tuple_element_t<K, types>>{
            std::get<K>(_values) });
        iterate_impl<K + 1>(std::move(fct));
      }
    }

    template <typename T, size_t K>
    inline static constexpr std::size_t validate_key_index(std::string_view key) noexcept {
      if constexpr (K == count) {
        return count;
      }
      else if (std::tuple_element_t<K, keys>::key.value == key) {
        return std::is_same_v<T, std::tuple_element_t<K, types>> ? K : count;
      }
      else {
        return validate_key_index<T, K + 1>(key);
      }
    }

    template <typename T, size_t K>
    inline static constexpr std::size_t validate_key_index_convertible(std::string_view key) noexcept {
      if constexpr (K == count) {
        return count;
      }
      else if (std::tuple_element_t<K, keys>::key.value == key) {
        return std::is_convertible_v<std::tuple_element_t<K, types>, T> ? K : count;
      }
      else {
        return validate_key_index_convertible<T, K + 1>(key);
      }
    }

    template <std::size_t K = 0, typename T>
    inline constexpr std::optional<T> convert_to_impl(std::string_view key) const {
      if constexpr (K == count) {
        return std::optional<T>();
      }
      else if (std::tuple_element_t<K, keys>::key.value == key) {
        if constexpr (std::is_convertible_v<std::tuple_element_t<K, types>, T>) {
          return std::optional<T>((T)std::get<K>(_values));
        }
        else {
          return std::optional<T>();
        }
      }
      else {
        return convert_to_impl<K + 1, T>(key);
      }
    }

  public:
    meta_object_base() = default;

    template <typename... Params>
    meta_object_base(Params&&... ps)
        : _values{ std::forward<Params>(ps)... } {}

    template <string_literal Key>
    inline static constexpr bool contains() noexcept {

      if constexpr (mts::get_split_string_count<Key>() > 0) {
        constexpr auto ts = mts::split<Key>();
        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
          return contains_impl<std::get<Indices>(ts)...>();
        }
        (std::make_index_sequence<std::tuple_size_v<decltype(ts)>>{});
      }
      else {
        return contains_impl<Key>();
      }
    }

    template <string_literal Key>
    inline constexpr const auto& get() const noexcept {
      return std::get<index_of<Key>()>(_values);
    }

    template <string_literal Key>
    inline constexpr auto& get() noexcept {
      return std::get<index_of<Key>()>(_values);
    }

    template <typename T>
    inline constexpr std::optional<T> get_as(std::string_view key) const {
      return convert_to_impl<0, T>(key);
    }

    template <typename T>
    inline static constexpr bool compare_type(std::string_view key) noexcept {
      return validate_key_index<T, 0>(key) != count;
    }

    template <typename T>
    inline static constexpr bool compare_convertible_type(std::string_view key) noexcept {
      return validate_key_index_convertible<T, 0>(key) != count;
    }

    template <typename Fct>
    inline constexpr void iterate(Fct&& fct) {
      iterate_impl<0>(std::move(fct));
    }

    template <typename Fct>
    inline constexpr void iterate(Fct&& fct) const {
      iterate_impl<0>(std::move(fct));
    }

  protected:
    value_types _values;
  };

  template <typename ObjectType, typename... Args>
  class discoverable_object_base
      : public detail::meta_object_base<discoverable_property_to_property_t<Args, std::is_const_v<ObjectType>>...> {
  public:
    using base = detail::meta_object_base<discoverable_property_to_property_t<Args, std::is_const_v<ObjectType>>...>;
    using object_type = ObjectType;

    using types = typename base::types;
    using base::contains;
    using base::get;
    using base::iterate;

    static constexpr std::array<std::size_t, base::size()> offsets
        = { extract_offset_from_discoverable_property_v<Args>... };

  private:
    object_type& _obj;

    template <std::size_t Index>
    inline static std::tuple_element_t<Index, types>& obj_to_t_ref(object_type& obj) {
      return *mts::pointer_offset_cast<std::tuple_element_t<Index, types>>(&obj, offsets[Index]);
    }

    template <std::size_t... __indices>
    inline discoverable_object_base(object_type& obj, std::index_sequence<__indices...>)
        : base{ obj_to_t_ref<__indices>(obj)... }
        , _obj(obj) {}

  public:
    inline discoverable_object_base(object_type& obj)
        : discoverable_object_base(obj, std::make_index_sequence<base::size()>()) {}

    template <typename T>
    inline const T* get(std::string_view key) const noexcept {
      std::size_t index = base::template validate_key_index<T, 0>(key);
      return index == base::size() ? nullptr : mts::pointer_offset_cast<const T>(&_obj, offsets[index]);
    }

    template <typename T>
    inline T* get(std::string_view key) noexcept {
      std::size_t index = base::template validate_key_index<T, 0>(key);
      return index == base::size() ? nullptr : mts::pointer_offset_cast<T>(&_obj, offsets[index]);
    }
  };
} // namespace detail.

template <typename... Args>
class object : public detail::meta_object_base<Args...> {
public:
  using base = detail::meta_object_base<Args...>;
  using base::base;
  using base::get;

  template <typename T>
  inline const T* get(std::string_view key) const noexcept {
    std::size_t index = validate_key_index<T, 0>(key);

    return index == base::size() ? nullptr : mts::pointer_offset_cast<const T>(&this->_values, base::offsets[index]);
  }

  template <typename T>
  inline T* get(std::string_view key) noexcept {
    std::size_t index = base::template validate_key_index<T, 0>(key);
    return index == base::size() ? nullptr : mts::pointer_offset_cast<T>(&this->_values, base::offsets[index]);
  }
};

template <typename ObjectType, typename... Args>
class discoverable_object : public detail::discoverable_object_base<ObjectType, Args...> {
  using base = detail::discoverable_object_base<ObjectType, Args...>;
  using base::base;
};

template <typename T, string_literal Key>
inline constexpr bool has() {
  if constexpr (mts::is_meta_object_v<T>) {
    return T::template contains<Key>();
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    return discoverable_t<T>::template contains<Key>();
  }
  else {
    return false;
  }
}

template <typename T, string_literal Key, string_literal... Keys>
inline constexpr bool has_all() {
  if constexpr (sizeof...(Keys) == 0) {
    return has<T, Key>();
  }
  else {
    return has<T, Key>() && has_all<T, Keys...>();
  }
}

template <string_literal Key, typename T>
inline auto& get(T& obj) {
  if constexpr (mts::is_meta_object_v<T>) {
    return obj.template get<Key>();
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    return discoverable_t<T>(obj).template get<Key>();
  }
}

template <string_literal Key, typename T>
inline const auto& get(const T& obj) {
  if constexpr (mts::is_meta_object_v<T>) {
    return obj.template get<Key>();
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    const discoverable_t<const T> dobj(obj);
    return dobj.template get<Key>();
  }
}

template <typename ValueType, typename T>
inline ValueType* get_ptr(T& obj, std::string_view key) {
  if constexpr (mts::is_meta_object_v<T>) {
    return obj.template get<ValueType>(key);
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    return discoverable_t<T>(obj).template get<ValueType>(key);
  }
}

template <typename ValueType, typename T>
inline const ValueType* get_ptr(const T& obj, std::string_view key) {
  if constexpr (mts::is_meta_object_v<T>) {
    return obj.template get<ValueType>(key);
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    const discoverable_t<const T> dobj(obj);
    return dobj.template get<ValueType>(key);
  }
}

template <typename ValueType, typename T>
inline std::optional<ValueType> convert_to(const T& obj, std::string_view key) {
  if constexpr (mts::is_meta_object_v<T>) {
    return obj.template get_as<ValueType>(key);
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    const discoverable_t<const T> dobj(obj);
    return dobj.template get_as<ValueType>(key);
  }
}

template <typename T>
class get_as {
public:
  get_as(T& obj, std::string_view key)
      : _obj(obj)
      , _key(key) {}

  template <typename ValueType>
  operator std::optional<ValueType>() {

    return convert_to<ValueType>(_obj, _key);
  }

  template <typename ValueType>
  operator std::optional<ValueType>() const {

    return convert_to<ValueType>(_obj, _key);
  }

  template <typename ValueType>
  operator ValueType*() {
    return get_ptr<mts::remove_cvref_t<ValueType>>(_obj, _key);
  }

  template <typename ValueType>
  operator const ValueType*() const {

    return get_ptr<mts::remove_cvref_t<ValueType>>(_obj, _key);
  }

private:
  T& _obj;
  std::string_view _key;
};

template <typename Fct, typename T>
inline void iterate(T& obj, Fct&& fct) {
  if constexpr (mts::is_meta_object_v<T>) {
    obj.template iterate(std::forward<Fct>(fct));
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    discoverable_t<T>(obj).template iterate(std::forward<Fct>(fct));
  }
}

template <typename Fct, typename T>
inline void iterate(const T& obj, Fct&& fct) {
  if constexpr (mts::is_meta_object_v<T>) {
    obj.template iterate(std::forward<Fct>(fct));
  }
  else if constexpr (mts::is_discoverable_v<T>) {
    const discoverable_t<const T> dobj(obj);
    dobj.template iterate(std::forward<Fct>(fct));
  }
}

template <mts::string_literal... Keys, typename... Args>
inline auto make_object(Args&&... args) {
  static_assert(sizeof...(Keys) == sizeof...(Args), "");
  return object<mts::meta::property<Keys, Args>...>(std::forward<Args>(args)...);
}
} // namespace meta.

MTS_END_NAMESPACE
