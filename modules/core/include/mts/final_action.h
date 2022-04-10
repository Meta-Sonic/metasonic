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
#include <functional>
#include <type_traits>
#include <utility>

MTS_BEGIN_NAMESPACE
/// final_action allows you to ensure something gets run at the end of a scope.
template <typename _Fct>
class final_action {
public:
  using function_type = _Fct;
  static_assert(!std::is_reference<function_type>::value && !std::is_const<function_type>::value
          && !std::is_volatile<function_type>::value,
      "final_action should store its callable by value");

  inline final_action(function_type&& f) noexcept
      : _fct(std::move(f)) {}

  inline final_action(const function_type& f)
      : _fct(f) {}

  inline final_action(final_action&& other) noexcept
      : _fct(std::move(other._fct))
      , _need_invoke(std::exchange(other._need_invoke, false)) {}

  final_action(const final_action&) = delete;
  final_action& operator=(const final_action&) = delete;
  final_action& operator=(final_action&&) = delete;

  inline ~final_action() noexcept {
    if (_need_invoke && _fct) {
      _fct();
    }
  }

  inline void cancel() {
    _fct = function_type();
    _need_invoke = false;
  }

private:
  function_type _fct;
  bool _need_invoke = true;
};

class final_member {
public:
  using function_type = std::function<void()>;

  final_member() = default;

  final_member(function_type&& f)
      : _fct(std::move(f))
      , _need_invoke(true) {}

  final_member(final_member&& other) noexcept
      : _fct(std::move(other._fct))
      , _need_invoke(std::exchange(other._need_invoke, false)) {}

  final_member(const final_member&) = delete;
  final_member& operator=(const final_member&) = delete;
  final_member& operator=(final_member&& other) {
    _fct = std::move(other._fct);
    _need_invoke = std::exchange(other._need_invoke, false);
    return *this;
  };

  ~final_member() noexcept {
    if (_need_invoke) {
      _fct();
    }
  }

private:
  function_type _fct;
  bool _need_invoke = false;
};

MTS_END_NAMESPACE
