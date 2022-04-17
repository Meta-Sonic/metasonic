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
#include <algorithm>
#include <array>
#include <iostream>
#include <cstddef>
#include <type_traits>
#include <functional>
#include <iterator>
#include <complex>
#include <memory>
#include <string_view>
#include <utility>
#include <tuple>

MTS_PUSH_MACROS
#include "mts/detail/undef_macros.h"

MTS_BEGIN_NAMESPACE

/// type_identity.
template <class T>
struct type_identity {
  using type = T;
};

/// nonesuch.
struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

/// string_litteral.
template <std::size_t N>
struct string_litteral {
  static constexpr size_t size() { return N; }

  constexpr string_litteral(const char (&str)[N]) { std::copy_n(str, N, value); }

  char value[N];
};

template <typename T>
using array_of_type = T (*)[];

template <bool...>
struct bool_pack;

template <bool... bs>
using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

template <typename T>
inline constexpr bool always_false_v = false;

template <bool Condition>
using enable_if_ptr = std::enable_if<Condition, std::nullptr_t>;

template <bool Condition>
using enable_if_ptr_t = typename enable_if_ptr<Condition>::type;

/// enable_if_cond.
template <bool Condition, typename T = void>
struct enable_if_cond {
  typedef T type;
};

template <typename T>
struct enable_if_cond<false, T> {};

/// disable_if_cond.
template <bool Condition, typename T = void>
struct disable_if_cond {
  typedef T type;
};

template <typename T>
struct disable_if_cond<true, T> {};

/// enabled_if_or_void.
template <typename T, typename R = void>
struct enabled_if_or_void {
  using type = R;
};

template <typename T, typename R = void>
using enabled_if_or_void_t = typename enabled_if_or_void<T, R>::type;

/// enable_if_same.
template <class A, class B>
using enable_if_same = std::enable_if<std::is_same_v<A, B>>;

template <class A, class B>
using enable_if_same_t = typename enable_if_same<A, B>::type;

/// enable_if_different.
template <class A, class B>
using enable_if_different = std::enable_if<!std::is_same_v<A, B>>;

template <class A, class B>
using enable_if_different_t = typename enable_if_different<A, B>::type;

template <bool Dummy, class D>
using enable_if_dependant_same = std::enable_if<std::is_same_v<D, std::true_type>>;

template <bool Dummy, class D>
using enable_if_dependant_same_t = typename enable_if_dependant_same<Dummy, D>::type;

template <bool Dummy, class D>
using enable_if_dependant_different = std::enable_if<std::is_same_v<D, std::false_type>>;

template <bool Dummy, class D>
using enable_if_dependant_different_t = typename enable_if_dependant_different<Dummy, D>::type;

/// dependent_type.
template <class T, bool>
struct dependent_type : public T {};

template <bool Dummy, class D>
using dependent_type_condition = typename dependent_type<type_identity<D>, Dummy>::type;

/// remove_cvref.
template <typename T>
using remove_cvref = std::remove_cv<std::remove_reference_t<T>>;

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

/// is_detected.
namespace detail {
template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};
} // namespace detail

template <template <class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;

template <template <class...> class Op, class... Args>
inline constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <class Default, template <class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template <class Expected, template <class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class Expected, template <class...> class Op, class... Args>
inline constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

template <class To, template <class...> class Op, class... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

template <class To, template <class...> class Op, class... Args>
inline constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

template <typename T, typename Tuple>
struct has_type;

template <typename T, typename... Us>
struct has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};

template <typename T, typename Tuple>
inline constexpr bool has_type_v = has_type<T, Tuple>::value;

namespace common_members {
/// first_t.
template <typename T>
using first_t = decltype(T::first);

/// second_t.
template <typename T>
using second_t = decltype(T::second);

/// min_t.
template <typename T>
using min_t = decltype(T::min);

/// max_t.
template <typename T>
using max_t = decltype(T::max);

/// x_t.
template <typename T>
using x_t = decltype(T::x);

/// y_t.
template <typename T>
using y_t = decltype(T::y);

/// width_t.
template <typename T>
using width_t = decltype(T::width);

/// height_t.
template <typename T>
using height_t = decltype(T::height);
} // namespace common_members.

/// is_different.
template <typename A, typename B>
using is_different = std::bool_constant<!std::is_same_v<A, B>>;

