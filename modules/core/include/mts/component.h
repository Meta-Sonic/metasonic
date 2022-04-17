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
#include <type_traits>

MTS_BEGIN_NAMESPACE
/// Can only be derived by component::base<...>.
class component {
public:
  using type = std::size_t;
  virtual ~component() = default;
  virtual type get_type() const noexcept = 0;

  template <typename T>
  class base;

  template <typename T>
  static inline type get_component_type() noexcept {
    return component_type_mapper<T>::type;
  }

private:
  component() = default;

  inline static type generate_new_type() noexcept {
    static type __type = 0;
    return ++__type;
  }

  template <typename T>
  class component_type_mapper {
  public:
    inline static const type type = generate_new_type();
  };
};

///
///
/// @code
/// class my_component : public component::base<my_component> {
/// public:
///   my_component(int v) : value(v) {}
///   int value;
/// };
/// @endcode
///
template <typename T>
class component::base : public component {
public:
  inline base() {
    mts_assert((std::is_base_of_v<component, T> && std::is_base_of_v<base<T>, T>), "Wrong component type");
  }
  virtual ~base() override = default;
  virtual type get_type() const noexcept override final { return component_type_mapper<T>::type; }
};

MTS_END_NAMESPACE
