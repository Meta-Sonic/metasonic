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
#include "mts/pcm.h"
#include "mts/detail/iterator.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

MTS_PUSH_MACROS
#include "mts/detail/undef_macros.h"

MTS_BEGIN_NAMESPACE

template <typename _Tp>
class memory_range;

namespace detail {
template <class _Tp>
struct is_memory_range_impl : public std::false_type {};

template <class _Tp>
struct is_memory_range_impl<memory_range<_Tp>> : public std::true_type {};
} // namespace detail.

template <class _Tp>
struct is_memory_range : public detail::is_memory_range_impl<std::remove_cv_t<_Tp>> {};

namespace detail {
template <class _Tp, class _ElementType, class = void>
struct is_memory_range_compatible_container : public std::false_type {};

template <class _Tp, class _ElementType>
struct is_memory_range_compatible_container<_Tp, _ElementType,
    std::void_t<
        // is not a specialization of memory_range
        std::enable_if_t<!_VMTS::is_memory_range<_Tp>::value, std::nullptr_t>,
        // is not a specialization of array
        std::enable_if_t<!_VMTS::is_std_array<_Tp>::value, std::nullptr_t>,
        // is_array_v<Container> is false,
        std::enable_if_t<!std::is_array_v<_Tp>, std::nullptr_t>,
        // data(cont) and size(cont) are well formed
        decltype(std::data(std::declval<_Tp>())), decltype(std::size(std::declval<_Tp>())),
        // remove_pointer_t<decltype(data(cont))>(*)[] is convertible to ElementType(*)[]
        std::enable_if_t<std::is_convertible_v<_VMTS::container_data_type_t<_Tp> (*)[], _ElementType (*)[]>,
            std::nullptr_t>>> : public std::true_type {};

template <class _Tp, class _ElementType>
inline constexpr bool is_memory_range_compatible_container_v
    = is_memory_range_compatible_container<_Tp, _ElementType>::value;

template <class _Tp, class = void>
struct is_byte_view_compatible_container : public std::false_type {};

template <class _Tp>
struct is_byte_view_compatible_container<_Tp,
    std::void_t<
        // is not a specialization of memory_range
        std::enable_if_t<!_VMTS::is_memory_range<_Tp>::value, std::nullptr_t>,
        // is not a specialization of array
        std::enable_if_t<!_VMTS::is_std_array<_Tp>::value, std::nullptr_t>,
        // is_array_v<Container> is false,
        std::enable_if_t<!std::is_array_v<_Tp>, std::nullptr_t>,
        // data(cont) and size(cont) are well formed
        decltype(std::data(std::declval<_Tp>())), decltype(std::size(std::declval<_Tp>())),
        std::enable_if_t<std::is_trivially_copyable_v<_VMTS::container_data_type_t<_Tp>>, std::nullptr_t>>>
    : public std::true_type {};

template <class _Tp>
inline constexpr bool is_byte_view_compatible_container_v = is_byte_view_compatible_container<_Tp>::value;

} // namespace detail.

class byte_view;

/// @class memory_range
template <class _Tp>
class memory_range {
public:
  using __self = memory_range;
  using element_type = _Tp;
  using value_type = std::remove_cv_t<_Tp>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using pointer = element_type*;
  using const_pointer = const element_type*;
  using reference = element_type&;
  using const_reference = const element_type&;

  using iterator = _VMTS::__wrap_iterator<pointer>;
  using const_iterator = _VMTS::__wrap_iterator<const_pointer>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

  template <class Container>
  using enable_if_compatible_t
      = std::enable_if_t<detail::is_memory_range_compatible_container<Container, _Tp>::value, std::nullptr_t>;

public:
  inline constexpr memory_range() noexcept
      : __data{ nullptr }
      , __size{ 0 } {}

  constexpr memory_range(const memory_range&) noexcept = default;
  constexpr memory_range& operator=(const memory_range&) noexcept = default;

  inline constexpr memory_range(pointer ptr, size_type count)
      : __data{ ptr }
      , __size{ count } {}

  inline constexpr memory_range(pointer first, pointer last)
      : __data{ first }
      , __size{ static_cast<std::size_t>(std::distance(first, last)) } {}

  template <std::size_t Size>
  inline constexpr memory_range(element_type (&__arr)[Size]) noexcept
      : __data{ __arr }
      , __size{ Size } {}

  template <class U, std::size_t Size, enable_if_convertible_t<U> = nullptr>
  inline constexpr memory_range(std::array<U, Size>& arr) noexcept
      : __data{ arr.data() }
      , __size{ Size } {}

