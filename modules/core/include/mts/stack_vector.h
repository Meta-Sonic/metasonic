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
#include "mts/assert.h"
#include "mts/traits.h"
#include <array>
#include <exception>
#include <initializer_list>
#include <type_traits>

MTS_BEGIN_NAMESPACE
template <class T, std::size_t _Size>
class stack_vector : private std::array<T, _Size> {

  using is_default_constructible = std::bool_constant<std::is_default_constructible_v<T>>;
  using is_copy_constructible
      = std::bool_constant<std::is_copy_constructible_v<T> || std::is_trivially_copy_constructible_v<T>>;
  using is_move_constructible = std::bool_constant<std::is_move_constructible_v<T>>;
  using is_copy_assignable = std::bool_constant<std::is_copy_assignable_v<T>>;
  using is_move_assignable = std::bool_constant<std::is_move_assignable_v<T>>;
  using is_assignable = std::bool_constant<is_copy_assignable::value || is_move_assignable::value>;
  using is_copy_resizable = std::bool_constant<is_copy_assignable::value && is_default_constructible::value>;
  using is_default_erasable = std::bool_constant<is_assignable::value && is_default_constructible::value>;

  template <class... Ts>
  using are_all_convertible = _VMTS::all_true<std::is_same_v<Ts, T>...>;

  template <bool Dummy, class D>
  using cond = dependent_type_condition<Dummy, D>;

  template <bool Dummy, class D>
  using enable_cond = enable_if_dependant_same_t<Dummy, D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_copy_constructible>>
  using enable_if_copy_constructible = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_move_constructible>>
  using enable_if_move_constructible = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_assignable>>
  using enable_if_assignable = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_copy_assignable>>
  using enable_if_copy_assignable = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_move_assignable>>
  using enable_if_move_assignable = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_default_erasable>>
  using enable_if_default_erasable = enable_cond<_Dummy, _D>;

  template <bool _Dummy, class _D = cond<_Dummy, is_copy_resizable>>
  using enable_if_copy_resizable = enable_cond<_Dummy, _D>;

public:
  using array_t = std::array<T, _Size>;
  using value_type = typename array_t::value_type;
  using reference = typename array_t::reference;
  using const_reference = typename array_t::const_reference;
  using iterator = typename array_t::iterator;
  using const_iterator = typename array_t::const_iterator;
  using pointer = typename array_t::pointer;
  using const_pointer = typename array_t::const_pointer;
  using size_type = typename array_t::size_type;
  using difference_type = typename array_t::difference_type;
  using reverse_iterator = typename array_t::reverse_iterator;
  using const_reverse_iterator = typename array_t::const_reverse_iterator;

  template <class... Ts, typename = typename std::enable_if<are_all_convertible<Ts...>::value>::type>
  inline constexpr stack_vector(Ts&&... ts)
      : array_t{ std::forward<Ts>(ts)... }
      , _size(sizeof...(Ts)) {}

  inline constexpr stack_vector() noexcept = default;

  inline constexpr stack_vector(const stack_vector& ua) noexcept
      : array_t(static_cast<const array_t&>(ua))
      , _size(ua._size) {}

  inline constexpr stack_vector(stack_vector&& ua) noexcept
      : array_t(std::move(static_cast<array_t&>(ua)))
      , _size(ua._size) {
    ua._size = 0;
  }

  inline constexpr stack_vector& operator=(const stack_vector& ua) noexcept {
    array_t::operator=(static_cast<const array_t&>(ua));
    _size = ua._size;
    return *this;
  }

  inline constexpr stack_vector& operator=(stack_vector&& ua) noexcept {
    array_t::operator=(std::move(static_cast<array_t&>(ua)));
    _size = ua._size;
    ua._size = 0;
    return *this;
  }

  using array_t::data;
  using array_t::front;
  using array_t::max_size;

  inline constexpr size_type size() const noexcept { return _size; }
  inline constexpr size_type capacity() const noexcept { return _Size; }
  inline constexpr bool empty() const noexcept { return _size == 0; }

