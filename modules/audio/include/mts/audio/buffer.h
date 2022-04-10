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
#include "mts/audio/detail/buffer_allocator.h"
#include "mts/audio/vector_operations.h"

MTS_BEGIN_NAMESPACE
template <typename T>
class audio_buffer {
public:
  using value_type = mts::remove_cvref_t<T>;
  using size_type = std::size_t;
  using channel_pointer = value_type*;
  using channel_const_pointer = const value_type*;
  using buffer_pointer = value_type* const*;
  using buffer_const_pointer = const value_type* const*;

  static_assert(std::is_floating_point_v<value_type> && std::is_same_v<mts::remove_cvref_t<T>, T>,
      "mts::audio_buffer only works with floating point value type.");

  audio_buffer() noexcept = default;
  audio_buffer(const audio_buffer& d) = delete;

  inline audio_buffer(size_type __buffer_size, size_type __channel_size)
      : audio_buffer() {

    detail::allocate_audio_buffer(_buffers, __buffer_size, __channel_size);

    if (_buffers) {
      _buffer_size = __buffer_size;
      _channel_size = __channel_size;

      clear();
    }
  }

  inline audio_buffer(audio_buffer&& d) noexcept
      : _buffers(d._buffers)
      , _buffer_size(d._buffer_size)
      , _channel_size(d._channel_size) {
    d._buffers = nullptr;
    d._buffer_size = 0;
    d._channel_size = 0;
  }

  inline ~audio_buffer() { reset(); }

  audio_buffer& operator=(const audio_buffer& d) = delete;

  inline audio_buffer& operator=(audio_buffer&& d) noexcept {
    reset();
    _buffers = d._buffers;
    _buffer_size = d._buffer_size;
    _channel_size = d._channel_size;
    d._buffers = nullptr;
    d._buffer_size = 0;
    d._channel_size = 0;
    return *this;
  }

  inline bool is_valid() const noexcept { return (bool)_buffers; }

  inline size_type channel_size() const noexcept { return _channel_size; }
  inline size_type buffer_size() const noexcept { return _buffer_size; }

  inline buffer_pointer data() noexcept { return _buffers; }
  inline buffer_const_pointer data() const noexcept { return _buffers; }

  inline channel_pointer channel(size_type index) noexcept {
    mts_assert(index < _channel_size, "Out of bounds index");
    return _buffers[index];
  }

  inline channel_const_pointer channel(size_type index) const noexcept {
    mts_assert(index < _channel_size, "Out of bounds index");
    return _buffers[index];
  }

  inline channel_pointer operator[](size_type index) noexcept {
    mts_assert(index < _channel_size, "Out of bounds index");
    return _buffers[index];
  }

  inline channel_const_pointer operator[](size_type index) const noexcept {
    mts_assert(index < _channel_size, "Out of bounds index");
    return _buffers[index];
  }

  inline void clear() {
    if (_buffers) {
      for (std::size_t i = 0; i < _channel_size; i++) {
        mts::vec::clear(channel(i), 1, _buffer_size);
      }
    }
  }

  inline void reset() {
    if (_buffers) {
      detail::deallocate_audio_buffer(_buffers);
      _buffers = nullptr;
    }

    _buffer_size = 0;
    _channel_size = 0;
  }

  inline void reset(size_type __buffer_size, size_type __channel_size) {

    if (_buffers && _buffer_size == __buffer_size && _channel_size == __channel_size) {
      clear();
      return;
    }

    reset();

    detail::allocate_audio_buffer(_buffers, __buffer_size, __channel_size);

    if (_buffers) {
      _buffer_size = __buffer_size;
      _channel_size = __channel_size;
    }
  }

private:
  buffer_pointer _buffers = nullptr;
  size_type _buffer_size = 0;
  size_type _channel_size = 0;
};

template <typename T>
class interleaved_audio_buffer {
public:
  using value_type = mts::remove_cvref_t<T>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using buffer_pointer = value_type* const*;
  using buffer_const_pointer = const value_type* const*;

  static_assert(std::is_floating_point_v<value_type> && std::is_same_v<mts::remove_cvref_t<T>, T>,
      "mts::interleaved_audio_buffer only works with floating point value type.");

  interleaved_audio_buffer() noexcept = default;
  interleaved_audio_buffer(const interleaved_audio_buffer& d) = delete;

  inline interleaved_audio_buffer(size_type __buffer_size, size_type __channel_size)
      : interleaved_audio_buffer() {

    detail::allocate_interleaved_audio_buffer(_buffers, __buffer_size, __channel_size);

    if (_buffers) {
      _buffer_size = __buffer_size;
      _channel_size = __channel_size;

      clear();
    }
  }

  inline interleaved_audio_buffer(interleaved_audio_buffer&& d) noexcept
      : _buffers(d._buffers)
      , _buffer_size(d._buffer_size)
      , _channel_size(d._channel_size) {
    d._buffers = nullptr;
    d._buffer_size = 0;
    d._channel_size = 0;
  }

  inline ~interleaved_audio_buffer() { reset(); }

  interleaved_audio_buffer& operator=(const interleaved_audio_buffer& d) = delete;

  inline interleaved_audio_buffer& operator=(interleaved_audio_buffer&& d) noexcept {
    reset();
    _buffers = d._buffers;
    _buffer_size = d._buffer_size;
    _channel_size = d._channel_size;
    d._buffers = nullptr;
    d._buffer_size = 0;
    d._channel_size = 0;
    return *this;
  }

  inline bool is_valid() const noexcept { return (bool)_buffers; }

  inline size_type channel_size() const noexcept { return _channel_size; }
  inline size_type buffer_size() const noexcept { return _buffer_size; }

  inline pointer data() noexcept { return *_buffers; }
  inline const_pointer data() const noexcept { return *_buffers; }

  inline reference operator[](size_type index) noexcept {
    mts_assert(index < _buffer_size * _channel_size, "Out of bounds index");
    return data()[index];
  }

  inline const_reference operator[](size_type index) const noexcept {
    mts_assert(index < _buffer_size * _channel_size, "Out of bounds index");
    return data()[index];
  }

  inline void clear() {
    if (_buffers) {
      mts::vec::clear(data(), 1, _channel_size * _buffer_size);
    }
  }

  inline void reset() {
    if (_buffers) {
      detail::deallocate_audio_buffer(_buffers);
      _buffers = nullptr;
    }

    _buffer_size = 0;
    _channel_size = 0;
  }

  inline void reset(size_type __buffer_size, size_type __channel_size) {

    if (_buffers && _buffer_size == __buffer_size && _channel_size == __channel_size) {
      clear();
      return;
    }

    reset();

    detail::allocate_interleaved_audio_buffer(_buffers, __buffer_size, __channel_size);

    if (_buffers) {
      _buffer_size = __buffer_size;
      _channel_size = __channel_size;
    }
  }

private:
  buffer_pointer _buffers = nullptr;
  size_type _buffer_size = 0;
  size_type _channel_size = 0;
};
MTS_END_NAMESPACE

MTS_BEGIN_SUB_NAMESPACE(audio)
template <typename T>
using buffer = audio_buffer<T>;

template <typename T>
using interleaved_buffer = interleaved_audio_buffer<T>;
MTS_END_SUB_NAMESPACE(audio)
