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
#include "mts/traits.h"
#include "mts/range.h"
#include "mts/time.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string_view>

MTS_BEGIN_NAMESPACE
template <typename T>
inline void print_element(std::ostream& stream, const T& t);

template <std::size_t I = 0, typename... Tp>
inline void print_tuple(std::ostream& stream, const std::tuple<Tp...>& t) {
  if constexpr (I < sizeof...(Tp) - 1) {
    print_element(stream, std::get<I>(t));
    stream << ", ";

    print_tuple<I + 1, Tp...>(stream, t);
  }
  else {
    print_element(stream, std::get<I>(t));
  }
}

template <typename T>
inline void print_element(std::ostream& stream, const T& t) {
  if constexpr (std::is_same_v<bool, T>) {
    std::ios_base::fmtflags flags = stream.flags(std::ios_base::boolalpha);
    stream << t;
    stream.setf(flags, std::ios_base::boolalpha);
  }
  else if constexpr (_VMTS::has_ostream_v<T>) {
    stream << t;
  }
  else if constexpr (_VMTS::is_iterable_v<T>) {
    stream << "{";
    std::size_t size = t.size();
    std::size_t count = 0;
    for (auto it = t.begin(); it != t.end(); ++it, ++count) {
      print_element(stream, *it);
      stream << (count == size - 1 ? "" : ", ");
    }
    stream << "}";
  }
  else if constexpr (_VMTS::is_const_iterable_v<T>) {
    stream << "{";
    std::size_t size = t.size();
    std::size_t count = 0;
    for (auto it = t.cbegin(); it != t.cend(); ++it, ++count) {
      print_element(stream, *it);
      stream << (count == size - 1 ? "" : ", ");
    }
    stream << "}";
  }
  else if constexpr (_VMTS::is_pair_v<T>) {
    stream << "{";
    print_element(stream, t.first);
    stream << ", ";
    print_element(stream, t.second);
    stream << "}";
  }
  else if constexpr (_VMTS::is_range_v<T>) {
    stream << "{";
    print_element(stream, t.min);
    stream << ", ";
    print_element(stream, t.max);
    stream << "}";
  }
  else if constexpr (_VMTS::is_tuple_v<T>) {
    stream << "{";
    print_tuple(stream, t);
    stream << "}";
  }
  else if constexpr (_VMTS::is_complex_v<T>) {
    stream << "{";
    print_element(stream, t.real());
    stream << ", ";
    print_element(stream, t.imag());
    stream << "}";
  }
  else {
    stream << "unknown";
  }
}

struct empty_string {
  static constexpr const char* value = "";
};

struct space_string {
  static constexpr const char* value = " ";
};

struct comma_string {
  static constexpr const char* value = ",";
};

struct comma_space_string {
  static constexpr const char* value = ", ";
};

struct equal_string {
  static constexpr const char* value = " = ";
};

template <typename D = comma_space_string, typename T, typename... Ts>
inline void basic_print(std::ostream& stream, const T& t, const Ts&... ts) {
  if constexpr (sizeof...(ts) > 0) {
    print_element(stream, t);
    stream << D::value;
    basic_print<D>(stream, ts...);
  }
  else {
    print_element(stream, t);
    stream << std::endl;
  }
}

template <typename D = comma_space_string, typename Msg, typename T, typename... Ts>
inline void basic_header_print(std::ostream& stream, Msg&& msg, const T& t, const Ts&... ts) {
  stream << "[" << msg << "] : ";
  basic_print<D>(stream, t, ts...);
}

template <typename D = comma_space_string, typename T, typename... Ts>
inline void print(const T& t, const Ts&... ts) {
  basic_print<D>(std::cout, t, ts...);
}

template <typename D = comma_space_string, typename T, typename... Ts>
inline void errprint(const T& t, const Ts&... ts) {
  basic_header_print<D>(std::cerr, " ERROR ", t, ts...);
}

template <typename D = comma_space_string, typename T, typename... Ts>
inline void warnprint(const T& t, const Ts&... ts) {
  basic_header_print<D>(std::cerr, "WARNING", t, ts...);
}

template <typename D = comma_space_string, typename T, typename... Ts>
inline void tmprint(const T& t, const Ts&... ts) {
  basic_header_print<D>(std::cout, current_timems_string(), t, ts...);
}

namespace custom_print_detail {
template <class T>
using separator_t = typename T::separator;

template <class T>
using prefix_t = decltype(T::prefix(std::declval<std::ostream&>()));
} // namespace custom_print_detail.

template <typename Descriptor, typename... Ts>
inline void custom_print(std::ostream& stream, const Ts&... ts) {
  using has_prefix = _VMTS::is_detected<custom_print_detail::prefix_t, Descriptor>;

  if constexpr (has_prefix::value) {
    using is_invocable = std::is_invocable_r<std::ostream&, decltype(Descriptor::prefix), std::ostream&>;
    if constexpr (is_invocable::value) {
      Descriptor::prefix(stream);
    }
    else {
      MTS_STATIC_WARNING(
          "mts::custom_print Descriptor prefix function should be static std::ostream& prefix(std::ostream&);",
          is_invocable::value);
    }
  }

  if constexpr (_VMTS::is_detected_v<custom_print_detail::separator_t, Descriptor>) {
    basic_print<typename Descriptor::separator>(stream, ts...);
  }
  else {
    basic_print<comma_space_string>(stream, ts...);
  }
}

inline void initialize_print() {
  std::cout.sync_with_stdio(false);
  std::cerr.sync_with_stdio(false);
}

MTS_END_NAMESPACE