template <typename A, typename B>
inline constexpr bool is_different_v = is_different<A, B>::value;

/// is_defined.
template <class, class = void>
struct is_defined : std::false_type {};

template <class T>
struct is_defined<T, std::enable_if_t<std::is_object<T>::value && !std::is_pointer<T>::value && (sizeof(T) > 0)>>
    : std::true_type {};

template <class T>
inline constexpr bool is_defined_v = is_defined<T>::value;

namespace detail {
template <class _Tp>
struct is_std_array_impl : public std::false_type {};

template <class _Tp, std::size_t _Sz>
struct is_std_array_impl<std::array<_Tp, _Sz>> : public std::true_type {};

} // namespace detail.

/// is_array_of_type_convertible.
template <typename A, typename B>
using is_array_of_type_convertible = std::is_convertible<array_of_type<A>, array_of_type<B>>;

template <typename A, typename B>
inline constexpr bool is_array_of_type_convertible_v = is_array_of_type_convertible<A, B>::value;

/// is_std_array.
template <class _Tp>
struct is_std_array : public detail::is_std_array_impl<std::remove_cv_t<_Tp>> {};

template <typename _Tp>
inline constexpr bool is_std_array_v = is_std_array<_Tp>::value;

/// is_unique_ptr.
template <typename>
struct is_unique_ptr : std::false_type {};

template <typename... T>
struct is_unique_ptr<std::unique_ptr<T...>> : std::true_type {};

template <typename... T>
inline constexpr bool is_unique_ptr_v = is_unique_ptr<T...>::value;

/// is_tuple.
template <typename>
struct is_tuple : std::false_type {};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

template <typename... T>
inline constexpr bool is_tuple_v = is_tuple<T...>::value;

/// is_complex.
template <typename>
struct is_complex : std::false_type {};

template <typename... T>
struct is_complex<std::complex<T...>> : std::true_type {};

template <typename... T>
inline constexpr bool is_complex_v = is_complex<T...>::value;

/// is_pair.
template <typename>
struct is_pair : std::false_type {};

template <typename... T>
struct is_pair<std::pair<T...>> : std::true_type {};

template <typename... T>
inline constexpr bool is_pair_v = is_pair<T...>::value;

/// has_first_second.
template <typename T>
using has_first_second
    = std::conjunction<is_detected<common_members::first_t, T>, is_detected<common_members::second_t, T>>;

template <typename T>
inline constexpr bool has_first_second_v = has_first_second<T>::value;

/// has_min_max.
template <typename T>
using has_min_max = std::conjunction<is_detected<common_members::min_t, T>, is_detected<common_members::max_t, T>>;

template <typename T>
inline constexpr bool has_min_max_v = has_min_max<T>::value;

/// is_range.
template <typename T>
using is_range = has_min_max<T>;

template <typename T>
inline constexpr bool is_range_v = is_range<T>::value;

/// has_x_y.
template <typename T>
using has_x_y = std::conjunction<is_detected<common_members::x_t, T>, is_detected<common_members::y_t, T>>;

template <typename T>
inline constexpr bool has_x_y_v = has_x_y<T>::value;

/// is_point.
template <typename T>
using is_point = has_x_y<T>;

template <typename T>
inline constexpr bool is_point_v = is_point<T>::value;

/// has_width_height.
template <typename T>
using has_width_height
    = std::conjunction<is_detected<common_members::width_t, T>, is_detected<common_members::height_t, T>>;

template <typename T>
inline constexpr bool has_width_height_v = has_width_height<T>::value;

/// is_size.
template <typename T>
using is_size = has_width_height<T>;

template <typename T>
inline constexpr bool is_size_v = is_size<T>::value;

/// has_x_y_width_height.
template <typename T>
using has_x_y_width_height = std::conjunction<is_detected<common_members::x_t, T>, is_detected<common_members::y_t, T>,
    is_detected<common_members::width_t, T>, is_detected<common_members::height_t, T>>;

template <typename T>
inline constexpr bool has_x_y_width_height_v = has_x_y_width_height<T>::value;

/// is_rect.
template <typename T>
using is_rect = has_x_y_width_height<T>;

template <typename T>
inline constexpr bool is_rect_v = is_rect<T>::value;

/// is_integer_convertible.
template <typename T>
using is_integer_convertible = std::bool_constant<std::is_unsigned_v<T> || std::is_signed_v<T>>;

