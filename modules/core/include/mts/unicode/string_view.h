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
#include "mts/unicode/convert.h"
#include <string>
#include <string_view>

#if defined(MTS_UNICODE_STRING_VIEW_NO_ASSERT) || defined(MTS_UNIT_TESTS)
  #define MTS_UNICODE_STRING_VIEW_ASSERT(x, msg)
#else
  #include "mts/assert.h"
  #define MTS_UNICODE_STRING_VIEW_ASSERT(x, msg) mts_assert(x, msg)
#endif // MTS_UNICODE_STRING_VIEW_NO_ASSERT

MTS_BEGIN_SUB_NAMESPACE(unicode)

// Forward declared.
class string_view;

/// Converts any string type to a utf8 std::string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
MTS_NODISCARD inline std::string to_utf8(string_view s);

/// Converts any string type to a utf16 std::u16string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
MTS_NODISCARD inline std::u16string to_utf16(string_view s);

/// Converts any string type to a utf32 std::u32string.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
MTS_NODISCARD inline std::u32string to_utf32(string_view s);

/// Converts any string type to a std::wstring.
/// @remarks On Windows, functions from stringapiset.h are used for convertion.
MTS_NODISCARD inline std::wstring to_wide(string_view s);

/*!
 * @brief   Generalization of a std::basic_string_view that accepts any char type.
 *
 * @details The mts::unicode::string_view describes an object that can refer to a constant
 *          contiguous sequence of any char-like objects with the first element of
 *          the sequence at position zero.
 *
 *          This class is intended to be used when you need to support multiple
 *          character types and don't want to write a function for each of them.
 *
 *          It also prevents from creating unnecessary std::basic_string
 *          when passing a string literals or a std::basic_string_view to a function.
 *
 * @warning Keep in mind that the size of wchar_t is 2 bytes on Windows and 4 bytes
 *          on other platforms.
 *
 * Here's an example on how this could be used for drawing multiple string types (drawText)
 * or how the string_view could be converted to any string type and help to abstract a
 * cross-platform implementation (setText) :
 *
 * @code
 *   // In example.h
 *   void drawText(mts::unicode::string_view text);
 *   void setText(mts::unicode::string_view text);
 *
 *   // In example.cpp
 *   void drawText(mts::unicode::string_view text) {
 *     switch(text.encoding()) {
 *       case mts::unicode::string_view::UTF8:
 *         drawUtf8Text(text.u8view());
 *         break;
 *       case mts::unicode::string_view::UTF16:
 *         drawUtf16Text(text.u16view());
 *         break;
 *       case mts::unicode::string_view::UTF32:
 *         drawUtf32Text(text.u32view());
 *         break;
 *     }
 *   }
 *
 *   #ifdef _WIN32
 *   void setTextWindows(const std::wstring&) { ... }
 *   #else
 *   void setTextMac(const std::string&) { ... }
 *   #endif
 *
 *   void setText(mts::unicode::string_view text) {
 *     #ifdef _WIN32
 *     setTextWindows(text.to_wide());
 *     #else
 *     setTextMac(text.to_utf8());
 *     #endif
 *   }
 *
 *   // In main.cpp
 *   #include "example.h"
 *
 *   int main() {
 *     std::string text = "Text";
 *     std::wstring wtext = L"Text";
 *
 *     drawText(text);
 *     drawText(wtext);
 *     drawText("Text");
 *     drawText(L"Text");
 *
 *     setText(text);
 *     setText(wtext);
 *     setText("Text");
 *     setText(L"Text");
 *     return 0;
 *   }
 * @endcode
 */
class string_view {
  template <typename T>
  using is_any_string_view_constructible
      = mts::is_one_of_constructible<T, std::string_view, std::u16string_view, std::u32string_view, std::wstring_view>;

  /// Helper to make sure that T is a valid string input when calling
  /// the string_view(T&&) constructor.
  template <typename T>
  using enable_if_string_view_constructible = mts::enable_if_t<is_any_string_view_constructible<T>::value>;

