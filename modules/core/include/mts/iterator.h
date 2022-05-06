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
#include <iterator>

MTS_BEGIN_NAMESPACE
template <typename IteratorT>
class iterator_range {
  IteratorT _begin_iterator;
  IteratorT _end_iterator;

public:
  // TODO: Add SFINAE to test that the Container's iterators match the range's

  template <typename Container>
  inline iterator_range(Container&& c)
      // TODO: Consider ADL/non-member begin/end calls.
      : _begin_iterator(c.begin())
      , _end_iterator(c.end()) {}

  inline iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
      : _begin_iterator(std::move(begin_iterator))
      , _end_iterator(std::move(end_iterator)) {}

  inline IteratorT begin() const { return _begin_iterator; }
  inline IteratorT end() const { return _end_iterator; }
  inline bool empty() const { return _begin_iterator == _end_iterator; }
};

template <class _Container>
class start_index_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
public:
  using container_type = _Container;
  using value_type = typename _Container::value_type;
  using size_type = typename _Container::size_type;

  start_index_insert_iterator() = delete;
  start_index_insert_iterator(start_index_insert_iterator&&) noexcept = default;
  start_index_insert_iterator(const start_index_insert_iterator&) noexcept = default;

  inline explicit start_index_insert_iterator(_Container& __x, size_type index) noexcept
      : container(std::addressof(__x))
      , _index(std::clamp<size_type>(index, 0, __x.size())) {}

  start_index_insert_iterator& operator=(start_index_insert_iterator&&) noexcept = default;
  start_index_insert_iterator& operator=(const start_index_insert_iterator&) noexcept = default;

  inline start_index_insert_iterator& operator=(const value_type& __value_) {
    container->insert(container->begin() + _index, __value_);
    return *this;
  }

  inline start_index_insert_iterator& operator=(value_type&& __value_) {
    container->insert(container->begin() + _index, __value_);
    return *this;
  }

  inline start_index_insert_iterator& operator*() noexcept { return *this; }

  inline start_index_insert_iterator& operator++() noexcept {
    ++_index;
    return *this;
  }

  inline start_index_insert_iterator operator++(int) noexcept {
    start_index_insert_iterator __tmp(*this);
    ++_index;
    return __tmp;
  }

private:
  container_type* container;
  size_type _index;
};

MTS_END_NAMESPACE