  template <class U, std::size_t Size, enable_if_convertible_t<const U> = nullptr>
  inline constexpr memory_range(const std::array<U, Size>& arr) noexcept
      : __data{ arr.data() }
      , __size{ Size } {}

  template <class Container>
  inline constexpr memory_range(Container& c, enable_if_compatible_t<Container> = nullptr)
      : __data{ std::data(c) }
      , __size{ (size_type)std::size(c) } {}

  template <class Container>
  inline constexpr memory_range(const Container& c, enable_if_compatible_t<const Container> = nullptr)
      : __data{ std::data(c) }
      , __size{ (size_type)std::size(c) } {}

  template <class U>
  inline constexpr memory_range(const memory_range<U>& r, enable_if_convertible_t<U> = nullptr) noexcept
      : __data{ r.data() }
      , __size{ r.size() } {}

  ~memory_range() noexcept = default;

  inline constexpr memory_range sub_range(
      size_type __offset, size_type __count = std::numeric_limits<size_type>::max()) const noexcept {
    mts_assert(__offset <= size(), "Offset out of range in mts::memory_range::subrange(offset, count)");
    //    mts_assert(__count <= size(), "count out of range in mts::memory_range::subrange(offset, count)");
    //    mts_assert(
    //        __count <= size() - __offset, "Offset + count out of range in mts::memory_range::subrange(offset,
    //        count)");

    __count = std::min(size() - __offset, __count);
    return { data() + __offset, __count };
  }

  inline constexpr memory_range sub_first(size_type __count) const noexcept {
    mts_assert(__count <= size(), "Count out of range in mts::memory_range::sub_first(count)");
    return { data(), __count };
  }

  inline constexpr memory_range sub_last(size_type __count) const noexcept {
    mts_assert(__count <= size(), "Count out of range in mts::memory_range::sub_last(count)");
    return { data() + size() - __count, __count };
  }

  inline constexpr size_type size() const noexcept { return __size; }
  inline constexpr size_type size_bytes() const noexcept { return __size * sizeof(element_type); }
  inline constexpr bool empty() const noexcept { return __size == 0; }

  inline constexpr reference operator[](size_type __idx) const noexcept {
    mts_assert(__idx < size(), "mts::memory_range::operator[] index out of bounds");
    return __data[__idx];
  }

  inline constexpr reference front() const noexcept {
    mts_assert(!empty(), "mts::memory_range::front() on empty range");
    return __data[0];
  }

  inline constexpr reference back() const noexcept {
    mts_assert(!empty(), "mts::memory_range::back() on empty range");
    return __data[size() - 1];
  }

  inline constexpr pointer data() const noexcept { return __data; }

  inline constexpr iterator begin() noexcept { return iterator(data()); }
  inline constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
  inline constexpr iterator end() noexcept { return iterator(data() + size()); }
  inline constexpr const_iterator end() const noexcept { return const_iterator(data() + size()); }

  inline constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  inline constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  inline constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  inline constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  inline constexpr const_iterator cbegin() const noexcept { return begin(); }
  inline constexpr const_iterator cend() const noexcept { return end(); }
  inline constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  inline constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  inline byte_view as_byte_view() const noexcept;

  inline memory_range<std::uint8_t> as_byte_range() const noexcept {
    return { reinterpret_cast<std::uint8_t*>(data()), size_bytes() };
  }

private:
  pointer __data;
  size_type __size;
};

/// @class byte_view
///
class byte_view : public memory_range<const std::uint8_t> {
  template <typename T>
  using enable_if_triv_copyable_t = std::enable_if_t<std::is_trivially_copyable_v<T>, std::nullptr_t>;

  template <typename T>
  using enable_if_float_t = std::enable_if_t<std::is_floating_point_v<T>, std::nullptr_t>;

public:
  using range_type = memory_range<const std::uint8_t>;
  using range_type::range_type;

  template <typename T>
  inline constexpr byte_view(const T* ptr, size_type count)
      : byte_view(memory_range<const T>(ptr, count).as_byte_view()) {}

  template <typename T>
  inline constexpr byte_view(const T* __first, const T* __last)
      : byte_view(memory_range<const T>(__first, __last).as_byte_view()) {}

  template <typename T, std::size_t Size>
  inline constexpr byte_view(const T (&arr)[Size]) noexcept
      : byte_view(memory_range<const T>(arr).as_byte_view()) {}