  /// Helper to make sure that the input CharT is one of these:
  /// char, char16_t, char32_t, wchar_t
  template <typename CharT>
  using enable_if_valid_char_type
      = mts::enable_if_t<mts::is_one_of_type<CharT, char, char16_t, char32_t, wchar_t>::value>;

public:
  /// String encoding.
  enum Encoding { UTF8, UTF16, UTF32 };

  /// Default constructor.
  /// Creates an empty string view of char type (UTF8).
  inline string_view() noexcept;

  /// Constructor.
  /// Takes any possible kind of string input:
  ///
  /// Strings:
  /// const std::string&
  /// const std::u16string&
  /// const std::u32string&
  /// const std::wstring&
  ///
  /// Views:
  /// std::string_view
  /// std::u16string_view
  /// std::u32string_view
  /// std::wstring_view
  ///
  /// Pointers:
  /// const char*
  /// const char16_t*
  /// const char32_t*
  /// const wchar_t*
  ///
  /// Literals:
  /// const char(&)[N]
  /// const char16_t(&)[N]
  /// const char32_t(&)[N]
  /// const wchar_t(&)[N]
  template <typename T, enable_if_string_view_constructible<T> = mts::enabled>
  inline string_view(T&& s);

  string_view(string_view&&) noexcept = default;
  string_view(const string_view&) noexcept = default;

  string_view& operator=(string_view&&) noexcept = default;
  string_view& operator=(const string_view&) noexcept = default;

  /// Indicates if the character array is null terminated.
  MTS_NODISCARD inline bool null_terminated() const noexcept { return m_nullTerminated; }

  /// Returns true if the character array contains no characters.
  MTS_NODISCARD inline bool empty() const noexcept { return !m_size; }

  /// Returns the number of characters.
  MTS_NODISCARD inline std::size_t size() const noexcept { return static_cast<std::size_t>(m_size); }

  /// Returns the size of a single char (i.e. sizeof(char_type)).
  MTS_NODISCARD inline std::size_t char_size() const noexcept { return m_charSize; }

  /// Returns the size of the string in bytes (i.e. size() * char_size()).
  MTS_NODISCARD inline std::size_t size_bytes() const noexcept { return m_size * char_size(); }

  /// Returns the human readable character count.
  MTS_NODISCARD inline std::size_t count() const;

  /// Returns the encoding.
  MTS_NODISCARD inline Encoding encoding() const noexcept;

  /// Returns a pointer to a null-terminated character array with data equivalent to those
  /// stored in the string. The pointer is such that the range [c_str(); c_str() + size()]
  /// is valid and the values in it correspond to the values stored in the string with an
  /// additional null character after the last position.
  ///
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          A nullptr is returned if sizeof(CharT) != char_size().
  ///
  /// @warning This can only be called when null_terminated() returns true.
  ///          A nullptr is returned if null_terminated() == false.
  ///
  /// @see     data() or view() for non null-terminated strings.
  template <typename CharT, enable_if_valid_char_type<CharT> = mts::enabled>
  MTS_NODISCARD inline const CharT* c_str() const noexcept;

  /// Same as c_str<char>().
  MTS_NODISCARD inline const char* u8cstr() const noexcept { return c_str<char>(); }

  /// Same as c_str<char16_t>().
  MTS_NODISCARD inline const char16_t* u16cstr() const noexcept { return c_str<char16_t>(); }

  /// Same as c_str<char32_t>().
  MTS_NODISCARD inline const char32_t* u32cstr() const noexcept { return c_str<char32_t>(); }

  /// Same as c_str<wchar_t>().
  MTS_NODISCARD inline const wchar_t* wcstr() const noexcept { return c_str<wchar_t>(); }

  /// Returns a pointer to the character array.
  /// The pointer is such that the range [data(); data() + size()) is valid and the values
  /// in it correspond to the values of the view.
  ///
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          A nullptr is returned if sizeof(CharT) != char_size().
  ///
  /// @warning Unlike std::basic_string::data() and string literals, data() may return a pointer
  ///          to a buffer that is not null-terminated. Therefore it is typically a mistake to
  ///          pass data() to a routine that takes just a const CharT* and expects a
  ///          null-terminated string.
  ///
  /// @see     null_terminated()
  template <typename CharT, enable_if_valid_char_type<CharT> = mts::enabled>
  MTS_NODISCARD inline const CharT* data() const noexcept;

