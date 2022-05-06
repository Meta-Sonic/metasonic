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
#include "mts/iterator.h"
#include "mts/traits.h"
#include "mts/util.h"

MTS_BEGIN_SUB_NAMESPACE(unicode)

/// is_valid_char_type.
template <typename CharT>
using is_valid_char_type = mts::is_one_of_type<std::remove_cv_t<CharT>, char, char8_t, char16_t, char32_t, wchar_t>;

template <typename CharT>
inline constexpr bool is_valid_char_type_v = is_valid_char_type<CharT>::value;

/// is_valid_char_size.
template <std::size_t CharSize>
struct is_valid_char_size {
  static constexpr bool value = mts::is_one_of(CharSize, sizeof(char), sizeof(char16_t), sizeof(char32_t));
};

template <std::size_t CharSize>
inline constexpr bool is_valid_char_size_v = is_valid_char_size<CharSize>::value;

/// Encoding.
enum class encoding { utf8, utf16, utf32 };

/// encoding_to_char_size.
template <encoding Encoding>
struct encoding_to_char_size {
  static constexpr std::size_t value = []() {
    if constexpr (Encoding == encoding::utf8) {
      return sizeof(char);
    }
    else if constexpr (Encoding == encoding::utf16) {
      return sizeof(char16_t);
    }
    else if constexpr (Encoding == encoding::utf32) {
      return sizeof(char32_t);
    }
  }();
};

template <encoding Encoding>
inline constexpr std::size_t encoding_to_char_size_v = encoding_to_char_size<Encoding>::value;

template <encoding Encoding>
struct encoding_to_max_char_count {
  static constexpr std::size_t value = []() {
    if constexpr (Encoding == encoding::utf8) {
      return 4;
    }
    else if constexpr (Encoding == encoding::utf16) {
      return 2;
    }
    else if constexpr (Encoding == encoding::utf32) {
      return 1;
    }
  }();
};

/// char_size_to_encoding.
template <std::size_t CharSize, mts::enable_if_t<is_valid_char_size_v<CharSize>> = mts::enabled>
struct char_size_to_encoding {
  static constexpr encoding value = []() {
    if constexpr (CharSize == sizeof(char)) {
      return encoding::utf8;
    }
    else if constexpr (CharSize == sizeof(char16_t)) {
      return encoding::utf16;
    }
    else if constexpr (CharSize == sizeof(char32_t)) {
      return encoding::utf32;
    }
  }();
};

template <std::size_t CharSize, mts::enable_if_t<is_valid_char_size_v<CharSize>> = mts::enabled>
inline constexpr encoding char_size_to_encoding_v = char_size_to_encoding<CharSize>::value;

/// char_type_to_encoding.
template <typename CharT, mts::enable_if_t<is_valid_char_type_v<CharT>> = mts::enabled>
struct char_type_to_encoding {
  static constexpr encoding value = char_size_to_encoding_v<sizeof(CharT)>;
};

template <typename CharT, mts::enable_if_t<is_valid_char_type_v<CharT>> = mts::enabled>
inline constexpr encoding char_type_to_encoding_v = char_type_to_encoding<CharT>::value;

// is_valid_string_type.
template <class SType>
using is_valid_string_type = mts::is_one_of_constructible<SType, std::string_view, std::u8string_view,
    std::u16string_view, std::u32string_view, std::wstring_view>;

template <class SType>
inline constexpr bool is_valid_string_type_v = is_valid_string_type<SType>::value;

namespace detail {
template <class SType, class CharT>
using enable_if_string_view_constructible
    = std::enable_if_t<std::is_constructible_v<std::basic_string_view<CharT>, mts::remove_cvref_t<SType>>>;
} // namespace detail.

// string_char_type.
template <class SType, class = void>
struct string_char_type {
  using type = void;
};

template <class SType>
struct string_char_type<SType, detail::enable_if_string_view_constructible<SType, char>> {
  using type = char;
};

