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
#include "mts/util.h"
#include "mts/detail/iterator.h"
#include "mts/audio/vector_operations.h"
#include <array>

MTS_BEGIN_NAMESPACE

inline constexpr std::size_t wire_dynamic_extent = std::numeric_limits<std::size_t>::max();

namespace detail {
template <typename T, std::size_t Size = wire_dynamic_extent>
class wire_base;

template <class _Tp>
struct is_wire_base_impl : public std::false_type {};

template <class _Tp, std::size_t _Extent>
struct is_wire_base_impl<wire_base<_Tp, _Extent>> : public std::true_type {};

//} // namespace detail.

template <class _Tp>
struct is_wire_base : public detail::is_wire_base_impl<std::remove_cv_t<_Tp>> {};

// namespace detail {
template <class _Tp, class _ElementType, class = void>
struct is_wire_base_compatible_container : public std::false_type {};

template <bool _Bp>
using enble_if_t = std::enable_if_t<_Bp, std::nullptr_t>;

template <class _Tp, class _ElementType>
struct is_wire_base_compatible_container<_Tp, _ElementType,
    std::void_t< //
        enble_if_t<!is_wire_base<_Tp>::value>, //
        enble_if_t<!mts::is_std_array_v<_Tp>>,
        enble_if_t<!std::is_array_v<_Tp>>, //
        decltype(std::data(std::declval<_Tp>())), //
        decltype(std::size(std::declval<_Tp>())), //
        enble_if_t<std::is_convertible_v<mts::container_data_type_t<_Tp> (*)[], _ElementType (*)[]>> //
        >> : public std::true_type {};

template <class _Tp, class _ElementType>
inline constexpr bool is_wire_base_compatible_container_v = is_wire_base_compatible_container<_Tp, _ElementType>::value;

template <typename T, std::size_t Size>
class wire_base {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using reference = element_type&;
  using const_reference = const element_type&;
  using pointer = element_type*;
  using const_pointer = const element_type*;
  using iterator = mts::__wrap_iterator<pointer>;
  using const_iterator = mts::__wrap_iterator<const_pointer>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr std::size_t extent = Size;

  static_assert(
      std::is_floating_point_v<value_type>, "mts::audio::wire_base only works with floating point value type.");

private:
  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

  template <class Container>
  using enable_if_compatible_t
      = std::enable_if_t<detail::is_wire_base_compatible_container<Container, T>::value, std::nullptr_t>;

public:
  wire_base() noexcept = default;
  wire_base(const wire_base&) noexcept = default;
  wire_base(wire_base&&) noexcept = default;

  wire_base(pointer __buffer) noexcept
      : _buffer(__buffer) {}

  template <class U, std::size_t OtherSize>
  inline constexpr wire_base(wire_base<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data()) {
    static_assert(OtherSize >= Size, "Invalid size");
  }

  template <class U, std::size_t OtherSize>
  inline constexpr wire_base(const wire_base<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data()) {
    static_assert(OtherSize >= Size, "Invalid size");
  }

  template <class U>
  inline constexpr wire_base(wire_base<U, wire_dynamic_extent>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data()) {
    mts_assert(b.size() >= Size, "Invalid size");
  }

  template <class U>
  inline constexpr wire_base(const wire_base<U, wire_dynamic_extent>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data()) {
    mts_assert(b.size() >= Size, "Wrong size");
  }

  template <class U>
  inline constexpr wire_base(wire_base<U, wire_dynamic_extent>&& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data()) {
    mts_assert(b.size() >= Size, "Wrong size");
  }

  inline constexpr wire_base(element_type (&__arr)[Size]) noexcept
      : _buffer{ __arr } {}

  template <class U, enable_if_convertible_t<U> = nullptr>
  inline constexpr wire_base(std::array<U, Size>& arr) noexcept
      : _buffer{ arr.data() } {}

  template <class U, enable_if_convertible_t<const U> = nullptr>
  inline constexpr wire_base(const std::array<U, Size>& arr) noexcept
      : _buffer{ arr.data() } {}

  template <class Container>
  inline constexpr wire_base(Container& c, enable_if_compatible_t<Container> = nullptr)
      : _buffer{ std::data(c) } {
    mts_assert(std::size(c) >= Size, "Wrong size");
  }

  template <class Container>
  inline constexpr wire_base(const Container& c, enable_if_compatible_t<const Container> = nullptr)
      : _buffer{ std::data(c) } {
    mts_assert(std::size(c) >= Size, "Wrong size");
  }

  ~wire_base() noexcept = default;

  wire_base& operator=(const wire_base&) noexcept = default;
  wire_base& operator=(wire_base&&) noexcept = default;

