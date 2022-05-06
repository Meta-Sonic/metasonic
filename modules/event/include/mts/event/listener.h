///
/// Copyright (C) 2021, Meta-Sonic
/// All rights reserved.
///
/// Proprietary and confidential.
/// Any unauthorized copying, alteration, distribution, transmission,
/// performance, display or other use of this material is strictly prohibited.
///
/// Written by Alexandre Arsenault <alx.arsenault@gmail.com>
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