template <class SType>
struct string_char_type<SType, detail::enable_if_string_view_constructible<SType, char8_t>> {
  using type = char8_t;
};

template <class SType>
struct string_char_type<SType, detail::enable_if_string_view_constructible<SType, char16_t>> {
  using type = char16_t;
};

template <class SType>
struct string_char_type<SType, detail::enable_if_string_view_constructible<SType, char32_t>> {
  using type = char32_t;
};

template <class SType>
struct string_char_type<SType, detail::enable_if_string_view_constructible<SType, wchar_t>> {
  using type = wchar_t;
};

template <class SType>
using string_char_type_t = typename string_char_type<SType>::type;

//**********************************************************************************************
// Implementation.
//**********************************************************************************************
namespace detail {
inline constexpr const std::uint16_t k_lead_surrogate_min = 0xD800u;
inline constexpr const std::uint16_t k_lead_offset = k_lead_surrogate_min - (0x10000 >> 10);
inline constexpr const std::uint16_t k_trail_surrogate_min = 0xDC00u;

/// Maximum valid value for a Unicode code point.
inline constexpr const std::uint32_t k_code_point_max = 0x0010FFFFu;
inline constexpr const std::uint32_t k_surrogate_offset
    = 0x10000u - (k_lead_surrogate_min << 10) - k_trail_surrogate_min;

inline constexpr const std::uint8_t bom[] = { 0xef, 0xbb, 0xbf };

template <typename u8char_type>
inline std::uint8_t cast_8(u8char_type c) noexcept {
  return static_cast<std::uint8_t>(c);
}

template <typename u16char_type>
inline std::uint16_t cast_16(u16char_type c) noexcept {
  return static_cast<std::uint16_t>(c);
}

template <typename u8char_type>
inline bool is_trail(u8char_type c) {
  return (cast_8(c) >> 6) == 0x2;
}

inline constexpr bool is_surrogate(char16_t uc) noexcept { return (uc - k_lead_surrogate_min) < 2048u; }

// inline constexpr bool is_surrogate(char16_t uc) noexcept { return (uc - 0xD800u) < 2048u; }

// Lead.
inline constexpr bool is_high_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == k_lead_surrogate_min; }
// inline constexpr bool is_high_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xD800; }

// Trail.
inline constexpr bool is_low_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == k_trail_surrogate_min; }
// inline constexpr bool is_low_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xDC00; }

template <typename u32char_type>
inline constexpr bool is_valid_code_point(u32char_type cp) noexcept {
  return cp <= k_code_point_max && !is_surrogate(cp);
}

inline constexpr std::size_t sequence_length(std::uint8_t lead) noexcept {
  return lead < 0x80 ? 1 : (lead >> 5) == 0x6 ? 2 : (lead >> 4) == 0xE ? 3 : (lead >> 3) == 0x1E ? 4 : 0;
}

inline constexpr std::size_t u16_sequence_length(char16_t c) noexcept {
  std::uint32_t cp = detail::cast_16(c);
  return is_high_surrogate(cp) ? 2 : 1;
}

template <typename u8_iterator>
inline constexpr bool starts_with_bom(u8_iterator it, u8_iterator end) noexcept {
  return (((it != end) && (cast_8(*it++)) == bom[0]) && ((it != end) && (cast_8(*it++)) == bom[1])
      && ((it != end) && (cast_8(*it)) == bom[2]));
}
} // namespace detail.

template <typename u8_iterator>
inline u8_iterator append_u32_to_u8(std::uint32_t cp, u8_iterator it) {
  // 1 byte.
  if (cp < 0x80) {
    *it++ = detail::cast_8(cp);
  }
  // 2 bytes.
  else if (cp < 0x800) {
    *it++ = detail::cast_8((cp >> 6) | 0xC0);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }
  // 3 bytes.
  else if (cp < 0x10000) {
    *it++ = detail::cast_8((cp >> 12) | 0xE0);
    *it++ = detail::cast_8(((cp >> 6) & 0x3F) | 0x80);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }
  // 4 bytes.
  else {
    *it++ = detail::cast_8((cp >> 18) | 0xF0);
    *it++ = detail::cast_8(((cp >> 12) & 0x3F) | 0x80);
    *it++ = detail::cast_8(((cp >> 6) & 0x3F) | 0x80);
    *it++ = detail::cast_8((cp & 0x3F) | 0x80);
  }

  return it;
}

