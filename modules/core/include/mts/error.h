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
#include <array>
#include <ostream>
#include <string>
#include <system_error>
#include <type_traits>

MTS_BEGIN_NAMESPACE
enum class errc {
  exception_error = 1,

  // This should be avoided but is here to cover some corner cases where an
  // unknown error is needed. This could also be used during development, before
  // defining your own error codes.
  unknown = 2
};

const std::error_category& generic_category() noexcept;

inline std::error_code make_error_code(_VMTS::errc ec) noexcept { return { static_cast<int>(ec), generic_category() }; }

/// @class error_result
///
/// This class basically encapsulates a std::error_code and extends it with an
/// optional extra content string. Your error codes should be precise enough for
/// you to rarely need the content string.
class error_result {
public:
  /// Default constructor creates a success value with error code 0.
  error_result() noexcept = default;
  error_result(const error_result&) = default;
  error_result(error_result&&) noexcept = default;

  inline error_result(const std::error_code& __ec) noexcept
      : m_ec(__ec) {}

  inline error_result(const std::error_code& __ec, const std::string& __content)
      : m_ec(__ec)
      , m_content(__content) {}

  inline error_result(const std::error_code& __ec, const char* __content)
      : m_ec(__ec)
      , m_content(__content) {}

  error_result(const std::string& __content)
      : m_ec(make_error_code(errc::unknown))
      , m_content(__content) {}

  error_result(const char* __content)
      : m_ec(make_error_code(errc::unknown))
      , m_content(__content) {}

  ~error_result() = default;

  error_result& operator=(const error_result&) = default;
  error_result& operator=(error_result&&) = default;

  const std::error_code& code() const noexcept { return m_ec; }

  const std::string& content() const noexcept { return m_content; }

  const std::error_category& category() const noexcept { return m_ec.category(); }

  std::string message() const noexcept;

  inline explicit operator bool() const noexcept { return (bool)m_ec; }

  friend std::ostream& operator<<(std::ostream& stream, const error_result& er);

private:
  std::error_code m_ec;
  std::string m_content;
};

MTS_END_NAMESPACE

inline bool operator==(const _VMTS::error_result& __x, const std::error_code& __y) noexcept {
  return __x.code() == __y;
}

inline bool operator==(const std::error_code& __x, const _VMTS::error_result& __y) noexcept {
  return __x == __y.code();
}

inline bool operator==(const _VMTS::error_result& __x, const std::error_condition& __y) noexcept {
  return __x.code() == __y;
}

inline bool operator==(const std::error_condition& __x, const _VMTS::error_result& __y) noexcept {
  return __x == __y.code();
}

inline bool operator!=(const _VMTS::error_result& __x, const std::error_code& __y) noexcept {
  return __x.code() != __y;
}

inline bool operator!=(const std::error_code& __x, const _VMTS::error_result& __y) noexcept {
  return __x != __y.code();
}

inline bool operator!=(const _VMTS::error_result& __x, const std::error_condition& __y) noexcept {
  return __x.code() != __y;
}

inline bool operator!=(const std::error_condition& __x, const _VMTS::error_result& __y) noexcept {
  return __x != __y.code();
}

namespace std {
template <>
struct is_error_code_enum<_VMTS::errc> : std::true_type {};

inline std::error_code make_error_code(_VMTS::errc ec) noexcept { return _VMTS::make_error_code(ec); }
} // namespace std.
