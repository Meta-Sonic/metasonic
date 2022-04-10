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
#include "mts/assert.h"
#include "mts/error.h"
#include "mts/exception.h"
#include "mts/filesystem.h"
#include "mts/memory_range.h"

#include <cstddef>
#include <string_view>

MTS_BEGIN_NAMESPACE
class file_view {
public:
  using value_type = std::uint8_t;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = const_pointer;
  using const_iterator = const_pointer;
  using size_type = std::size_t;

  file_view() noexcept = default;

  file_view(const file_view&) = delete;

  inline file_view(file_view&& fb) noexcept
      : _data(fb._data)
      , _size(fb._size) {
    fb._data = nullptr;
    fb._size = 0;
  }

  inline ~file_view() { close(); }

  file_view& operator=(const file_view&) = delete;

  inline file_view& operator=(file_view&& fb) noexcept {
    _data = fb._data;
    _size = fb._size;
    fb._data = nullptr;
    fb._size = 0;
    return *this;
  }

  std::error_code open(const _VMTS::filesystem::path& file_path);

  void close();

  inline bool is_open() const noexcept { return _data && _size; }

  MTS_NODISCARD inline bool empty() const noexcept { return !is_open(); }

  inline size_type size() const noexcept { return _size; }

  inline std::string_view str() const noexcept { return std::string_view((const char*)_data, _size); }

  inline _VMTS::byte_view content() const noexcept {
    return _VMTS::byte_view((const _VMTS::byte_view::value_type*)_data, _size);
  }

  inline const_pointer data() const { return _data; }

  inline iterator begin() const noexcept { return _data; }
  inline iterator end() const noexcept { return _data + _size; }

  inline const_iterator cbegin() const noexcept { return begin(); }
  inline const_iterator cend() const noexcept { return end(); }

  inline value_type operator[](size_type __n) const noexcept {
    mts_assert(__n < size(), "index out of bounds");
    return _data[__n];
  }

  inline value_type at(size_type __n) const {
    if (__n >= size()) {
      _VMTS::throw_exception<std::out_of_range>("file_view::at");
    }

    return _data[__n];
  }

  inline value_type front() const noexcept {
    mts_assert(!empty(), "front() called when empty");
    return *_data;
  }

  inline value_type back() const noexcept {
    mts_assert(!empty(), "back() called when empty");
    return *(_data + _size - 1);
  }

private:
  struct impl;
  pointer _data = nullptr;
  size_type _size = 0;
};
MTS_END_NAMESPACE