inline std::size_t code_point_size_u8(std::uint32_t cp) {
  // 1 byte.
  if (cp < 0x80) {
    return 1;
  }
  // 2 bytes.
  else if (cp < 0x800) {
    return 2;
  }
  // 3 bytes.
  else if (cp < 0x10000) {
    return 3;
  }
  // 4 bytes.
  else {
    return 4;
  }
}

template <typename u8_iterator>
inline std::uint32_t next_u8_to_u32(u8_iterator& it) {
  std::uint32_t cp = detail::cast_8(*it);
  mts::iterator_difference_type_t<u8_iterator> length = detail::sequence_length(*it);

  switch (length) {
  case 1:
    break;
  case 2:
    it++;
    cp = ((cp << 6) & 0x7FF) + ((*it) & 0x3F);
    break;
  case 3:
    ++it;
    cp = ((cp << 12) & 0xFFFF) + ((detail::cast_8(*it) << 6) & 0xFFF);
    ++it;
    cp += (*it) & 0x3F;
    break;
  case 4:
    ++it;
    cp = ((cp << 18) & 0x1FFFFF) + ((detail::cast_8(*it) << 12) & 0x3FFFF);
    ++it;
    cp += (detail::cast_8(*it) << 6) & 0xFFF;
    ++it;
    cp += (*it) & 0x3F;
    break;
  }
  ++it;
  return cp;
}

template <typename octet_iterator>
std::uint32_t prior_u8_to_u32(octet_iterator& it) {
  while (detail::is_trail(*(--it)))
    ;

  octet_iterator temp = it;
  return next_u8_to_u32(temp);
}

template <typename u16_iterator, typename u8_iterator>
u16_iterator u8_to_u16(u8_iterator start, u8_iterator end, u16_iterator outputIt) {
  while (start < end) {
    std::uint32_t cp = next_u8_to_u32(start);

    if (cp > 0xFFFF) { // make a surrogate pair
      *outputIt++ = detail::cast_16((cp >> 10) + detail::k_lead_offset);
      *outputIt++ = detail::cast_16((cp & 0x3FF) + detail::k_trail_surrogate_min);
    }
    else {
      *outputIt++ = detail::cast_16(cp);
    }
  }

  return outputIt;
}

template <typename u8_iterator>
std::size_t u8_to_u16_length(u8_iterator start, u8_iterator end) {
  std::size_t count = 0;
  while (start < end) {
    std::uint32_t cp = next_u8_to_u32(start);
    count += (cp > 0xFFFF) ? 2 : 1;
  }

  return count;
}

template <typename u32_iterator, typename u8_iterator>
u32_iterator u8_to_u32(u8_iterator start, u8_iterator end, u32_iterator outputIt) {
  while (start < end) {
    *outputIt++ = next_u8_to_u32(start);
  }

  return outputIt;
}

template <typename u8_iterator>
std::size_t u8_to_u32_length(u8_iterator start, u8_iterator end) {
  std::size_t count = 0;
  while (start < end) {
    switch (detail::sequence_length(*start++)) {
    case 1:
      break;
    case 2:
      ++start;
      break;
    case 3:
      ++start;
      ++start;
      break;
    case 4:
      ++start;
      ++start;
      ++start;
      break;
    }
    count++;
  }

  return count;
}

template <typename CharT, mts::enable_if_t<sizeof(CharT) == sizeof(char)> = mts::enabled>
std::size_t u8_length(const CharT* str, std::size_t size) noexcept {
  std::size_t dist = 0;

  for (std::size_t i = 0; i < size; i += detail::sequence_length(str[i])) {
    dist++;
  }

  return dist;
}