  /// Same as data<char>().
  MTS_NODISCARD inline const char* u8data() const noexcept { return data<char>(); }

  /// Same as data<char16_t>().
  MTS_NODISCARD inline const char16_t* u16data() const noexcept { return data<char16_t>(); }

  /// Same as data<char32_t>().
  MTS_NODISCARD inline const char32_t* u32data() const noexcept { return data<char32_t>(); }

  /// Same as data<wchar_t>().
  MTS_NODISCARD inline const wchar_t* wdata() const noexcept { return data<wchar_t>(); }

  /// Returns a std::basic_string<CharT>.
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          An empty std::basic_string<CharT> is returned if sizeof(CharT) != char_size().
  template <typename CharT, enable_if_valid_char_type<CharT> = mts::enabled>
  MTS_NODISCARD inline std::basic_string<CharT> str() const;

  /// Same as str<char>()
  MTS_NODISCARD inline std::string u8str() const noexcept { return str<char>(); }

  /// Same as str<char16_t>()
  MTS_NODISCARD inline std::basic_string<char16_t> u16str() const noexcept { return str<char16_t>(); }

  /// Same as str<char32_t>()
  MTS_NODISCARD inline std::basic_string<char32_t> u32str() const noexcept { return str<char32_t>(); }

  /// Same as str<wchar_t>()
  MTS_NODISCARD inline std::wstring wstr() const noexcept { return str<wchar_t>(); }

  /// Returns a std::basic_string_view<CharT>.
  /// @warning This can only be called with a CharT matching the current char_size().
  ///          An empty std::basic_string_view<CharT> is returned if sizeof(CharT) != char_size().
  template <typename CharT, enable_if_valid_char_type<CharT> = mts::enabled>
  MTS_NODISCARD inline std::basic_string_view<CharT> view() const;

  /// Same as view<char>().
  MTS_NODISCARD inline std::string_view u8view() const noexcept { return view<char>(); }

  /// Same as view<char16_t>().
  MTS_NODISCARD inline std::basic_string_view<char16_t> u16view() const noexcept { return view<char16_t>(); }

  /// Same as view<char32_t>().
  MTS_NODISCARD inline std::basic_string_view<char32_t> u32view() const noexcept { return view<char32_t>(); }

  /// Same as view<wchar_t>().
  MTS_NODISCARD inline std::wstring_view wview() const noexcept { return view<wchar_t>(); }

  /// Converts the character array to a utf8 std::string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  MTS_NODISCARD inline std::string to_utf8() const;

  /// Converts the character array to a utf16 std::u16string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  MTS_NODISCARD inline std::u16string to_utf16() const;

  /// Converts the character array to a utf32 std::u32string.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  MTS_NODISCARD inline std::u32string to_utf32() const;

  /// Converts the character array to a std::wstring.
  /// @remarks On Windows, functions from stringapiset.h are used for convertion.
  MTS_NODISCARD inline std::wstring to_wide() const;

private:
  union content {
    inline content() noexcept;
    inline content(const char* s) noexcept;
    inline content(const char16_t* s) noexcept;
    inline content(const char32_t* s) noexcept;
    inline content(const wchar_t* s) noexcept;

    inline const wchar_t* cw() const noexcept;

    const char* c8;
    const char16_t* c16;
    const char32_t* c32;
  } m_data;

  std::uint32_t m_size;
  int m_charSize;
  bool m_nullTerminated;

  struct normal_tag {};

  struct null_terminated_tag {};

  template <typename T>
  inline static auto make(T&& s);

  inline string_view(const std::string& str, normal_tag) noexcept;
  inline string_view(const std::u16string& str, normal_tag) noexcept;
  inline string_view(const std::u32string& str, normal_tag) noexcept;
  inline string_view(const std::wstring& str, normal_tag) noexcept;
  inline string_view(std::string_view str, normal_tag) noexcept;
  inline string_view(std::u16string_view str, normal_tag) noexcept;
  inline string_view(std::u32string_view str, normal_tag) noexcept;
  inline string_view(std::wstring_view str, normal_tag) noexcept;
  inline string_view(std::string_view str, null_terminated_tag) noexcept;
  inline string_view(std::u16string_view str, null_terminated_tag) noexcept;
  inline string_view(std::u32string_view str, null_terminated_tag) noexcept;
  inline string_view(std::wstring_view str, null_terminated_tag) noexcept;
};

