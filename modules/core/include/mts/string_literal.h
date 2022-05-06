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
#include <algorithm>

MTS_BEGIN_NAMESPACE

/// string_literal.
template <std::size_t N>
class string_literal {
public:
  static constexpr size_t size() { return N; }

  constexpr string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }

  char value[N];

private:
  template <std::size_t Start, std::size_t Count, std::size_t... Indices>
  inline constexpr string_literal<Count + 1> substr_impl(std::index_sequence<Indices...>) const {
    return string_literal<Count + 1>({ value[Start + Indices]..., 0 });
  }

public:
  template <std::size_t Start, std::size_t Count>
  inline constexpr string_literal<Count + 1> substr() const {
    return substr_impl<Start, Count>(std::make_index_sequence<Count>());
  }
};

template <std::size_t N>
string_literal(const char (&str)[N]) -> string_literal<N>;

template <string_literal A, string_literal B>
struct is_s_same : mts::false_t {};

template <string_literal A>
struct is_s_same<A, A> : mts::true_t {};

template <string_literal A, string_literal B>
inline constexpr bool s_compare() {
  return is_s_same<A, B>::value;
}

namespace detail {
template <string_literal A, string_literal B, std::size_t... AIndices, std::size_t... BIndices>
inline constexpr auto concat_impl(std::index_sequence<AIndices...>, std::index_sequence<BIndices...>) {
  return string_literal({ A.value[AIndices]..., B.value[BIndices]... });
}

template <string_literal Separator, string_literal A, string_literal B, std::size_t... SIndices,
    std::size_t... AIndices, std::size_t... BIndices>
inline constexpr auto join_impl(
    std::index_sequence<SIndices...>, std::index_sequence<AIndices...>, std::index_sequence<BIndices...>) {
  return string_literal({ A.value[AIndices]..., Separator.value[SIndices]..., B.value[BIndices]... });
}
} // namespace detail.

template <string_literal A, string_literal B, string_literal... Cs>
inline constexpr auto concat() {
  if constexpr (sizeof...(Cs) == 0) {
    return detail::concat_impl<A, B>(std::make_index_sequence<A.size() - 1>(), std::make_index_sequence<B.size()>());
  }
  else {
    return concat<detail::concat_impl<A, B>(
                      std::make_index_sequence<A.size() - 1>(), std::make_index_sequence<B.size()>()),
        Cs...>();
  }
}

template <string_literal Separator, string_literal A, string_literal B, string_literal... Cs>
inline constexpr auto join() {
  if constexpr (sizeof...(Cs) == 0) {
    return detail::join_impl<Separator, A, B>(std::make_index_sequence<Separator.size() - 1>(),
        std::make_index_sequence<A.size() - 1>(), std::make_index_sequence<B.size()>());
  }
  else {
    return join<Separator,
        detail::join_impl<Separator, A, B>(std::make_index_sequence<Separator.size() - 1>(),
            std::make_index_sequence<A.size() - 1>(), std::make_index_sequence<B.size()>()),
        Cs...>();
  }
}

template <string_literal A>
inline constexpr bool is_valid_split_string() {
  if constexpr (A.size() == 0) {
    return false;
  }

  std::size_t start = 0;
  std::size_t end = std::string_view(A.value).find('.');

  if (end == std::string_view::npos) {
    return true;
  }

  while (end != std::string_view::npos) {
    if (std::string_view(A.value).substr(start, end - start).empty()) {
      return false;
    }

    start = end + 1;
    end = std::string_view(A.value).find('.', start);
  }

  return !std::string_view(A.value).substr(start, end).empty();
}

template <string_literal A>
inline constexpr std::size_t get_split_string_count() {
  std::size_t start = 0;
  std::size_t end = std::string_view(A.value).find('.');

  if (end == std::string_view::npos) {
    return 0;
  }

  std::size_t count = 0;
  while (end != std::string_view::npos) {
    ++count;
    start = end + 1;
    end = std::string_view(A.value).find('.', start);
  }

  return count;
}

template <string_literal A, std::size_t Count>
inline constexpr std::array<std::pair<std::size_t, std::size_t>, Count + 1> get_split_string_index_pair() {
  std::size_t start = 0;
  std::size_t end = std::string_view(A.value).find('.');

  if (end == std::string_view::npos) {
    return {};
  }

  std::array<std::pair<std::size_t, std::size_t>, Count + 1> arr;

  std::size_t count = 0;
  while (end != std::string_view::npos) {
    arr[count++] = { start, end - start };
    start = end + 1;
    end = std::string_view(A.value).find('.', start);
  }

  arr[count++] = { start, std::string_view(A.value).substr(start, end).size() };

  return arr;
}

template <string_literal A, auto Indexes, std::size_t Index>
inline constexpr auto split_at_impl() {
  constexpr std::size_t start = Indexes[Index].first;
  constexpr std::size_t end = Indexes[Index].second;
  return A.template substr<start, end>();
}

template <string_literal A, auto ArrIndexes, std::size_t... Indices>
inline constexpr auto split_impl(std::index_sequence<Indices...>) {
  return std::make_tuple(split_at_impl<A, ArrIndexes, Indices>()...);
}

template <string_literal A>
inline constexpr auto split() {
  constexpr std::size_t count = mts::get_split_string_count<A>();

  if constexpr (count == 0) {
    return std::make_tuple(A);
  }
  else {
    constexpr auto pindexes = get_split_string_index_pair<A, count>();
    return split_impl<A, pindexes>(std::make_index_sequence<count + 1>{});
  }
}

MTS_END_NAMESPACE