template <typename u16_iterator, typename u8_iterator>
u8_iterator u16_to_u8(u16_iterator start, u16_iterator end, u8_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    outputIt = append_u32_to_u8(cp, outputIt);
  }

  return outputIt;
}

template <typename u16_iterator, typename u32_iterator>
u32_iterator u16_to_u32(u16_iterator start, u16_iterator end, u32_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    *outputIt++ = cp;
  }

  return outputIt;
}

template <typename u16_iterator>
std::size_t u16_to_u8_length(u16_iterator start, u16_iterator end) {
  std::size_t count = 0;
  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    count += code_point_size_u8(cp);
  }

  return count;
}

template <typename u16_iterator>
std::size_t u16_to_u32_length(u16_iterator start, u16_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    std::uint32_t cp = detail::cast_16(*start++);

    // Take care of surrogate pairs first.
    if (detail::is_high_surrogate(cp)) {
      cp = (cp << 10) + ((std::uint32_t)detail::cast_16(*start++)) + detail::k_surrogate_offset;
    }

    count++;
  }

  return count;
}

template <typename CharT, mts::enable_if_t<sizeof(CharT) == sizeof(char16_t)> = mts::enabled>
std::size_t u16_length(const CharT* str, std::size_t size) noexcept {
  std::size_t dist = 0;

  for (std::size_t i = 0; i < size; i++) {
    if (detail::is_high_surrogate(detail::cast_16(str[i]))) {
      i++;
    }

    dist++;
  }

  return dist;
}

template <typename u8_iterator, typename u32_iterator>
u8_iterator u32_to_u8(u32_iterator start, u32_iterator end, u8_iterator outputIt) {
  while (start != end) {
    outputIt = append_u32_to_u8(*start++, outputIt);
  }

  return outputIt;
}

template <typename u16_iterator, typename u32_iterator>
u16_iterator u32_to_u16(u32_iterator start, u32_iterator end, u16_iterator outputIt) {
  while (start != end) {
    std::uint32_t cp = *start++;

    if (cp <= 0x0000FFFF) {
      // UTF-16 surrogate values are illegal in UTF-32
      // 0xFFFF or 0xFFFE are both reserved values.
      if (cp >= 0xD800 && cp <= 0xDFFF) {
        *outputIt++ = 0x0000FFFD;
      }
      else {
        // BMP character.
        *outputIt++ = (std::int16_t)cp;
      }
    }
    else if (cp > 0x0010FFFF) {
      // U+10FFFF is the largest code point of Unicode character set.
      *outputIt++ = (std::int16_t)0x0000FFFD;
    }
    else {
      // c32 is a character in range 0xFFFF - 0x10FFFF.
      cp -= 0x0010000UL;
      *outputIt++ = (std::int16_t)((cp >> 10) + 0xD800);
      *outputIt++ = (std::int16_t)((cp & 0x3FFUL) + 0xDC00);
    }
  }

  return outputIt;
}

template <typename u32_iterator>
std::size_t u32_to_u8_length(u32_iterator start, u32_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    count += code_point_size_u8(*start++);
  }

  return count;
}

template <typename u32_iterator>
std::size_t u32_to_u16_length(u32_iterator start, u32_iterator end) {
  std::size_t count = 0;

  while (start != end) {
    std::uint32_t cp = *start++;
    count += (cp <= 0x0000FFFF || cp > 0x0010FFFF) ? 1 : 2;
  }

  return count;
}

template <typename CharT, mts::enable_if_t<sizeof(CharT) == sizeof(char32_t)> = mts::enabled>
std::size_t u32_length(const CharT*, std::size_t size) noexcept {
  return size;
}