//
// MARK: Implementation.
//

template <typename T, string_view::enable_if_string_view_constructible<T>>
string_view::string_view(T&& s)
    : string_view(make<T>(std::forward<T>(s))) {}

string_view::string_view() noexcept
    : m_data()
    , m_size(0)
    , m_charSize(sizeof(char))
    , m_nullTerminated(false) {}

string_view::string_view(const std::string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(true) {}

string_view::string_view(const std::u16string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(true) {}

string_view::string_view(const std::u32string& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(true) {}

string_view::string_view(const std::wstring& str, normal_tag) noexcept
    : m_data{ str.c_str() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(false) {}

string_view::string_view(std::u16string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::u32string_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::wstring_view str, normal_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(false) {}

string_view::string_view(std::string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char))
    , m_nullTerminated(true) {}

string_view::string_view(std::u16string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char16_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::u32string_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(char32_t))
    , m_nullTerminated(true) {}

string_view::string_view(std::wstring_view str, null_terminated_tag) noexcept
    : m_data{ str.data() }
    , m_size((std::uint32_t)str.size())
    , m_charSize(sizeof(wchar_t))
    , m_nullTerminated(true) {}

string_view::content::content() noexcept
    : c8(nullptr) {}

string_view::content::content(const char* s) noexcept
    : c8(s) {}

string_view::content::content(const char16_t* s) noexcept
    : c16(s) {}

string_view::content::content(const char32_t* s) noexcept
    : c32(s) {}

string_view::content::content(const wchar_t* s) noexcept
    : content([](const wchar_t* s) {
      if constexpr (sizeof(wchar_t) == sizeof(char16_t)) {
        return content((const char16_t*)s);
      }
      else {
        return content((const char32_t*)s);
      }
    }(s)) {}

const wchar_t* string_view::content::cw() const noexcept {
  if constexpr (sizeof(wchar_t) == sizeof(char16_t)) {
    return (const wchar_t*)(const void*)c16;
  }
  else {
    return (const wchar_t*)c32;
  }
}

string_view::Encoding string_view::encoding() const noexcept {
  switch (char_size()) {
  case sizeof(char):
    return UTF8;
  case sizeof(char16_t):
    return UTF16;
  case sizeof(char32_t):
    return UTF32;
  default:
    return UTF8;
  }
}

template <typename CharT, string_view::enable_if_valid_char_type<CharT>>
const CharT* string_view::c_str() const noexcept {
  if constexpr (std::is_same_v<CharT, char>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char) && m_nullTerminated) ? m_data.c8 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char16_t) && m_nullTerminated) ? m_data.c16 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(char32_t) && m_nullTerminated) ? m_data.c32 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t) && m_nullTerminated, "Wrong char type");
    return (m_charSize == sizeof(wchar_t) && m_nullTerminated) ? m_data.cw() : nullptr;
  }

  else {
    // Should never happen.
    mts_error("Wrong char type");
    return nullptr;
  }
}

template <typename CharT, string_view::enable_if_valid_char_type<CharT>>
const CharT* string_view::data() const noexcept {
  if constexpr (std::is_same_v<CharT, char>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? m_data.c8 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? m_data.c16 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? m_data.c32 : nullptr;
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? m_data.cw() : nullptr;
  }

  else {
    // Should never happen.
    mts_error("Wrong char type");
    return nullptr;
  }
}