  inline bool is_valid() const noexcept { return (bool)_buffer; }

  MTS_NODISCARD inline size_type size() const noexcept { return Size; }
  MTS_NODISCARD inline bool empty() const noexcept { return Size == 0; }

  inline pointer data() noexcept { return _buffer; }
  inline const_pointer data() const noexcept { return _buffer; }

  inline reference operator[](size_type index) noexcept {
    mts_assert(index < Size, "Out of bounds index");
    return _buffer[index];
  }

  inline const_reference operator[](size_type index) const noexcept {
    mts_assert(index < Size, "Out of bounds index");
    return _buffer[index];
  }

  inline iterator begin() noexcept { return iterator(data()); }
  inline const_iterator begin() const noexcept { return const_iterator(data()); }
  inline iterator end() noexcept { return iterator(data() + size()); }
  inline const_iterator end() const noexcept { return const_iterator(data() + size()); }

  inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  inline const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  inline const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  inline const_iterator cbegin() const noexcept { return begin(); }
  inline const_iterator cend() const noexcept { return end(); }
  inline const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  inline const_reverse_iterator crend() const noexcept { return rend(); }

private:
  pointer _buffer;
};

template <typename T>
class wire_base<T, wire_dynamic_extent> {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using reference = element_type&;
  using const_reference = const element_type&;
  using pointer = element_type*;
  using const_pointer = const element_type*;
  using iterator = mts::__wrap_iterator<pointer>;
  using const_iterator = mts::__wrap_iterator<const_pointer>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr std::size_t extent = wire_dynamic_extent;

  static_assert(
      std::is_floating_point_v<value_type>, "mts::audio::wire_base only works with floating point value type.");

private:
  using is_writable = std::bool_constant<!std::is_const_v<element_type>>;

  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

  template <class Container>
  using enable_if_compatible_t
      = std::enable_if_t<detail::is_wire_base_compatible_container<Container, T>::value, std::nullptr_t>;

  template <bool Dummy, class D>
  using cond = dependent_type_condition<Dummy, D>;

  template <bool Dummy, class D>
  using enable_cond = enable_if_dependant_same_t<Dummy, D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_writable>>
  using enable_if_writable = enable_cond<_Dummy, _D>;

public:
  wire_base() noexcept = default;
  wire_base(const wire_base&) noexcept = default;
  wire_base(wire_base&&) noexcept = default;

  wire_base(pointer __buffer, size_type __size) noexcept
      : _buffer(__buffer)
      , _size(__size) {}

  template <class U>
  inline constexpr wire_base(const wire_base<U>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer{ b.data() }
      , _size(b.size()) {}

  template <class U, std::size_t OtherSize>
  inline constexpr wire_base(wire_base<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data())
      , _size(b.size()) {
    mts_assert(OtherSize >= size(), "Invalid size");
  }

  template <class U, std::size_t OtherSize>
  inline constexpr wire_base(const wire_base<U, OtherSize>& b, enable_if_convertible_t<U> = nullptr) noexcept
      : _buffer(b.data())
      , _size(b.size()) {
    mts_assert(OtherSize >= size(), "Invalid size");
  }

  template <std::size_t Size>
  inline constexpr wire_base(element_type (&__arr)[Size]) noexcept
      : _buffer{ __arr }
      , _size{ Size } {}

  template <class U, std::size_t Size, enable_if_convertible_t<U> = nullptr>
  inline constexpr wire_base(std::array<U, Size>& arr) noexcept
      : _buffer{ arr.data() }
      , _size{ Size } {}

  template <class U, std::size_t Size, enable_if_convertible_t<const U> = nullptr>
  inline constexpr wire_base(const std::array<U, Size>& arr) noexcept
      : _buffer{ arr.data() }
      , _size{ Size } {}

  template <class Container>
  inline constexpr wire_base(Container& c, enable_if_compatible_t<Container> = nullptr)
      : _buffer{ std::data(c) }
      , _size{ (size_type)std::size(c) } {}

  template <class Container>
  inline constexpr wire_base(const Container& c, enable_if_compatible_t<const Container> = nullptr)
      : _buffer{ std::data(c) }
      , _size{ (size_type)std::size(c) } {}

  ~wire_base() noexcept = default;

  wire_base& operator=(const wire_base&) noexcept = default;
  wire_base& operator=(wire_base&&) noexcept = default;

  inline bool is_valid() const noexcept { return (bool)_buffer; }

  MTS_NODISCARD inline size_type size() const noexcept { return _size; }
  MTS_NODISCARD inline bool empty() const noexcept { return _size == 0; }

  inline pointer data() noexcept { return _buffer; }
  inline const_pointer data() const noexcept { return _buffer; }

  inline reference operator[](size_type index) noexcept {
    mts_assert(index < _size, "Out of bounds index");
    return _buffer[index];
  }

  inline const_reference operator[](size_type index) const noexcept {
    mts_assert(index < _size, "Out of bounds index");
    return _buffer[index];
  }

  inline iterator begin() noexcept { return iterator(data()); }
  inline const_iterator begin() const noexcept { return const_iterator(data()); }
  inline iterator end() noexcept { return iterator(data() + size()); }
  inline const_iterator end() const noexcept { return const_iterator(data() + size()); }

  inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  inline const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  inline const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  inline const_iterator cbegin() const noexcept { return begin(); }
  inline const_iterator cend() const noexcept { return end(); }
  inline const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  inline const_reverse_iterator crend() const noexcept { return rend(); }

private:
  pointer _buffer = nullptr;
  size_type _size = 0;
};
} // namespace detail.

template <typename T, std::size_t Extent = wire_dynamic_extent>
class wire : public detail::wire_base<T, Extent> {
public:
  using base = detail::wire_base<T, Extent>;
  using element_type = typename base::element_type;
  using value_type = typename base::value_type;
  using size_type = typename base::size_type;
  using reference = typename base::reference;
  using const_reference = typename base::const_reference;
  using pointer = typename base::pointer;
  using const_pointer = typename base::const_pointer;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using reverse_iterator = typename base::reverse_iterator;
  using const_reverse_iterator = typename base::const_reverse_iterator;
  using base::extent;

private:
  using is_writable = std::bool_constant<!std::is_const_v<element_type>>;

