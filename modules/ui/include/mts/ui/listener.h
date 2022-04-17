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
#include "mts/pointer.h"
#include "mts/ui/event.h"
#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>

MTS_BEGIN_NAMESPACE
template <class _ListenerType>
class listener_list {
public:
  using listener = _ListenerType;
  using mutex = std::recursive_mutex;
  using scoped_lock = std::scoped_lock<mutex>;

  listener_list() = default;
  ~listener_list() = default;

  inline void add(listener& l) {
    if (!contains(&l)) {
      scoped_lock lock(m_mutex);
      m_listeners.push_back(&l);
    }
  }

  inline void add(_VMTS::not_null<listener*> l) {
    if (l) {
      add(*l);
    }
  }

  inline void remove(listener& l) {
    {
      scoped_lock lock(m_mutex);

      if (auto it = std::find(m_listeners.begin(), m_listeners.end(), &l); it != m_listeners.end()) {
        m_listeners.erase(it);
      }
    }
  }

  inline void remove(_VMTS::not_null<listener*> l) {
    if (l) {
      remove(*l);
    }
  }

  inline std::size_t size() const noexcept {
    return locked_return([this]() { return m_listeners.size(); });
  }

  inline bool empty() const noexcept {
    return locked_return([this]() { return m_listeners.empty(); });
  }

  inline void clear() {
    {
      scoped_lock lock(m_mutex);
      m_listeners.clear();
    }
  }

  inline bool contains(listener& l) const noexcept { return contains(&l); }

  inline bool contains(_VMTS::not_null<listener*> l) const noexcept {
    return locked_return(
        [=]() { return std::find(m_listeners.begin(), m_listeners.end(), (listener*)l) != m_listeners.end(); });
  }

  template <typename Callback>
  inline void call(Callback&& callback) const {
    for (listener* l : update_call_list()) {
      callback(*l);
    }
  }

  inline void call(void (listener::*callback)()) const {
    for (listener* l : update_call_list()) {
      (l->*callback)();
    }
  }

  template <typename... MethodArgs, typename... Args>
  inline void call(void (listener::*callback)(MethodArgs...), Args&&... args) const {
    for (listener* l : update_call_list()) {
      (l->*callback)(std::forward<Args>(args)...);
    }
  }

  template <typename Callback>
  inline void call_async(Callback&& callback) const {
    event_manager::async([this, cb = std::move(callback)]() {
      for (listener* l : update_call_list()) {
        cb(*l);
      }
    });
  }

  inline void call_async(void (listener::*callback)()) const {
    event_manager::async([this, cb = callback]() {
      for (listener* l : update_call_list()) {
        (l->*cb)();
      }
    });
  }

  template <typename... MethodArgs, typename... Args>
  inline void call_async(void (listener::*callback)(MethodArgs...), Args&&... args) const {
    event_manager::async([this, fct = callback, args = std::make_tuple(std::forward<Args>(args)...)]() {
      for (listener* l : update_call_list()) {
        internal_call(l, fct, args);
      }
    });
  }

private:
  std::vector<listener*> m_listeners;
  mutable std::vector<listener*> m_callListeners;
  mutable mutex m_mutex;

  inline std::vector<listener*>& update_call_list() const {
    {
      scoped_lock lock(m_mutex);
      m_callListeners.assign(m_listeners.begin(), m_listeners.end());
    }
    return m_callListeners;
  }

  template <class F, std::enable_if_t<std::is_arithmetic_v<typename std::result_of<F()>::type>, bool> = true>
  inline typename std::result_of<F()>::type locked_return(F&& f) const {
    typename std::result_of<F()>::type r;
    {
      scoped_lock lock(m_mutex);
      r = f();
    }
    return r;
  }

  template <typename... MethodArgs, typename Tuple, std::size_t... I>
  static inline void internal_call(
      listener* l, void (listener::*callback)(MethodArgs...), Tuple& t, std::index_sequence<I...>) {
    (l->*callback)(std::get<I>(t)...);
  }

  template <typename... MethodArgs, typename Tuple>
  static inline void internal_call(listener* l, void (listener::*callback)(MethodArgs...), Tuple& t) {
    return internal_call(l, callback, t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }
};

MTS_END_NAMESPACE