  template <typename T, std::size_t Size>
  inline constexpr byte_view(const std::array<T, Size>& arr) noexcept
      : byte_view(memory_range<const T>(arr).as_byte_view()) {}

  template <class Container, mts::enable_if_t<detail::is_byte_view_compatible_container_v<Container>> = mts::enabled>
  inline constexpr byte_view(const Container& c)
      : byte_view(memory_range<_VMTS::container_data_type_t<const Container>>(c.data(), c.size()).as_byte_view()) {}

  template <class T>
  inline constexpr byte_view(const memory_range<T>& r) noexcept
      : byte_view(r.as_byte_view()) {}

  template <typename T = std::uint8_t>
  inline constexpr const _VMTS::remove_cvref_t<T>* data() const noexcept {
    return std::launder(reinterpret_cast<const _VMTS::remove_cvref_t<T>*>(range_type::data()));
  }

  template <typename T = std::uint8_t>
  inline constexpr const _VMTS::remove_cvref_t<T>* data(size_type __index) const noexcept {
    return std::launder(reinterpret_cast<const _VMTS::remove_cvref_t<T>*>(range_type::data() + __index));
  }

  //
  // MARK: Find.
  //
  template <class T>
  inline difference_type find(const T* data, size_type size) const noexcept {
    iterator it = std::search(begin(), end(), data, data + size);
    if (it == end()) {
      return -1;
    }

    return std::distance(begin(), it);
  }

  template <class T>
  inline difference_type find(size_type offset, const T* data, size_type size) const noexcept {
    iterator it = std::search(begin() + offset, end(), data, data + size);
    if (it == end()) {
      return -1;
    }

    return std::distance(begin(), it);
  }

  //
  // MARK: Convertions.
  //

  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline const T& as_ref(size_type __index) const noexcept {
    static_assert(_IsLittleEndian, "memory_range::as_ref is not supported for big endian.");
    return *data<T>(__index);
  }

  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline T as(size_type __index) const noexcept {

    if constexpr (_IsLittleEndian) {
      if constexpr (sizeof(T) <= 8) {
        T value;
        value_type* value_data = reinterpret_cast<value_type*>(&value);
        for (size_type i = __index, j = 0; i < __index + sizeof(T); i++, j++) {
          value_data[j] = range_type::data()[i];
        }
        return value;
      }
      else {
        T value;
        value_type* value_data = reinterpret_cast<value_type*>(&value);
        std::memmove(value_data, data<T>(__index), sizeof(T));
        return value;
      }
    }
    else {
      T value;
      value_type* value_data = reinterpret_cast<value_type*>(&value);
      for (size_type i = __index, j = sizeof(T) - 1; i < __index + sizeof(T); i++, j--) {
        value_data[j] = range_type::data()[i];
      }
      return value;
    }
  }

  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline T as(iterator pos) const noexcept {
    difference_type index = std::distance(pos, begin());
    mts_assert(index >= 0, "Wrong iterator position.");
    return as<T, _IsLittleEndian>((size_type)index);
  }

  // Get array element at array_index from array starting at index.
  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline T as(size_type index, size_type array_index) const noexcept {
    return as<T, _IsLittleEndian>(index + array_index * sizeof(T));
  }

  // Get array element at array_index from array starting at pos.
  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline T as(iterator pos, size_type array_index) const noexcept {
    difference_type index = std::distance(pos, begin());
    mts_assert(index >= 0, "Wrong iterator position.");
    return as<T, _IsLittleEndian>((size_type)index, array_index);
  }

  template <typename T, bool _IsLittleEndian = true, enable_if_triv_copyable_t<T> = nullptr>
  inline void copy_as(T* buffer, size_type index, size_type array_size) const noexcept {

    if constexpr (_IsLittleEndian) {
      std::memmove(buffer, data<T>(index), array_size * sizeof(T));
    }
    else {
      for (size_type i = 0; i < array_size; i++) {
        buffer[i] = as<T, _IsLittleEndian>(index, i);
      }
    }
  }

  template <typename T, pcm::type c_opts, enable_if_float_t<T> = nullptr>
  inline T as_from(size_type __index) const noexcept {
    return pcm::convert<T, c_opts>(range_type::data() + __index);
  }
};

template <typename T>
inline byte_view memory_range<T>::as_byte_view() const noexcept {
  return { reinterpret_cast<const std::uint8_t*>(data()), size_bytes() };
}

using byte_range = memory_range<std::uint8_t>;
MTS_END_NAMESPACE
MTS_POP_MACROS