  inline constexpr iterator begin() { return array_t::begin(); }
  inline constexpr const_iterator begin() const { return array_t::begin(); }
  inline constexpr iterator end() { return array_t::begin() + _size; }
  inline constexpr const_iterator end() const { return array_t::begin() + _size; }

  inline constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  inline constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  inline constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  inline constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  inline constexpr const_iterator cbegin() const noexcept { return begin(); }
  inline constexpr const_iterator cend() const noexcept { return end(); }
  inline constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  inline constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  inline constexpr reference back() noexcept { return array_t::operator[](_size - 1); }
  inline constexpr const_reference back() const noexcept { return array_t::operator[](_size - 1); }

  inline constexpr void clear() {
    if constexpr (!std::is_trivial<value_type>::value) {
      for (size_type i = 0; i < _size; i++) {
        (*this)[i] = value_type();
      }
    }

    _size = 0;
  }

  inline constexpr bool is_full() const noexcept { return _size == max_size(); }

  template <bool _Dummy = true, class = enable_if_copy_assignable<_Dummy>>
  inline constexpr void push_back(const value_type& value) {
    mts_assert(_size != max_size(), "Can't push_back when stack_vector is full.");

    if (_size == max_size()) {
      return;
    }

    (*this)[_size++] = value;
  }

  template <bool _Dummy = true, class = enable_if_move_assignable<_Dummy>>
  inline constexpr void push_back(value_type&& value) {
    mts_assert(_size != max_size(), "Can't push_back when stack_vector is full.");

    if (_size == max_size()) {
      return;
    }

    (*this)[_size++] = std::move(value);
  }

  template <typename... Args, bool _Dummy = true, class = enable_if_assignable<_Dummy>>
  inline constexpr void emplace_back(Args&&... args) {
    mts_assert(_size != max_size(), "Can't emplace_back when stack_vector is full.");

    if (_size == max_size()) {
      return;
    }

    construct((*this)[_size++], value_type(std::forward<Args>(args)...));
  }

  constexpr iterator insert(const_iterator pos, const value_type& value) {
    mts_assert(_size != max_size(), "Can't insert when stack_vector is full.");

    if (_size == max_size()) {
      return nullptr;
    }

    if (!_size || pos == cend()) {
      push_back(value);
      return end() - 1;
    }

    size_type index = std::distance(cbegin(), pos);
    shift_right(begin() + index, end(), 1);
    construct((*this)[index], value);
    _size++;
    return begin() + index;
  }

  constexpr iterator insert(const_iterator pos, value_type&& value) {
    mts_assert(_size != max_size(), "Can't insert when stack_vector is full.");

    if (_size == max_size()) {
      return nullptr;
    }

    if (!_size || pos == cend()) {
      push_back(value);
      return end() - 1;
    }

    size_type index = std::distance(cbegin(), pos);
    shift_right(begin() + index, end(), 1);
    construct((*this)[index], std::move(value));
    _size++;
    return begin() + index;
  }

  inline constexpr void pop_back() {
    mts_assert(_size > 0, "Can't pop_back an empty stack_vector.");

    if (_size) {
      --_size;
      default_construct((*this)[_size]);
    }
  }

  template <typename... Args, bool _Dummy = true, class = enable_if_default_erasable<_Dummy>>
  inline constexpr void erase(size_type index) {
    mts_assert(index < _size, "Try to erase out of bounds index.");

    if (_size == 0) {
      return;
    }

    if (index != _size - 1) {
      shift_left(begin() + index + 1, end(), 1);
    }

    _size--;

    default_construct((*this)[_size]);
  }

  template <bool _Dummy = true, class = enable_if_default_erasable<_Dummy>>
  inline constexpr void unordered_erase(size_type index) {
    mts_assert(index < _size, "Try to erase out of bounds index.");

    if (_size == 0) {
      return;
    }

    // Move last element to index unless we're erasing the last element.
    if (index != _size - 1) {
      construct((*this)[index], std::move(back()));
    }

    _size--;
    default_construct((*this)[_size]);
  }

