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
#include "mts/traits.h"
#include "mts/audio/buffer.h"
#include <array>

MTS_BEGIN_NAMESPACE

inline constexpr std::size_t audio_bus_dynamic_extent = std::numeric_limits<std::size_t>::max();

template <typename T, std::size_t Size = audio_bus_dynamic_extent>
class audio_bus {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using channel_pointer = element_type*;
  using channel_const_pointer = const element_type*;
  using buffer_pointer = element_type* const*;
  using buffer_const_pointer = const element_type* const*;

  static_assert(std::is_floating_point_v<value_type>, "mts::audio::bus only works with floating point value type.");

private:
  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

public:
  audio_bus() noexcept { _buffers.fill(nullptr); }

  audio_bus(const audio_bus&) noexcept = default;
  audio_bus(audio_bus&&) noexcept = default;

  audio_bus(buffer_pointer __buffers, size_type __buffer_size) noexcept
      : _buffer_size(__buffer_size) {
    for (std::size_t i = 0; i < Size; i++) {
      _buffers[i] = __buffers[i];
    }
  }

  template <class U, std::size_t OtherSize>
  inline constexpr audio_bus(audio_bus<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept {
    static_assert(OtherSize >= Size, "Invalid channel size");

    for (std::size_t i = 0; i < Size; i++) {
      _buffers[i] = b[i];
    }
    _buffer_size = b.buffer_size();
  }

  template <class U, std::size_t OtherSize>
  inline constexpr audio_bus(const audio_bus<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept {
    static_assert(OtherSize >= Size, "Invalid channel size");

    for (std::size_t i = 0; i < Size; i++) {
      _buffers[i] = b[i];
    }
    _buffer_size = b.buffer_size();
  }

  template <class U>
  inline constexpr audio_bus(audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept {
    mts_assert(b.channel_size() >= Size, "Invalid channel size");

    if (b.channel_size() < Size) {
      _buffers.fill(nullptr);
      _buffer_size = 0;
      return;
    }

    _buffer_size = b.buffer_size();

    for (std::size_t i = 0; i < Size; i++) {
      _buffers[i] = b[i];
    }
  }

  template <class U>
  inline constexpr audio_bus(const audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept {
    mts_assert(b.channel_size() >= Size, "Invalid channel size");

    if (b.channel_size() < Size) {
      _buffers.fill(nullptr);
      _buffer_size = 0;
      return;
    }

    _buffer_size = b.buffer_size();

    for (std::size_t i = 0; i < Size; i++) {
      _buffers[i] = b[i];
    }
  }

  ~audio_bus() noexcept = default;

  audio_bus& operator=(const audio_bus&) noexcept = default;
  audio_bus& operator=(audio_bus&&) noexcept = default;

  inline bool is_valid() const noexcept { return (bool)_buffers[0]; }

  inline size_type channel_size() const noexcept { return Size; }
  inline size_type buffer_size() const noexcept { return _buffer_size; }

  inline buffer_pointer data() noexcept { return _buffers.data(); }
  inline buffer_const_pointer data() const noexcept { return _buffers.data(); }

  inline channel_pointer channel(size_type index) noexcept { return _buffers[index]; }
  inline channel_const_pointer channel(size_type index) const noexcept { return _buffers[index]; }

  inline channel_pointer operator[](size_type index) noexcept { return _buffers[index]; }
  inline channel_const_pointer operator[](size_type index) const noexcept { return _buffers[index]; }

private:
  template <class, std::size_t>
  friend class bus;

  std::array<channel_pointer, Size> _buffers;
  size_type _buffer_size = 0;
};

template <typename T>
class audio_bus<T, audio_bus_dynamic_extent> {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using channel_pointer = element_type*;
  using channel_const_pointer = const element_type*;
  using buffer_pointer = element_type* const*;
  using buffer_const_pointer = const element_type* const*;

  static_assert(std::is_floating_point_v<value_type>, "mts::audio::bus only works with floating point value type.");

private:
  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

public:
  audio_bus() noexcept = default;
  audio_bus(const audio_bus&) noexcept = default;
  audio_bus(audio_bus&&) noexcept = default;

  audio_bus(buffer_pointer __buffers, size_type __buffer_size, size_type __channel_size) noexcept
      : _buffers(__buffers)
      , _buffer_size(__buffer_size)
      , _channel_size(__channel_size) {}

  template <class U>
  inline constexpr audio_bus(const audio_bus<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffers{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  template <class U>
  inline constexpr audio_bus(audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffers{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  template <class U>
  inline constexpr audio_bus(const audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffers{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  ~audio_bus() noexcept = default;

  audio_bus& operator=(const audio_bus&) noexcept = default;
  audio_bus& operator=(audio_bus&&) noexcept = default;

  inline bool is_valid() const noexcept { return (bool)_buffers; }

  inline size_type channel_size() const noexcept { return _channel_size; }
  inline size_type buffer_size() const noexcept { return _buffer_size; }

  inline buffer_pointer data() noexcept { return _buffers; }
  inline buffer_const_pointer data() const noexcept { return _buffers; }

  inline channel_pointer channel(size_type index) noexcept { return _buffers[index]; }
  inline channel_const_pointer channel(size_type index) const noexcept { return _buffers[index]; }

  inline channel_pointer operator[](size_type index) noexcept { return _buffers[index]; }
  inline channel_const_pointer operator[](size_type index) const noexcept { return _buffers[index]; }

private:
  buffer_pointer _buffers = nullptr;
  size_type _buffer_size = 0;
  size_type _channel_size = 0;
};

template <typename T>
class interleaved_audio_bus {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using reference = element_type&;
  using const_reference = const element_type&;
  using pointer = element_type*;
  using const_pointer = const element_type*;
  using buffer_pointer = element_type* const*;
  using buffer_const_pointer = const element_type* const*;

  static_assert(std::is_floating_point_v<value_type>,
      "mts::audio::interleaved_audio_bus only works with floating point value type.");

private:
  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

public:
  interleaved_audio_bus() noexcept = default;
  interleaved_audio_bus(const interleaved_audio_bus&) noexcept = default;
  interleaved_audio_bus(interleaved_audio_bus&&) noexcept = default;

  interleaved_audio_bus(pointer __buffer, size_type __buffer_size, size_type __channel_size) noexcept
      : _buffer(__buffer)
      , _buffer_size(__buffer_size)
      , _channel_size(__channel_size) {}

  template <class U>
  inline constexpr interleaved_audio_bus(
      const interleaved_audio_bus<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  template <class U>
  inline constexpr interleaved_audio_bus(interleaved_audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  template <class U>
  inline constexpr interleaved_audio_bus(
      const interleaved_audio_buffer<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer{ b.data() }
      , _buffer_size(b.buffer_size())
      , _channel_size(b.channel_size()) {}

  ~interleaved_audio_bus() noexcept = default;

  interleaved_audio_bus& operator=(const interleaved_audio_bus&) noexcept = default;
  interleaved_audio_bus& operator=(interleaved_audio_bus&&) noexcept = default;

  inline bool is_valid() const noexcept { return (bool)_buffer; }

  inline size_type channel_size() const noexcept { return _channel_size; }
  inline size_type buffer_size() const noexcept { return _buffer_size; }

  inline pointer data() noexcept { return _buffer; }
  inline const_pointer data() const noexcept { return _buffer; }

  inline reference operator[](size_type index) noexcept { return data()[index]; }
  inline const_reference operator[](size_type index) const noexcept { return data()[index]; }

private:
  pointer _buffer = nullptr;
  size_type _buffer_size = 0;
  size_type _channel_size = 0;
};
MTS_END_NAMESPACE

MTS_BEGIN_SUB_NAMESPACE(audio)
template <typename T>
using bus = audio_bus<T>;

template <typename T>
using interleaved_bus = interleaved_audio_bus<T>;
MTS_END_SUB_NAMESPACE(audio)