template <typename CharT, string_view::enable_if_valid_char_type<CharT>>
std::basic_string<CharT> string_view::str() const {
  using stype = std::basic_string<CharT>;

  if constexpr (std::is_same_v<CharT, char>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? stype(m_data.c8, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? stype(m_data.c16, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? stype(m_data.c32, m_size) : stype();
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? stype(m_data.cw(), m_size) : stype();
  }

  else {
    // Should never happen.
    mts_error("Wrong char type");
    return stype();
  }
}

template <typename CharT, string_view::enable_if_valid_char_type<CharT>>
std::basic_string_view<CharT> string_view::view() const {
  using svtype = std::basic_string_view<CharT>;

  if constexpr (std::is_same_v<CharT, char>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char), "Wrong char type");
    return (m_charSize == sizeof(char)) ? svtype(m_data.c8, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, char16_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char16_t), "Wrong char type");
    return (m_charSize == sizeof(char16_t)) ? svtype(m_data.c16, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, char32_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(char32_t), "Wrong char type");
    return (m_charSize == sizeof(char32_t)) ? svtype(m_data.c32, m_size) : svtype();
  }

  else if constexpr (std::is_same_v<CharT, wchar_t>) {
    MTS_UNICODE_STRING_VIEW_ASSERT(m_charSize == sizeof(wchar_t), "Wrong char type");
    return (m_charSize == sizeof(wchar_t)) ? svtype(m_data.cw(), m_size) : svtype();
  }

  else {
    // Should never happen.
    mts_error("Wrong char type");
    return svtype();
  }
}

template <typename T>
auto string_view::make(T&& s) {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;

  constexpr bool is_any_string_or_string_view = mts::is_one_of_type<type, std::string, std::u16string, std::u32string,
      std::wstring, std::string_view, std::u16string_view, std::u32string_view, std::wstring_view>::value;

  static_assert(is_any_string_or_string_view || is_any_string_view_constructible<T>::value, "");

  if constexpr (is_any_string_or_string_view) {
    return string_view(std::forward<T>(s), normal_tag{});
  }

  else if constexpr (std::is_constructible_v<std::string_view, T>) {
    return string_view(std::string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::u16string_view, T>) {
    return string_view(std::u16string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::u32string_view, T>) {
    return string_view(std::u32string_view(s), null_terminated_tag{});
  }

  else if constexpr (std::is_constructible_v<std::wstring_view, T>) {
    return string_view(std::wstring_view(s), null_terminated_tag{});
  }

  else {
    // Should never happen.
    mts_error("Wrong char type");
    return string_view();
  }
}

std::string to_utf8(string_view s) { return string_view(s).to_utf8(); }

std::u16string to_utf16(string_view s) { return string_view(s).to_utf16(); }

std::u32string to_utf32(string_view s) { return string_view(s).to_utf32(); }

std::wstring to_wide(string_view s) { return string_view(s).to_wide(); }

std::string string_view::to_utf8() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case UTF8:
    return std::string(data<char>(), size());

  case UTF16:
    return mts::unicode::convert_as<char>(view<char16_t>());

  case UTF32:
    return mts::unicode::convert_as<char>(view<char32_t>());
  }

  return {};
}

std::u16string string_view::to_utf16() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case UTF8:
    return mts::unicode::convert_as<char16_t>(view<char>());

  case UTF16:

    return std::u16string(data<char16_t>(), size());

  case UTF32:
    return mts::unicode::convert_as<char16_t>(view<char32_t>());
  }

  return {};
}

std::u32string string_view::to_utf32() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case UTF8:
    return mts::unicode::convert_as<char32_t>(view<char>());

  case UTF16:
    return mts::unicode::convert_as<char32_t>(view<char16_t>());

  case UTF32:
    return std::u32string(data<char32_t>(), size());
  }

  return {};
}

std::wstring string_view::to_wide() const {
  if (empty()) {
    return {};
  }

  switch (encoding()) {
  case UTF8:
    return mts::unicode::convert_as<wchar_t>(view<char>());

  case UTF16:
    return mts::unicode::convert_as<wchar_t>(view<char16_t>());

  case UTF32:
    return mts::unicode::convert_as<wchar_t>(view<char32_t>());
  }

  return {};
}

std::size_t string_view::count() const {
  switch (encoding()) {
  case UTF8:
    return mts::unicode::length(view<char>());

  case UTF16:
    return mts::unicode::length(view<char16_t>());

  case UTF32:
    return size();
  }
  return 0;
}
MTS_END_SUB_NAMESPACE(unicode)

MTS_BEGIN_NAMESPACE
using ustring_view = unicode::string_view;
MTS_END_NAMESPACE