template <typename T>
inline constexpr bool is_integer_convertible_v = is_integer_convertible<T>::value;

/// is_integer_convertible_not_bool.
template <typename T>
using is_integer_convertible_not_bool = std::bool_constant<is_integer_convertible_v<T> && !std::is_same_v<T, bool>>;

template <typename T>
inline constexpr bool is_integer_convertible_not_bool_v = is_integer_convertible_not_bool<T>::value;

/// is_base_of_template.
template <template <typename...> class Base, typename Derived>
struct is_base_of_template_impl {
  template <typename... Ts>
  static constexpr std::true_type test(const Base<Ts...>*);
  static constexpr std::false_type test(...);
  using type = decltype(test(std::declval<Derived*>()));
};

template <template <typename...> class Base, typename Derived>
using is_base_of_template = typename is_base_of_template_impl<Base, Derived>::type;

template <template <typename...> class Base, typename Derived>
inline constexpr bool is_base_of_template_v = is_base_of_template<Base, Derived>::value;

/// is_memcopyable
/// Check if a container can use memcpy when copying a buffer of T.
template <typename T>
using is_memcopyable
    = std::bool_constant<std::is_trivially_copy_constructible_v<T> && std::is_trivially_destructible_v<T>>;

template <typename T>
inline constexpr bool is_memcopyable_v = is_memcopyable<T>::value;

/// is_random_access_iterator.
template <typename Iterator>
using is_random_access_iterator
    = std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>;

template <typename T>
inline constexpr bool is_random_access_iterator_v = is_random_access_iterator<T>::value;

/// is_basic_string_view_convertible.
/// Only works if T is const convertible to basic_string_view.
template <class T, class CharT, class Traits = std::char_traits<CharT>>
struct is_basic_string_view_const_convertible_not_cchar
    : public std::bool_constant<
          std::is_convertible_v<const T&,
              std::basic_string_view<CharT, Traits>> && !std::is_convertible_v<const T&, const CharT*>> {};

template <class T, class CharT, class Traits = std::char_traits<CharT>>
inline constexpr bool is_basic_string_view_const_convertible_not_cchar_v
    = is_basic_string_view_const_convertible_not_cchar<T, CharT, Traits>::value;

/// is_string_view_convertible.
/// Only works if T is const convertible to string_view.
template <class T>
using is_string_view_const_convertible_not_cchar = is_basic_string_view_const_convertible_not_cchar<T, char>;

template <class T>
inline constexpr bool is_string_view_const_convertible_not_cchar_v
    = is_string_view_const_convertible_not_cchar<T>::value;

// has_ostream.
namespace detail {
template <typename = void, typename... Args>
struct has_ostream_impl : std::false_type {};

template <typename... Args>
struct has_ostream_impl<std::void_t<decltype(std::declval<std::ostream>().operator<<(std::declval<Args>()...))>,
    Args...> : std::true_type {};

template <typename = void, typename... Args>
struct has_global_ostream_impl : std::false_type {};

template <typename... Args>
struct has_global_ostream_impl<decltype(operator<<(std::declval<std::ostream&>(), std::declval<const Args&>()...)),
    Args...> : std::true_type {};
} // namespace detail.

/// has_ostream.
template <typename... Args>
using has_ostream = std::disjunction<detail::has_ostream_impl<void, Args...>,
    detail::has_global_ostream_impl<std::ostream&, Args...>>;

template <typename... Args>
inline constexpr bool has_ostream_v = has_ostream<Args...>::value;

// is_iterable.
namespace detail {
template <typename T>
auto is_iterable_impl(int)
    -> decltype(std::begin(std::declval<T&>()) != std::end(std::declval<T&>()), // begin/end and operator !=
        void(), // Handle evil operator ,
        ++std::declval<decltype(std::begin(std::declval<T&>()))&>(), // ++operator
        void(*std::begin(std::declval<T&>())), // operator*
        std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

template <typename T>
auto is_const_iterable_impl(int)
    -> decltype(std::declval<const T&>().cbegin() != std::declval<const T&>().cend(), // begin/end and operator !=
        void(), // Handle evil operator ,
        ++std::declval<decltype(std::declval<const T&>().cbegin())&>(), // ++operator
        void(*std::declval<const T&>().cbegin()), // operator*
        std::true_type{});

template <typename T>
std::false_type is_const_iterable_impl(...);
} // namespace detail

/// is_iterable.
/// Has both begin() and end() and the resulting iterator has
/// operator!=, ++operator and is dereferenceable (*operator).
template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(0));

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

