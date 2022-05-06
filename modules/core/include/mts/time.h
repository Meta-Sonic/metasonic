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
#include <chrono>
#include <cstring>
#include <ctime>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

#if __MTS_UNISTD__
  #include <sys/time.h>
#endif

MTS_BEGIN_NAMESPACE
static inline long long current_time_ms() noexcept {
#if __MTS_WINDOWS__ && !__MTS_MINGW__
  struct _timeb t;
  _ftime_s(&t);
  return ((std::int64_t)t.time) * 1000 + t.millitm;
#else
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return ((std::int64_t)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
#endif
}

/// YYYY-MM-DD
inline std::string current_date_string() {
  constexpr std::size_t date_size = std::char_traits<char>::length("YYYY-MM-DD");
  char str[date_size + 1];

  std::time_t t = std::time(nullptr);
  return std::strftime(str, sizeof(str), "%F", std::localtime(&t)) == date_size ? str : "";
}

/// YYYY-MM-DD
inline std::wstring current_date_wstring() {
  constexpr std::size_t date_size = std::char_traits<wchar_t>::length(L"YYYY-MM-DD");
  wchar_t str[date_size + 1];

  std::time_t t = std::time(nullptr);
  return std::wcsftime(str, sizeof(str), L"%F", std::localtime(&t)) == date_size ? str : std::wstring();
}

/// YYYY-MM-DD:HH:MM:SS
inline std::string current_datetime_string() {
  constexpr std::size_t str_size = std::char_traits<char>::length("YYYY-MM-DD:HH:MM:SS");
  char str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::strftime(str, sizeof(str), "%F:%T", std::localtime(&t)) == str_size ? str : "";
}

/// YYYY-MM-DD:HH:MM:SS
inline std::wstring current_datetime_wstring() {
  constexpr std::size_t str_size = std::char_traits<wchar_t>::length(L"YYYY-MM-DD:HH:MM:SS");
  wchar_t str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::wcsftime(str, sizeof(str), L"%F:%T", std::localtime(&t)) == str_size ? str : std::wstring();
}

/// YYYY-MM-DD-HH-MM-SS
inline std::string current_datetime_file_string() {
  constexpr std::size_t str_size = std::char_traits<char>::length("YYYY-MM-DD-HH-MM-SS");
  char str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::strftime(str, sizeof(str), "%F-%H-%M-%S", std::localtime(&t)) == str_size ? str : "";
}

/// YYYY-MM-DD-HH-MM-SS
inline std::wstring current_datetime_file_wstring() {
  constexpr std::size_t str_size = std::char_traits<wchar_t>::length(L"YYYY-MM-DD-HH-MM-SS");
  wchar_t str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::wcsftime(str, sizeof(str), L"%F-%H-%M-%S", std::localtime(&t)) == str_size ? str : std::wstring();
}

/// HH:MM:SS
inline std::string current_time_string() {
  constexpr std::size_t str_size = std::char_traits<char>::length("HH:MM:SS");
  char str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::strftime(str, sizeof(str), "%T", std::localtime(&t)) == str_size ? str : "";
}

/// HH:MM:SS
inline std::wstring current_time_wstring() {
  constexpr std::size_t str_size = std::char_traits<wchar_t>::length(L"HH:MM:SS");
  wchar_t str[str_size + 1];

  std::time_t t = std::time(nullptr);
  return std::wcsftime(str, sizeof(str), L"%T", std::localtime(&t)) == str_size ? str : std::wstring();
}

/// HH:MM:SS:MMM
inline std::string current_time_ms_string() {
  // https://stackoverflow.com/a/35157784
  constexpr std::size_t time_size = std::char_traits<char>::length("HH:MM:SS");
  constexpr std::size_t ms_size = std::char_traits<char>::length(":MMM");
  char str[time_size + ms_size + 1];

  // Current time.
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  if (std::strftime(str, sizeof(str), "%T", std::localtime(&t)) != time_size) {
    return std::string();
  }

  // Get number of milliseconds for the current second (remainder after division
  // into seconds).
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // If a call to sprintf or snprintf causes copying to take place between
  // objects that overlap, the behavior is undefined (e.g. sprintf(buf, "%s
  // text", buf);).
  return std::snprintf(str + time_size, ms_size + 1, ":%03d", (int)ms.count()) == ms_size ? str : std::string();
}

/// HH:MM:SS:MMM
inline std::wstring current_time_ms_wstring() {
  // https://stackoverflow.com/a/35157784
  constexpr std::size_t time_size = std::char_traits<wchar_t>::length(L"HH:MM:SS");
  constexpr std::size_t ms_size = std::char_traits<wchar_t>::length(L":MMM");
  wchar_t str[time_size + ms_size + 1];

  // Current time.
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  if (std::wcsftime(str, sizeof(str), L"%T", std::localtime(&t)) != time_size) {
    return std::wstring();
  }

  // Get number of milliseconds for the current second (remainder after division
  // into seconds).
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // If a call to sprintf or snprintf causes copying to take place between
  // objects that overlap, the behavior is undefined (e.g. sprintf(buf, "%s
  // text", buf);).
  return std::swprintf(str + time_size, ms_size + 1, L":%03d", (int)ms.count()) == ms_size ? str : std::wstring();
}

/// Current date and time expressed according to ISO 8601.
/// YYYY-MM-DDTHH:MM:SSZ
inline std::string current_datetime_utc_string() {
  constexpr std::size_t time_size = std::char_traits<char>::length("YYYY-MM-DDTHH:MM:SSZ");
  char str[time_size + 1];

  // Current date/time based on current system
  std::time_t now = std::time(nullptr);

  // Convert now to tm struct for UTC.
  std::tm* ltm = std::gmtime(&now);

  return std::snprintf(str, time_size + 1, "%04d-%02d-%02dT%02d:%02d:%02dZ", 1900 + ltm->tm_year, 1 + ltm->tm_mon,
             ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec)
          == time_size
      ? str
      : std::string();
}

/// Current date and time expressed according to ISO 8601.
/// YYYY-MM-DDTHH:MM:SSZ
inline std::wstring current_datetime_utc_wstring() {
  constexpr std::size_t time_size = std::char_traits<wchar_t>::length(L"YYYY-MM-DDTHH:MM:SSZ");
  wchar_t str[time_size + 1];

  // Current date/time based on current system
  std::time_t now = std::time(nullptr);

  // Convert now to tm struct for UTC.
  std::tm* ltm = std::gmtime(&now);

  return std::swprintf(str, time_size + 1, L"%04d-%02d-%02dT%02d:%02d:%02dZ", 1900 + ltm->tm_year, 1 + ltm->tm_mon,
             ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec)
          == time_size
      ? str
      : std::wstring();
}
MTS_END_NAMESPACE