  template <class OtherElementType>
  using enable_if_convertible_t
      = std::enable_if_t<std::is_convertible_v<OtherElementType (*)[], element_type (*)[]>, std::nullptr_t>;

  template <class OtherElementType>
  using enable_if_const_convertible_t
      = std::enable_if_t<std::is_convertible_v<const OtherElementType (*)[], const element_type (*)[]>, std::nullptr_t>;

  template <class Container>
  using enable_if_compatible_t
      = std::enable_if_t<detail::is_wire_base_compatible_container<Container, T>::value, std::nullptr_t>;

  template <bool Dummy, class D>
  using cond = dependent_type_condition<Dummy, D>;

  template <bool Dummy, class D>
  using enable_cond = enable_if_dependant_same_t<Dummy, D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_writable>>
  using enable_if_writable = enable_cond<_Dummy, _D>;

public:
  using base::base;
  using base::data;
  using base::is_valid;
  using base::size;
  using base::operator[];
  using base::begin;
  using base::cbegin;
  using base::cend;
  using base::crbegin;
  using base::crend;
  using base::end;
  using base::rbegin;
  using base::rend;

  inline constexpr wire<element_type, wire_dynamic_extent> subwire(
      size_type __offset, size_type __count = wire_dynamic_extent) noexcept {
    mts_assert(__offset <= size(), "Offset out of range in wire::subwire(offset, count)");
    mts_assert(
        __count <= size() || __count == wire_dynamic_extent, "Count out of range in wire::subwire(offset, count)");

    if (__count == wire_dynamic_extent) {
      return wire<element_type, wire_dynamic_extent>{ data() + __offset, size() - __offset };
    }

    mts_assert(__count <= size() - __offset, "Offset + count out of range in wire::subwire(offset, count)");
    return wire<element_type, wire_dynamic_extent>{ data() + __offset, __count };
  }