/// is_const_iterable.
/// Has both 'cbegin() const' and 'cend() const' and the resulting iterator has
/// operator!=, ++operator and is dereferenceable (*operator).
template <typename T>
using is_const_iterable = decltype(detail::is_const_iterable_impl<T>(0));

template <typename T>
inline constexpr bool is_const_iterable_v = is_const_iterable<T>::value;

template <class Container>
using container_data_type = std::remove_pointer<decltype(std::data(std::declval<Container&>()))>;

template <class Container>
using container_data_type_t = typename container_data_type<Container>::type;

namespace ops {
template <class A, class B>
using eq_t = decltype(std::declval<const A&>() == std::declval<const B&>());

template <class A, class B>
using neq_t = decltype(std::declval<const A&>() != std::declval<const B&>());

template <class A, class B>
using less_t = decltype(std::declval<const A&>() < std::declval<const B&>());

template <class A, class B>
using less_eq_t = decltype(std::declval<const A&>() <= std::declval<const B&>());

template <class A, class B>
using greater_t = decltype(std::declval<const A&>() > std::declval<const B&>());

template <class A, class B>
using greater_eq_t = decltype(std::declval<const A&>() >= std::declval<const B&>());

template <class A, class B>
using add_t = decltype(std::declval<const A&>() + std::declval<const B&>());

template <class A, class B>
using sub_t = decltype(std::declval<const A&>() - std::declval<const B&>());

template <class A, class B>
using mul_t = decltype(std::declval<const A&>() * std::declval<const B&>());

template <class A, class B>
using div_t = decltype(std::declval<const A&>() / std::declval<const B&>());
} // namespace ops

/// has_compare_less.
template <class A, class B>
using has_compare_less = is_detected<ops::less_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_less_v = has_compare_less<A, B>::value;

/// has_compare_less_eq.
template <class A, class B>
using has_compare_less_eq = is_detected<ops::less_eq_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_less_eq_v = has_compare_less_eq<A, B>::value;

/// has_compare_greater.
template <class A, class B>
using has_compare_greater = is_detected<ops::greater_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_greater_v = has_compare_greater<A, B>::value;

/// has_compare_greater_eq.
template <class A, class B>
using has_compare_greater_eq = is_detected<ops::greater_eq_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_greater_eq_v = has_compare_greater_eq<A, B>::value;

/// has_compare_eq.
template <class A, class B>
using has_compare_eq = is_detected<ops::eq_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_eq_v = has_compare_eq<A, B>::value;

/// has_compare_neq.
template <class A, class B>
using has_compare_neq = is_detected<ops::neq_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_compare_neq_v = has_compare_neq<A, B>::value;

/// has_arithmetic_add.
template <class A, class B>
using has_arithmetic_add = is_detected<ops::add_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_arithmetic_add_v = has_arithmetic_add<A, B>::value;

/// has_arithmetic_sub.
template <class A, class B>
using has_arithmetic_sub = is_detected<ops::sub_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_arithmetic_sub_v = has_arithmetic_sub<A, B>::value;

/// has_arithmetic_mul.
template <class A, class B>
using has_arithmetic_mul = is_detected<ops::mul_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_arithmetic_mul_v = has_arithmetic_mul<A, B>::value;

/// has_arithmetic_div.
template <class A, class B>
using has_arithmetic_div = is_detected<ops::div_t, A, B>;

template <typename A, typename B>
inline constexpr bool has_arithmetic_div_v = has_arithmetic_div<A, B>::value;

template <typename T>
using floating_point_return = std::conditional<std::is_integral_v<T>, double, T>;

template <typename T>
using floating_point_return_t = typename floating_point_return<T>::type;

template <typename... T>
using floating_point_common_return = floating_point_return<std::common_type_t<T...>>;

template <typename... T>
using floating_point_common_return_t = typename floating_point_common_return<T...>::type;

/// to_integral.
template <typename T>
inline constexpr std::underlying_type_t<T> to_integral(T value) {
  return static_cast<std::underlying_type_t<T>>(value);
}

namespace detail {
template <std::size_t __begin, std::size_t... __indices, typename Tuple>
auto tuple_slice_impl(Tuple&& tuple, std::index_sequence<__indices...>) {
  return std::make_tuple(std::get<__begin + __indices>(std::forward<Tuple>(tuple))...);
}

} // namespace detail.