namespace detail {
template <class SType>
using enable_if_valid_string_type = mts::enable_if_t<is_valid_string_type_v<SType>>;

template <class SType, typename CharT>
using enable_if_valid_string_and_char_type
    = mts::enable_if_t<is_valid_string_type_v<SType> && is_valid_char_type_v<CharT>>;
} // namespace detail.

template <typename CharT, typename SType, detail::enable_if_valid_string_and_char_type<SType, CharT> = mts::enabled>
inline std::size_t convert_size(const SType& str) {
  using input_char_type = mts::unicode::string_char_type_t<SType>;
  constexpr encoding input_encoding = mts::unicode::char_type_to_encoding_v<input_char_type>;

  using output_char_type = CharT;
  constexpr encoding output_encoding = mts::unicode::char_type_to_encoding_v<output_char_type>;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      return input_view.size();
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u8_to_u16_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u8_to_u32_length(input_view.begin(), input_view.end());
    }
    else {
      return 0;
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      return u16_to_u8_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return input_view.size();
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u16_to_u32_length(input_view.begin(), input_view.end());
    }
    else {
      return 0;
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      return u32_to_u8_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u32_to_u16_length(input_view.begin(), input_view.end());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return input_view.size();
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }
}

template <typename CharT, typename SType, detail::enable_if_valid_string_and_char_type<SType, CharT> = mts::enabled>
inline std::basic_string<CharT> convert_as(const SType& str) {
  using input_char_type = mts::unicode::string_char_type_t<SType>;
  constexpr encoding input_encoding = mts::unicode::char_type_to_encoding_v<input_char_type>;

  using output_char_type = CharT;
  constexpr encoding output_encoding = mts::unicode::char_type_to_encoding_v<output_char_type>;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else if constexpr (output_encoding == encoding::utf16) {
      std::basic_string<output_char_type> output;
      u8_to_u16(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      std::basic_string<output_char_type> output;
      u8_to_u32(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else {
      return {};
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      std::basic_string<output_char_type> output;
      u16_to_u8(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else if constexpr (output_encoding == encoding::utf32) {
      std::basic_string<output_char_type> output;
      u16_to_u32(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else {
      return {};
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      std::basic_string<output_char_type> output;
      u32_to_u8(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      std::basic_string<output_char_type> output;
      u32_to_u16(input_view.begin(), input_view.end(), std::back_inserter(output));
      return output;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return std::basic_string<output_char_type>((const output_char_type*)input_view.data(), input_view.size());
    }
    else {
      return {};
    }
  }
  else {
    return {};
  }
}

template <class SType, detail::enable_if_valid_string_type<SType> = mts::enabled>
class convert {
public:
  using input_char_type = mts::unicode::string_char_type_t<SType>;
  static constexpr encoding input_encoding = mts::unicode::char_type_to_encoding_v<input_char_type>;

  inline convert(const SType& str)
      : _input_view(str) {}

  template <typename CharT>
  inline operator std::basic_string<CharT>() const {
    return convert_as<CharT>(_input_view);
  }

private:
  std::basic_string_view<input_char_type> _input_view;
};

template <class SType, class OutputIt, detail::enable_if_valid_string_type<SType> = mts::enabled>
inline OutputIt copy(const SType& str, OutputIt outputIt) {

  using input_char_type = mts::unicode::string_char_type_t<SType>;
  constexpr encoding input_encoding = mts::unicode::char_type_to_encoding_v<input_char_type>;

  using output_char_type = mts::output_iterator_value_type_t<OutputIt>;
  constexpr encoding output_encoding = mts::unicode::char_type_to_encoding_v<output_char_type>;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    if constexpr (output_encoding == encoding::utf8) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u8_to_u16(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u8_to_u32(input_view.begin(), input_view.end(), outputIt);
    }
  }
  else if constexpr (input_encoding == encoding::utf16) {
    if constexpr (output_encoding == encoding::utf8) {
      return u16_to_u8(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf16) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
    else if constexpr (output_encoding == encoding::utf32) {
      return u16_to_u32(input_view.begin(), input_view.end(), outputIt);
    }
  }
  else if constexpr (input_encoding == encoding::utf32) {
    if constexpr (output_encoding == encoding::utf8) {
      return u32_to_u8(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf16) {
      return u32_to_u16(input_view.begin(), input_view.end(), outputIt);
    }
    else if constexpr (output_encoding == encoding::utf32) {
      for (std::size_t i = 0; i < input_view.size(); i++) {
        *outputIt++ = (output_char_type)input_view[i];
      }
      return outputIt;
    }
  }
}

template <class SType, detail::enable_if_valid_string_type<SType> = mts::enabled>
inline std::size_t length(const SType& str) {
  using input_char_type = mts::unicode::string_char_type_t<SType>;
  constexpr encoding input_encoding = mts::unicode::char_type_to_encoding_v<input_char_type>;

  std::basic_string_view<input_char_type> input_view(str);

  if constexpr (input_encoding == encoding::utf8) {
    return u8_length(input_view.data(), input_view.size());
  }
  else if constexpr (input_encoding == encoding::utf16) {
    return u16_length(input_view.data(), input_view.size());
  }
  else if constexpr (input_encoding == encoding::utf32) {
    return u32_length(input_view.data(), input_view.size());
  }
  else {
    return 0;
  }
}

namespace detail {
template <encoding Encoding = encoding::utf8>
struct iterator_sequence_length {
  static inline constexpr std::size_t length(std::uint8_t lead) noexcept { return sequence_length(lead); }
};

template <>
struct iterator_sequence_length<encoding::utf16> {
  static inline constexpr std::size_t length(std::int16_t c) noexcept { return u16_sequence_length((char16_t)c); }
};

template <>
struct iterator_sequence_length<encoding::utf32> {
  static inline constexpr std::size_t length(std::int32_t) noexcept { return 1; }
};

template <typename InputCharT, typename OutputChart, typename = void>
struct base_iterator {};

template <typename InputCharT, typename OutputCharT>
struct base_iterator<InputCharT, OutputCharT,
    std::enable_if_t<char_type_to_encoding_v<InputCharT> == char_type_to_encoding_v<OutputCharT>>> {

  using input_char_type = InputCharT;
  using output_char_type = OutputCharT;

  using input_view_type = std::basic_string_view<input_char_type>;
  using output_view_type = std::basic_string_view<output_char_type>;

  using it_seq_length = detail::iterator_sequence_length<char_type_to_encoding_v<InputCharT>>;

  template <typename Iterator>
  static inline output_view_type get(Iterator it) {
    return output_view_type((const output_char_type*)&(it[0]), it_seq_length::length(*it));
  }

  template <typename Iterator>
  static inline void advance(Iterator& it) {
    std::advance(it, it_seq_length::length(*it));
  }
};

template <typename InputCharT, typename OutputCharT>
struct base_iterator<InputCharT, OutputCharT,
    std::enable_if_t<char_type_to_encoding_v<InputCharT> != char_type_to_encoding_v<OutputCharT>>> {

  using input_char_type = InputCharT;
  using output_char_type = OutputCharT;

  using input_view_type = std::basic_string_view<input_char_type>;
  using output_view_type = std::basic_string_view<output_char_type>;

  using it_seq_length = detail::iterator_sequence_length<char_type_to_encoding_v<InputCharT>>;

  template <typename Iterator>
  inline output_view_type get(Iterator it) const {
    return output_view_type(_data.begin(),
        std::distance(
            _data.begin(), unicode::copy(input_view_type(&it[0], it_seq_length::length(*it)), _data.begin())));
  }

  template <typename Iterator>
  inline void advance(Iterator& it) {
    std::advance(it, it_seq_length::length(*it));
  }

  mutable std::array<output_char_type, encoding_to_max_char_count<char_type_to_encoding_v<OutputCharT>>::value> _data;
};
} // namespace detail.

template <class IteratorType>
class iterator;

template <class CharT, class SType, class IteratorType, detail::enable_if_valid_string_type<SType> = mts::enabled>
class basic_iterator : private detail::base_iterator<mts::unicode::string_char_type_t<SType>, CharT> {
  using base_type = detail::base_iterator<mts::unicode::string_char_type_t<SType>, CharT>;
  using output_view_type = typename base_type::output_view_type;

  template <class T>
  friend class iterator;

public:
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;

  inline basic_iterator() = default;

  inline explicit basic_iterator(IteratorType it)
      : _it(it) {}

  inline IteratorType base() const { return _it; }

  inline output_view_type operator*() const { return base_type::get(_it); }

  inline bool operator==(const basic_iterator& rhs) const { return (_it == rhs._it); }

  inline bool operator!=(const basic_iterator& rhs) const { return !(operator==(rhs)); }

  inline basic_iterator& operator++() {
    base_type::advance(_it);
    return *this;
  }

  inline basic_iterator operator++(int) {
    basic_iterator temp = *this;
    base_type::advance(_it);
    return temp;
  }

private:
  IteratorType _it;
};

template <typename CharT, typename SType>
inline auto iterate_as(const SType& str) {
  return mts::iterator_range<basic_iterator<CharT, SType, typename SType::const_iterator>>(str);
}

template <typename SType, mts::enable_if_t<mts::is_one_of_constructible_v<SType, std::string_view>> = mts::enabled>
inline auto iterate(const SType& str) {
  return mts::iterator_range<basic_iterator<char, SType, typename SType::const_iterator>>(str);
}

template <typename SType, mts::enable_if_t<mts::is_one_of_constructible_v<SType, std::u8string_view>> = mts::enabled>
inline auto iterate(const SType& str) {
  return mts::iterator_range<basic_iterator<char8_t, SType, typename SType::const_iterator>>(str);
}

template <typename SType, mts::enable_if_t<mts::is_one_of_constructible_v<SType, std::u16string_view>> = mts::enabled>
inline auto iterate(const SType& str) {
  return mts::iterator_range<basic_iterator<char16_t, SType, typename SType::const_iterator>>(str);
}

template <typename SType, mts::enable_if_t<mts::is_one_of_constructible_v<SType, std::u32string_view>> = mts::enabled>
inline auto iterate(const SType& str) {
  return mts::iterator_range<basic_iterator<char32_t, SType, typename SType::const_iterator>>(str);
}

template <typename SType, mts::enable_if_t<mts::is_one_of_constructible_v<SType, std::wstring_view>> = mts::enabled>
inline auto iterate(const SType& str) {
  return mts::iterator_range<basic_iterator<wchar_t, SType, typename SType::const_iterator>>(str);
}

template <class IteratorType>
class iterator
    : private basic_iterator<mts::remove_cvref_t<decltype(std::declval<IteratorType>()[0])>,
          std::basic_string_view<mts::remove_cvref_t<decltype(std::declval<IteratorType>()[0])>>, IteratorType> {

  using base_type = basic_iterator<mts::remove_cvref_t<decltype(std::declval<IteratorType>()[0])>,
      std::basic_string_view<mts::remove_cvref_t<decltype(std::declval<IteratorType>()[0])>>, IteratorType>;

public:
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;

  inline iterator() = default;

  inline explicit iterator(IteratorType it)
      : _it(it) {}

  inline IteratorType base() const { return _it; }

  inline typename base_type::output_view_type operator*() const { return base_type::get(_it); }

  inline bool operator==(const iterator& rhs) const { return (_it == rhs._it); }

  inline bool operator!=(const iterator& rhs) const { return !(operator==(rhs)); }

  inline iterator& operator++() {
    base_type::advance(_it);
    return *this;
  }

  inline iterator operator++(int) {
    iterator temp = *this;
    base_type::advance(_it);
    return temp;
  }

private:
  IteratorType _it;
};

MTS_END_SUB_NAMESPACE(unicode)