  template <bool _Dummy = true, class = enable_if_copy_resizable<_Dummy>>
  constexpr void resize(size_type count, const value_type& value) {

    count = std::max(_size, count);

    if (count == _size) {
      return;
    }

    if (count < _size) {
      default_construct_range(begin() + count, begin() + _size);
    }
    else {
      copy_value_to_range(begin() + _size, begin() + count, value);
    }

    _size = count;
  }

  template <bool _Dummy = true, class = enable_if_default_erasable<_Dummy>>
  constexpr void resize(size_type count) {

    count = std::max(_size, count);

    if (count == _size) {
      return;
    }

    if constexpr (!std::is_trivial<value_type>::value) {
      default_construct_range(std::minmax(count, _size));
    }

    _size = count;
  }

  inline constexpr void swap(stack_vector& __a) noexcept(std::is_nothrow_swappable_v<value_type>) {
    std::swap_ranges(data(), data() + _Size, __a.data());
  }

  // protected:
  using array_t::at;
  using array_t::operator[];

  inline constexpr array_t& to_array() noexcept { return *this; }
  inline constexpr const array_t& to_array() const noexcept { return *this; }

private:
  size_type _size = 0;

  inline constexpr void default_construct(value_type& dst) {
    if constexpr (!std::is_trivial<value_type>::value) {
      if constexpr (is_move_assignable::value) {
        dst = value_type();
      }
      else {
        copy_construct(dst, value_type());
      }
    }
  }

  inline constexpr void default_construct_range(iterator first, iterator last) {
    if constexpr (!std::is_trivial<value_type>::value) {
      for (iterator it = first; it < last; ++it) {
        default_construct(*it);
      }
    }
  }

  inline constexpr void default_construct_range(std::pair<size_type, size_type> range) {
    default_construct_range(begin() + range.first, begin() + range.second);
  }

  inline constexpr void copy_value_to_range(iterator first, iterator last, const value_type& value) {
    for (iterator it = first; it < last; ++it) {
      *it = value;
    }
  }

  inline constexpr void copy_value_to_range(std::pair<size_type, size_type> range, const value_type& value) {
    copy_value_to_range(begin() + range.first, begin() + range.second, value);
  }

  inline constexpr void copy_construct(value_type& dst, const value_type& value) { dst = value; }

  inline constexpr void construct(value_type& dst, value_type&& value) {
    if constexpr (is_move_assignable::value) {
      dst = std::move(value);
    }
    else {
      copy_construct(dst, value);
    }
  }

  inline constexpr void construct(value_type& dst, const value_type& value) { dst = value; }

  inline constexpr void shift_left(iterator first, iterator last, size_type diff) {
    if constexpr (is_move_assignable::value) {
      std::move(first, last, first - diff);
    }
    else {
      std::copy(first, last, first - diff);
    }
  }

  inline constexpr void shift_right(iterator first, iterator last, size_type diff) {
    if constexpr (is_move_assignable::value) {
      std::move_backward(first, last, last + diff);
    }
    else {
      std::copy_backward(first, last, last + diff);
    }
  }
};

template <class T, std::size_t _Size>
inline constexpr bool operator==(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return __x.size() == __y.size() && std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class T, std::size_t _Size>
inline constexpr bool operator!=(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return !(__x == __y);
}

template <class T, std::size_t _Size>
inline constexpr bool operator<(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class T, std::size_t _Size>
inline constexpr bool operator>(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return __y < __x;
}

template <class T, std::size_t _Size>
inline constexpr bool operator<=(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return !(__y < __x);
}

template <class T, std::size_t _Size>
inline constexpr bool operator>=(const stack_vector<T, _Size>& __x, const stack_vector<T, _Size>& __y) {
  return !(__x < __y);
}
MTS_END_NAMESPACE