/// tuple_slice.
template <std::size_t __begin, std::size_t __count, typename Tuple>
auto tuple_slice(Tuple&& tuple) {
  static_assert(__count > 0, "splicing tuple to 0-length is weird...");
  return detail::tuple_slice_impl<__begin>(std::forward<Tuple>(tuple), std::make_index_sequence<__count>());
}

template <std::size_t __begin, std::size_t __count, typename Tuple>
using tuple_slice_t = decltype(tuple_slice<__begin, __count>(std::declval<Tuple>()));

/// tuple_cat_t.
template <typename... _InputT>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<_InputT>()...));

/// tuple_index.
template <class T, class Tuple>
struct tuple_index;

template <class T, class... Types>
struct tuple_index<T, std::tuple<T, Types...>> {
  static constexpr std::size_t value = 0;
};

template <class T, class U, class... Types>
struct tuple_index<T, std::tuple<U, Types...>> {
  static constexpr std::size_t value = 1 + tuple_index<T, std::tuple<Types...>>::value;
};

template <class... Types>
inline constexpr size_t tuple_index_v = tuple_index<Types...>::value;

//
//
//
// Function traits.
//
//

template <int>
struct variadic_placeholder {};
MTS_END_NAMESPACE

namespace std {
template <int N>
struct is_placeholder<mts::variadic_placeholder<N>> : std::integral_constant<int, N + 1> {};
} // namespace std.

MTS_BEGIN_NAMESPACE
// template <class T>
// using call_operator_t = decltype(&T::operator());

template <class T>
using call_operator_t = decltype(std::declval<T&>()());
// using add_t = decltype(std::declval<T&>() + std::declval<const B&>());

/// Only works with function pointers (not member function or lamdas).
template <typename T>
struct is_function_pointer : std::bool_constant<std::is_function_v<std::remove_pointer_t<std::remove_reference_t<T>>>> {
};

/// Works with function pointers, member function pointers, lambdas and function objects.
/// The 'this' argument is excluded from the arguments list for member function pointers.
template <class>
struct callable_args;

template <class R, class... Args>
struct callable_args<R(Args...)> {
  using type = std::tuple<Args...>;

  using return_type = R;
};

template <class R, class... Args>
struct callable_args<R (*)(Args...)> {
  using type = std::tuple<Args...>;
  using return_type = R;
};

template <class R, class T, class... Args>
struct callable_args<R (T::*)(Args...)> {
  using type = std::tuple<Args...>;
  using return_type = R;
};

template <class R, class T, class... Args>
struct callable_args<R (T::*)(Args...) const> {
  using type = std::tuple<Args...>;
  using return_type = R;
};

template <class Fct>
struct callable_args {
  //    static_assert(is_detected_v<call_operator_t, Fct>, "");
  using type = typename callable_args<decltype(&Fct::operator())>::type;

  //    using type = typename callable_args<&decltype(std::declval<Fct>()())>::type;
  //    using call_operator_t = decltype(std::declval<T&>()());
  //    using type = typename callable_args<decltype(&Fct::operator())>::type;
  //
};

template <class Fct>
using callable_args_t = typename callable_args<Fct>::type;

/// Works with function pointers, member function pointers, lambdas and function objects.
template <class>
struct callable_return_type;

template <class R, class... Args>
struct callable_return_type<R(Args...)> {
  using type = R;
};

template <class R, class... Args>
struct callable_return_type<R (*)(Args...)> {
  using type = R;
};

template <class R, class T, class... Args>
struct callable_return_type<R (T::*)(Args...)> {
  using type = R;
};

template <class R, class T, class... Args>
struct callable_return_type<R (T::*)(Args...) const> {
  using type = R;
};

template <class Fct>
struct callable_return_type {
  using type = typename callable_return_type<decltype(&Fct::operator())>::type;
};

template <class Fct>
using callable_return_type_t = typename callable_return_type<Fct>::type;

/// Callable parameters count.
template <class Fct>
struct callable_args_count {
  static constexpr std::size_t value = std::tuple_size_v<callable_args_t<Fct>>;
};

template <class Fct>
inline constexpr std::size_t callable_args_count_v = callable_args_count<Fct>::value;
MTS_END_NAMESPACE
MTS_POP_MACROS