  template <size_t _Offset, size_t _Count = wire_dynamic_extent,
      std::enable_if_t<!(_Count == wire_dynamic_extent && extent == wire_dynamic_extent), std::nullptr_t> = nullptr>
  inline constexpr auto subwire() noexcept
      -> wire<element_type, _Count == wire_dynamic_extent ? extent - _Offset : _Count> {

    constexpr size_t count = _Count == wire_dynamic_extent ? extent - _Offset : _Count;
    using _ReturnType = wire<element_type, count>;

    if constexpr (extent == wire_dynamic_extent) {
      mts_assert(_Offset <= size(), "Offset out of range in wire::subwire(offset, count)");
      mts_assert(_Offset + count <= size(), "Offset + count out of range in wire::subwire(offset, count)");
      return _ReturnType{ data() + _Offset };
    }
    else {
      static_assert(_Offset <= extent, "Offset out of range in wire::subwire(offset, count)");
      static_assert(_Offset + count <= extent, "Offset + count out of range in wire::subwire(offset, count)");
      return _ReturnType{ data() + _Offset };
    }
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void clear() noexcept {
    vec::clear(data(), 1, size());
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void clear_n(size_type n) noexcept {
    mts_assert(n <= size(), "clear_n() n out of bounds");
    vec::clear(data(), 1, mts::minimum(size(), n));
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void clear_n(size_type offset, size_type n) noexcept {
    mts_assert(n <= size(), "clear_n() n out of bounds");
    mts_assert(offset < size(), "clear_n() offset out of bounds");
    vec::clear(data() + offset, 1, mts::minimum(size() - offset, n));
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void fill(value_type value) noexcept {
    vec::fill(data(), 1, value, size());
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void fill_n(value_type value, size_type n) noexcept {
    mts_assert(n <= size(), "fill_n() n out of bounds");
    vec::fill(data(), 1, value, mts::minimum(size(), n));
  }

  template <bool _Dummy = true, class = enable_if_writable<_Dummy>>
  inline void fill_n(value_type value, size_type offset, size_type n) noexcept {
    mts_assert(n <= size(), "fill_n() n out of bounds");
    mts_assert(offset < size(), "fill_n() offset out of bounds");
    vec::fill(data() + offset, 1, value, mts::minimum(size() - offset, n));
  }

  //
  // Copy.
  //

  // wire[i] = input[i]
  template <class U, std::size_t OtherSize, bool _Dummy = true, class = enable_if_writable<_Dummy>,
      enable_if_const_convertible_t<U> = nullptr>
  inline void copy(const detail::wire_base<U, OtherSize>& input) noexcept {

    static_assert(((extent == wire_dynamic_extent && OtherSize == wire_dynamic_extent)
                      || (extent == wire_dynamic_extent && OtherSize != wire_dynamic_extent)
                      || (extent != wire_dynamic_extent && OtherSize == wire_dynamic_extent))
            || (OtherSize <= extent),
        "");

    mts_assert(input.size() <= size(), "copy() size out of bounds");
    vec::copy(input.data(), 1, data(), 1, mts::minimum(size(), input.size()));
  }

  //
  // Convert.
  //
  inline wire& assign_from_int16(const std::int16_t* input, std::size_t input_stride = 1) {
    vec::convert_from_int16<value_type>(input, input_stride, data(), size());
    return *this;
  }

  inline wire& assign_from_int24(const mts::int24_t* input, std::size_t input_stride = 1) {
    vec::convert_from_int24<value_type>(input, input_stride, data(), size());
    return *this;
  }

  inline wire& assign_from_int32(const std::int32_t* input, std::size_t input_stride = 1) {
    vec::convert_from_int32<value_type>(input, input_stride, data(), size());
    return *this;
  }

  inline wire& assign_from_float(const float* input, std::size_t input_stride = 1) {
    vec::convert_from_float<value_type>(input, input_stride, data(), size());
    return *this;
  }

  inline wire& assign_from_double(const double* input, std::size_t input_stride = 1) {
    vec::convert_from_double<value_type>(input, input_stride, data(), size());
    return *this;
  }

private:
  template <typename _T, typename... Args>
  struct is_assignable_from_any_of {
    static constexpr bool value = (std::is_same_v<_T, Args> || ...);
  };

  template <typename _T>
  using is_assignable_from = is_assignable_from_any_of<_T, std::int16_t, mts::int24_t, std::int32_t, float, double>;

public:
  template <typename _T, typename = typename std::enable_if_t<is_assignable_from<_T>::value, _T>>
  inline wire& assign_from(const _T* input, std::size_t input_stride = 1) {
    if constexpr (std::is_same_v<_T, std::int16_t>) {
      return assign_from_int16(input, input_stride);
    }
    else if constexpr (std::is_same_v<_T, mts::int24_t>) {
      return assign_from_int24(input, input_stride);
    }
    else if constexpr (std::is_same_v<_T, std::int32_t>) {
      return assign_from_int32(input, input_stride);
    }
    else if constexpr (std::is_same_v<_T, float>) {
      return assign_from_float(input, input_stride);
    }
    else if constexpr (std::is_same_v<_T, double>) {
      return assign_from_double(input, input_stride);
    }

    return *this;
  }
  // wire[i] *= value
  inline wire& operator*=(value_type value) noexcept {
    vec::mul(data(), 1, value, data(), 1, size());
    //      vec::mul_value_with_vector(data(), data(), value, size());
    //        for (std::size_t i = 0; i < size(); i++) {
    //          data()[i] *= value;
    //        }
    return *this;
  }
};

MTS_END_NAMESPACE
