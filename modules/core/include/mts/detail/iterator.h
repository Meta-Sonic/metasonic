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
#include "mts/traits.h"

MTS_BEGIN_NAMESPACE

template <typename T>
class __wrap_iterator {
public:
  using iterator_type = T;
  using iterator_category = typename std::iterator_traits<iterator_type>::iterator_category;
  using value_type = typename std::iterator_traits<iterator_type>::value_type;
  using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
  using pointer = typename std::iterator_traits<iterator_type>::pointer;
  using reference = typename std::iterator_traits<iterator_type>::reference;

  __wrap_iterator() noexcept = default;
  ~__wrap_iterator() noexcept = default;

  inline explicit __wrap_iterator(iterator_type x) noexcept
      : __i(x) {}

  template <class U>
  inline __wrap_iterator(
      const __wrap_iterator<U>& other, std::enable_if_t<std::is_convertible_v<U, iterator_type>>* = 0) noexcept
      : __i(other.base()) {}

  template <class U, std::enable_if_t<std::is_convertible_v<U, iterator_type>>* = 0>
  inline __wrap_iterator& operator=(const __wrap_iterator<U>& other) noexcept {
    __i = other.base();
    return *this;
  }

  inline __wrap_iterator operator++(int) noexcept { return __i++; }

  inline __wrap_iterator& operator++() noexcept {
    ++__i;
    return *this;
  }

  inline __wrap_iterator operator--(int) noexcept { return __i--; }
  inline __wrap_iterator& operator--() noexcept {
    --__i;
    return *this;
  }

  inline __wrap_iterator& operator+=(difference_type n) noexcept {
    __i += n;
    return *this;
  }

  inline __wrap_iterator& operator-=(difference_type n) noexcept {
    __i -= n;
    return *this;
  }

  inline __wrap_iterator operator+(difference_type v) const noexcept { return __wrap_iterator(__i + v); }

  inline __wrap_iterator operator-(difference_type v) const noexcept { return __wrap_iterator(__i - v); }

  inline reference operator*() const noexcept { return *__i; }

  inline pointer operator->() const noexcept { return __i; }

  inline operator pointer() noexcept { return __i; }

  inline reference operator[](difference_type __n) const noexcept { return __i[__n]; }

  inline bool operator==(const __wrap_iterator& rhs) const noexcept { return __i == rhs.__i; }

  inline bool operator!=(const __wrap_iterator& rhs) const noexcept { return __i != rhs.__i; }

  inline iterator_type base() const noexcept { return __i; }

private:
  iterator_type __i = nullptr;
};

MTS_END_NAMESPACE

template <typename T>
inline _VMTS::__wrap_iterator<T> begin(T* val) {
  return _VMTS::__wrap_iterator<T>(val);
}

template <typename T, typename Tsize>
inline _VMTS::__wrap_iterator<T> end(T* val, Tsize size) {
  return _VMTS::__wrap_iterator<T>(val) + size;
}

template <class Iterator1, class Iterator2>
inline bool operator==(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() != rhs.base();
}
template <class Iterator1, class Iterator2>
inline bool operator!=(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() != rhs.base();
}

template <class Iterator1, class Iterator2>
inline bool operator>(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() > rhs.base();
}

template <class Iterator1, class Iterator2>
inline bool operator>=(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() >= rhs.base();
}

template <class Iterator1, class Iterator2>
inline bool operator<(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() < rhs.base();
}

template <class Iterator1, class Iterator2>
inline bool operator<=(const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() <= rhs.base();
}

template <class Iterator>
inline _VMTS::__wrap_iterator<Iterator> operator+(
    typename _VMTS::__wrap_iterator<Iterator>::difference_type n, const _VMTS::__wrap_iterator<Iterator>& it) {
  return it.base() + n;
}

template <class Iterator1, class Iterator2>
inline typename _VMTS::__wrap_iterator<Iterator1>::difference_type operator-(
    const _VMTS::__wrap_iterator<Iterator1>& lhs, const _VMTS::__wrap_iterator<Iterator2>& rhs) {
  return lhs.base() - rhs.base();
}
