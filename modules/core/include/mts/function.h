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
#include "mts/traits.h"
#include "mts/print.h"
#include <functional>

MTS_BEGIN_NAMESPACE

struct before_placeholders {};
struct after_placeholders {};

namespace detail {
template <typename T>
inline auto to_obj_pointer(T&& t) {
  if constexpr (std::is_pointer_v<T>) {
    return std::forward<T>(t);
  }
  else {
    return &t;
  }
}

template <typename Order = after_placeholders, typename Fct, typename... Args, std::size_t... Is>
inline auto bind_vars_impl(std::index_sequence<Is...>, Fct&& fct, Args&&... args) {
  if constexpr (std::is_same_v<Order, after_placeholders>) {
    return std::bind(std::forward<Fct>(fct), variadic_placeholder<Is>{}..., std::forward<Args>(args)...);
  }
  else {
    return std::bind(std::forward<Fct>(fct), std::forward<Args>(args)..., variadic_placeholder<Is>{}...);
  }
}

template <typename Order = after_placeholders, typename Fct, typename T, typename... Args, std::size_t... Is>
inline auto bind_member_function_impl(std::index_sequence<Is...>, Fct&& fct, T&& t, Args&&... args) {
  if constexpr (std::is_same_v<Order, after_placeholders>) {
    return std::bind(std::forward<Fct>(fct), to_obj_pointer(std::forward<T>(t)), variadic_placeholder<Is>{}...,
        std::forward<Args>(args)...);
  }
  else {
    return std::bind(std::forward<Fct>(fct), to_obj_pointer(std::forward<T>(t)), std::forward<Args>(args)...,
        variadic_placeholder<Is>{}...);
  }
}
} // namespace detail.

template <typename Order = after_placeholders, typename Fct, typename... Args>
inline auto bind(Fct&& fct, Args&&... args) {
  static_assert(std::is_same_v<Order, after_placeholders> || std::is_same_v<Order, before_placeholders>, "");
  constexpr std::size_t args_size = sizeof...(Args);

  if constexpr (args_size == 0) {
    return std::forward<Fct>(fct);
  }
  else if constexpr (std::is_member_function_pointer_v<Fct>) {
    constexpr std::size_t param_size = callable_args_count_v<Fct>;
    constexpr std::size_t args_size_without_obj_ptr = args_size - 1;
    constexpr std::size_t placeholder_size = param_size - args_size_without_obj_ptr;
    return detail::bind_member_function_impl<Order>(
        std::make_index_sequence<placeholder_size>{}, std::forward<Fct>(fct), std::forward<Args>(args)...);
  }
  else {
    constexpr std::size_t param_size = callable_args_count_v<Fct>;
    constexpr std::size_t placeholder_size = param_size - args_size;
    return detail::bind_vars_impl<Order>(
        std::make_index_sequence<placeholder_size>{}, std::forward<Fct>(fct), std::forward<Args>(args)...);
  }
}

template <typename Fct, typename... Args>
inline auto pre_bind(Fct&& fct, Args&&... args) {
  return bind<before_placeholders>(std::forward<Fct>(fct), std::forward<Args>(args)...);
}

template <typename Fct, typename... Args>
inline auto post_bind(Fct&& fct, Args&&... args) {
  return bind<after_placeholders>(std::forward<Fct>(fct), std::forward<Args>(args)...);
}

MTS_END_NAMESPACE
