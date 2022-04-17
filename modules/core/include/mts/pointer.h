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
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>
#include <iosfwd>

MTS_BEGIN_NAMESPACE
template <typename T>
using maybe_null = T;

///
/// owner is designed as a bridge for code that must deal directly with owning pointers for some reason.
/// T must be a pointer type (any type other than pointer type is disallow).
template <class T, class = std::enable_if_t<std::is_pointer_v<T>>>
using owner = T;

template <typename T, typename Fct, typename... Args>
inline auto optional_call(T&& obj, Fct&& fct, Args&&... args) {
  if constexpr (std::is_member_function_pointer_v<Fct>) {
    using return_type = decltype((obj->*fct)(std::forward<Args>(args)...));
    if constexpr (std::is_same_v<return_type, void>) {
      if (obj) {
        (obj->*fct)(std::forward<Args>(args)...);
      }
    }
    else {
      return obj ? (obj->*fct)(std::forward<Args>(args)...) : return_type{};
    }
  }
  else {
    using return_type = decltype(fct(obj, std::forward<Args>(args)...));
    if constexpr (std::is_same_v<return_type, void>) {
      if (obj) {
        fct(obj, std::forward<Args>(args)...);
      }
    }
    else {
      return obj ? fct(obj, std::forward<Args>(args)...) : return_type{};
    }
  }
}

template <class _Tp, class _Dp, typename... Args>
inline typename std::unique_ptr<_Tp, _Dp>::element_type& get_ref(std::unique_ptr<_Tp, _Dp>& ptr, Args&&... args) {
  return *(ptr
          ? ptr
          : (ptr = std::make_unique<typename std::unique_ptr<_Tp, _Dp>::element_type>(std::forward<Args>(args)...)));
}

///
/// not_null
///
/// Restricts a pointer or smart pointer to only hold non-null values.
///
/// Has zero size overhead over T.
///
/// If T is a pointer (i.e. T == U*) then
/// - allow construction from U*
/// - disallow construction from nullptr_t
/// - disallow default construction
/// - ensure construction from null U* fails
/// - allow implicit conversion to U*
///
template <class T>
class not_null {
public:
  static_assert(
      std::is_convertible_v<decltype(std::declval<T>() != nullptr), bool>, "T cannot be compared to nullptr.");

  template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr not_null(U&& u)
      : _ptr(std::forward<U>(u)) {
    mts_assert(_ptr != nullptr, "Pointer is null.");
  }

  template <typename = std::enable_if_t<!std::is_same_v<std::nullptr_t, T>>>
  constexpr not_null(T u)
      : _ptr(std::move(u)) {
    mts_assert(_ptr != nullptr, "Pointer is null.");
  }

  template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr not_null(const not_null<U>& other)
      : not_null(other.get()) {}

  not_null(const not_null& other) = default;
  not_null& operator=(const not_null& other) = default;
  constexpr std::conditional_t<std::is_copy_constructible_v<T>, T, const T&> get() const {
    mts_assert(_ptr != nullptr, "Pointer is null.");
    return _ptr;
  }

  constexpr operator T() const { return get(); }
  constexpr decltype(auto) operator->() const { return get(); }
  constexpr decltype(auto) operator*() const { return *get(); }

  // Prevents compilation when someone attempts to assign a null pointer constant.
  not_null(std::nullptr_t) = delete;
  not_null& operator=(std::nullptr_t) = delete;

  // Unwanted operators, pointers only point to single objects.
  not_null& operator++() = delete;
  not_null& operator--() = delete;
  not_null operator++(int) = delete;
  not_null operator--(int) = delete;
  not_null& operator+=(std::ptrdiff_t) = delete;
  not_null& operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

private:
  T _ptr;
};

template <class T>
auto make_not_null(T&& t) noexcept {
  return not_null<std::remove_cv_t<std::remove_reference_t<T>>>{ std::forward<T>(t) };
}

template <class T>
std::ostream& operator<<(std::ostream& os, const not_null<T>& val) {
  os << val.get();
  return os;
}

template <class T, class U>
auto operator==(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get()) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get()) {
  return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() < rhs.get()))
    -> decltype(lhs.get() < rhs.get()) {
  return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
    -> decltype(lhs.get() <= rhs.get()) {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() > rhs.get()))
    -> decltype(lhs.get() > rhs.get()) {
  return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
    -> decltype(lhs.get() >= rhs.get()) {
  return lhs.get() >= rhs.get();
}

// More unwanted operators.
template <class T, class U>
std::ptrdiff_t operator-(const not_null<T>&, const not_null<U>&) = delete;

template <class T>
not_null<T> operator-(const not_null<T>&, std::ptrdiff_t) = delete;

template <class T>
not_null<T> operator+(const not_null<T>&, std::ptrdiff_t) = delete;

template <class T>
not_null<T> operator+(std::ptrdiff_t, const not_null<T>&) = delete;

template <class _Tp, class = std::enable_if_t<!std::is_pointer_v<_Tp>>>
class managed_ptr;

class managed {
  struct handle_type {};

public:
  class weak_handle_type : private std::weak_ptr<handle_type> {
  public:
    weak_handle_type() = default;
    using weak_ptr::expired;
    using weak_ptr::reset;
    using weak_ptr::swap;

  private:
    friend class managed;

    inline weak_handle_type(std::shared_ptr<handle_type> p)
        : std::weak_ptr<handle_type>(p) {}
  };

  managed() = default;
  managed(const managed&) = delete;
  managed(managed&&) = delete;

  virtual ~managed() = default;

  managed& operator=(const managed&) = delete;
  managed& operator=(managed&&) = delete;

  inline weak_handle_type get_watcher() {
    return _shared_watcher ? _shared_watcher : (_shared_watcher = std::make_shared<handle_type>());
  }

  class validator {
  public:
    validator(managed& m)
        : _watcher(m.get_watcher()) {}

    inline bool expired() const { return _watcher.expired(); }
    inline weak_handle_type get_watcher() const { return _watcher; }

  private:
    weak_handle_type _watcher;
  };

private:
  std::shared_ptr<handle_type> _shared_watcher;

  template <class, class>
  friend class managed_ptr;
};

namespace pointer_detail {
template <class _Tp>
struct managed_if {
  typedef _VMTS::managed_ptr<_Tp> managed_single;
};

template <class _Tp>
struct managed_if<_Tp[]> {
  typedef _VMTS::managed_ptr<_Tp[]> managed_array_unknown_bound;
};

template <class _Tp, std::size_t _Np>
struct managed_if<_Tp[_Np]> {
  typedef void managed_array_known_bound;
};

template <typename _Tp>
struct managed_ptr_type_checker {
  inline managed_ptr_type_checker() noexcept {
    static_assert(std::is_base_of_v<_VMTS::managed, _Tp>, "managed_ptr value_type must be managed.");
  }
};
} // namespace pointer_detail.

template <typename _Tp>
class weak_managed_ptr;

template <class _Tp, class>
class managed_ptr : private pointer_detail::managed_ptr_type_checker<_Tp> {
public:
  using value_type = _Tp;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using validator_type = _VMTS::managed::validator;
  using weak_handle_type = _VMTS::managed::weak_handle_type;

  managed_ptr() = default;

  inline managed_ptr(std::nullptr_t)
      : managed_ptr() {}

  inline managed_ptr(not_null<pointer> obj, bool owned)
      : managed_ptr(owned ? managed_ptr(std::shared_ptr<value_type>(obj.get()), make_shared_tag{}) : managed_ptr(obj)) {
  }

  inline managed_ptr(std::shared_ptr<value_type> obj)
      : _raw(nullptr)
      , _shared(obj) {}

  inline managed_ptr(std::unique_ptr<value_type>&& obj)
      : managed_ptr(std::shared_ptr<value_type>(obj.release()), make_shared_tag{}) {}

  managed_ptr(const managed_ptr& mp)
      : _raw(mp._raw)
      , _shared(mp._shared)
      , _bailout(mp._bailout) {}

  inline managed_ptr(managed_ptr&& mp)
      : _raw(mp._raw)
      , _shared(std::move(mp._shared))
      , _bailout(std::move(mp._bailout)) {
    mp._raw = nullptr;
  }

  template <typename U>
  inline managed_ptr(const managed_ptr<U>& mp)
      : _raw(mp._raw)
      , _shared(std::static_pointer_cast<value_type>(mp._shared))
      , _bailout(mp._bailout) {}

  template <typename U>
  inline managed_ptr(managed_ptr<U>&& mp)
      : _raw(mp._raw)
      , _shared(std::static_pointer_cast<value_type>(std::move(mp._shared)))
      , _bailout(std::move(mp._bailout)) {
    mp._raw = nullptr;
  }

  inline ~managed_ptr() { reset(); }

  inline managed_ptr& operator=(const managed_ptr& mp) {
    _raw = mp._raw;
    _shared = mp._shared;
    _bailout = mp._bailout;
    return *this;
  }

  inline managed_ptr& operator=(managed_ptr&& mp) {
    _raw = mp._raw;
    _shared = std::move(mp._shared);
    _bailout = std::move(mp._bailout);
    mp._raw = nullptr;
    return *this;
  }

  template <typename U>
  inline managed_ptr& operator=(const managed_ptr<U>& mp) {
    _raw = mp._raw;
    _shared = std::static_pointer_cast<value_type>(mp._shared);
    _bailout = mp._bailout;
    return *this;
  }

  template <typename U>
  inline managed_ptr& operator=(managed_ptr<U>&& mp) {
    _raw = mp._raw;
    _shared = std::static_pointer_cast<value_type>(std::move(mp._shared));
    _bailout = std::move(mp._bailout);
    mp._raw = nullptr;
    return *this;
  }

  inline managed_ptr& operator=(std::nullptr_t) {
    reset();
    return *this;
  }

  inline managed_ptr& assign(not_null<pointer> obj, bool owned) {
    reset(obj, owned);
    return *this;
  }

  inline managed_ptr& operator=(std::unique_ptr<value_type>&& obj) {
    reset(std::move(obj));
    return *this;
  }

  inline managed_ptr& operator=(std::shared_ptr<value_type> obj) {
    reset(obj);
    return *this;
  }

  inline bool is_valid() const noexcept { return (is_shared() || (_raw && !_bailout.expired())) ? true : false; }

  inline explicit operator bool() const { return is_valid(); }

  inline bool is_reference() const noexcept { return _raw; }
  inline bool is_shared() const noexcept { return (bool)_shared; }

  inline std::add_lvalue_reference_t<value_type> operator*() { return *get(); }
  inline std::add_lvalue_reference_t<value_type> operator*() const { return *get(); }

  inline pointer get() { return _shared ? _shared.get() : _raw; }
  inline const_pointer get() const { return _shared ? _shared.get() : _raw; }

  inline pointer get_checked() const { return is_valid() ? get() : nullptr; }

  inline pointer operator->() { return get(); }
  inline const_pointer operator->() const { return get(); }

  explicit inline operator pointer() { return get(); }
  explicit inline operator const_pointer() const { return get(); }

  inline void reset() {
    _raw = nullptr;
    _shared.reset();
    _bailout.reset();
  }

  inline void reset(not_null<pointer> obj, bool owned) {
    reset();
    if (owned) {
      _shared = std::shared_ptr<value_type>(obj.get());
    }
    else {
      _raw = obj;
      _bailout = obj->get_watcher();
    }
  }

  inline void reset(std::unique_ptr<value_type>&& obj) {
    reset();
    _shared = std::shared_ptr<value_type>(obj.release());
  }

  inline void reset(std::shared_ptr<value_type> obj) {
    reset();
    _shared = obj;
  }

  inline void swap(managed_ptr& mp) noexcept {
    std::swap(_raw, mp._raw);
    std::swap(_shared, mp._shared);
    std::swap(_bailout, mp._bailout);
  }

  inline friend void swap(managed_ptr& a, managed_ptr& b) { a.swap(b); }

private:
  pointer _raw = nullptr;
  std::shared_ptr<value_type> _shared;
  weak_handle_type _bailout;

  inline std::shared_ptr<value_type> get_shared() { return _shared; }
  inline std::shared_ptr<value_type> get_shared() const { return _shared; }

  inline weak_handle_type get_watcher() { return _bailout; }
  inline weak_handle_type get_watcher() const { return _bailout; }

  struct make_shared_tag {};

  inline managed_ptr(not_null<pointer> obj) {
    _raw = obj;
    _bailout = obj->get_watcher();
  }

  inline managed_ptr(std::shared_ptr<value_type>&& obj, make_shared_tag)
      : _raw(nullptr)
      , _shared(obj) {}

  template <class U, class>
  friend class managed_ptr;

  template <typename U>
  friend class weak_managed_ptr;

  template <class U, class... _Args>
  friend typename pointer_detail::managed_if<U>::managed_single make_managed(_Args&&... __args);
};

// Deduction guides.
template <class T>
managed_ptr(T&) -> managed_ptr<T>;

template <class T>
managed_ptr(std::unique_ptr<T>&&) -> managed_ptr<T>;

template <class T>
managed_ptr(std::shared_ptr<T>) -> managed_ptr<T>;

template <class _T1, class _T2>
inline bool operator==(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  return __x.get() == __y.get();
}

template <class _T1, class _T2>
inline bool operator!=(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  return !(__x == __y);
}

template <class _T1, class _T2>
inline bool operator<(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  typedef typename managed_ptr<_T1>::pointer _P1;
  typedef typename managed_ptr<_T2>::pointer _P2;
  typedef typename std::common_type<_P1, _P2>::type _Vp;
  return std::less<_Vp>()(__x.get(), __y.get());
}

template <class _T1, class _T2>
inline bool operator>(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  return __y < __x;
}

template <class _T1, class _T2>
inline bool operator<=(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  return !(__y < __x);
}

template <class _T1, class _T2>
inline bool operator>=(const managed_ptr<_T1>& __x, const managed_ptr<_T2>& __y) {
  return !(__x < __y);
}

template <class _T1, class _D1>
inline bool operator==(const managed_ptr<_T1>& __x, std::nullptr_t) noexcept {
  return !__x;
}

template <class _T1, class _D1>
inline bool operator==(std::nullptr_t, const managed_ptr<_T1>& __x) noexcept {
  return !__x;
}

template <class _T1, class _D1>
inline bool operator!=(const managed_ptr<_T1>& __x, std::nullptr_t) noexcept {
  return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline bool operator!=(std::nullptr_t, const managed_ptr<_T1>& __x) noexcept {
  return static_cast<bool>(__x);
}

template <class _T1, class _D1>
inline bool operator<(const managed_ptr<_T1>& __x, std::nullptr_t) {
  typedef typename managed_ptr<_T1>::pointer _P1;
  return std::less<_P1>()(__x.get(), nullptr);
}

template <class _T1, class _D1>
inline bool operator<(std::nullptr_t, const managed_ptr<_T1>& __x) {
  typedef typename managed_ptr<_T1>::pointer _P1;
  return std::less<_P1>()(nullptr, __x.get());
}

template <class _T1, class _D1>
inline bool operator>(const managed_ptr<_T1>& __x, std::nullptr_t) {
  return nullptr < __x;
}

template <class _T1, class _D1>
inline bool operator>(std::nullptr_t, const managed_ptr<_T1>& __x) {
  return __x < nullptr;
}

template <class _T1, class _D1>
inline bool operator<=(const managed_ptr<_T1>& __x, std::nullptr_t) {
  return !(nullptr < __x);
}

template <class _T1, class _D1>
inline bool operator<=(std::nullptr_t, const managed_ptr<_T1>& __x) {
  return !(__x < nullptr);
}

template <class _T1, class _D1>
inline bool operator>=(const managed_ptr<_T1>& __x, std::nullptr_t) {
  return !(__x < nullptr);
}

template <class _T1, class _D1>
inline bool operator>=(std::nullptr_t, const managed_ptr<_T1>& __x) {
  return !(nullptr < __x);
}

template <class _Tp, class... _Args>
inline typename pointer_detail::managed_if<_Tp>::managed_single make_managed(_Args&&... __args) {
  using make_shared_tag = typename _VMTS::managed_ptr<_Tp>::make_shared_tag;
  return _VMTS::managed_ptr<_Tp>(std::make_shared<_Tp>(std::forward<_Args>(__args)...), make_shared_tag{});
}

//
template <class _Tp>
inline typename pointer_detail::managed_if<_Tp>::managed_array_unknown_bound make_managed(std::size_t __n) {
  using type = std::remove_extent_t<_Tp>;
  return _VMTS::managed_ptr<_Tp>(new type[__n](), true);
}

template <class _Tp, class... _Args>
typename pointer_detail::managed_if<_Tp>::managed_array_known_bound make_managed(_Args&&...) = delete;

namespace pointer_detail {
template <typename _Tp>
struct weak_managed_ptr_type_checker {
  inline weak_managed_ptr_type_checker() noexcept {
    static_assert(std::is_base_of_v<_VMTS::managed, _Tp>, "weak_managed_ptr value_type must be managed.");
  }
};
} // namespace pointer_detail.

template <typename _Tp>
class weak_managed_ptr : private pointer_detail::weak_managed_ptr_type_checker<_Tp> {
public:
  using value_type = _Tp;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using weak_handle_type = _VMTS::managed::weak_handle_type;

  weak_managed_ptr() noexcept = default;

  inline weak_managed_ptr(std::nullptr_t) noexcept
      : weak_managed_ptr() {}

  inline weak_managed_ptr(not_null<pointer> obj)
      : _watcher(obj->get_watcher())
      , _obj(obj.get()) {}

  inline weak_managed_ptr(_VMTS::managed_ptr<value_type>& m) {
    if (!m.is_valid()) {
      _obj = nullptr;
      return;
    }

    if (m.is_shared()) {
      _weak_ptr = m.get_shared();
      _obj = nullptr;
      return;
    }

    _watcher = m.get_watcher();
    _obj = m.get();
  }

  inline weak_managed_ptr(const _VMTS::managed_ptr<value_type>& m) {
    if (!m.is_valid()) {
      _obj = nullptr;
      return;
    }

    if (m.is_shared()) {
      _weak_ptr = m.get_shared();
      _obj = nullptr;
      return;
    }

    _watcher = m.get_watcher();
    _obj = m.get();
  }

  template <typename U>
  inline weak_managed_ptr(const _VMTS::managed_ptr<U>& m)

  {
    if (!m.is_valid()) {
      _obj = nullptr;
      return;
    }

    if (m.is_shared()) {
      _weak_ptr = std::static_pointer_cast<value_type>(m.get_shared());
      _obj = nullptr;
      return;
    }

    _watcher = m.get_watcher();
    _obj = static_cast<value_type*>(m.get());
  }

  weak_managed_ptr(const weak_managed_ptr& wp) noexcept
      : _weak_ptr(wp._weak_ptr)
      , _watcher(wp._watcher)
      , _obj(wp._obj) {}

  weak_managed_ptr(weak_managed_ptr&& sp) noexcept
      : _weak_ptr(std::move(sp._weak_ptr))
      , _watcher(std::move(sp._watcher))
      , _obj(sp._obj) {
    sp._obj = nullptr;
  }

  template <typename U>
  inline weak_managed_ptr(weak_managed_ptr<U>&& sp)
      : _weak_ptr(std::static_pointer_cast<value_type>(std::move(sp._weak_ptr).lock()))
      , _watcher(sp._watcher)
      , _obj(static_cast<value_type*>(sp._obj)) {}

  template <typename U>
  inline weak_managed_ptr(const weak_managed_ptr<U>& sp)
      : _weak_ptr(std::static_pointer_cast<value_type>(sp._weak_ptr.lock()))
      , _watcher(sp._watcher)
      , _obj(static_cast<value_type*>(sp._obj)) {}

  ~weak_managed_ptr() = default;

  weak_managed_ptr& operator=(const weak_managed_ptr&) noexcept = default;

  weak_managed_ptr& operator=(weak_managed_ptr&& sp) noexcept {
    _weak_ptr = std::move(sp._weak_ptr);
    _watcher = std::move(sp._watcher);
    _obj = sp._obj;
    sp._obj = nullptr;
    return *this;
  }

  inline weak_managed_ptr& operator=(std::nullptr_t) noexcept {
    _weak_ptr.reset();
    _watcher.reset();
    _obj = nullptr;
    return *this;
  }

  inline weak_managed_ptr& operator=(not_null<pointer> obj) {
    _weak_ptr.reset();
    _watcher = obj->get_watcher();
    _obj = obj.get();
    return *this;
  }

  inline weak_managed_ptr& operator=(_VMTS::managed_ptr<value_type>& m) {
    reset();

    if (!m.is_valid()) {
      return *this;
    }

    if (m.is_shared()) {
      _weak_ptr = m.get_shared();
      return *this;
    }

    _watcher = m.get_watcher();
    _obj = m.get();
    return *this;
  }

  template <typename U>
  inline weak_managed_ptr& operator=(_VMTS::managed_ptr<U>& m) {
    reset();

    if (!m.is_valid()) {
      return *this;
    }

    if (m.is_shared()) {
      _weak_ptr = std::static_pointer_cast<value_type>(m.get_shared());
      return *this;
    }

    _watcher = m.get_watcher();
    _obj = static_cast<value_type*>(m.get());
    return *this;
  }

  inline bool is_valid() const noexcept {
    return (!_weak_ptr.expired() || (_obj && !_watcher.expired())) ? true : false;
  }

  inline explicit operator bool() const { return is_valid(); }

  inline void reset() noexcept {
    _weak_ptr.reset();
    _watcher.reset();
    _obj = nullptr;
  }

  inline value_type& operator*() const { return *get(); }
  inline pointer get() {
    if (!_weak_ptr.expired()) {
      return _weak_ptr.lock().get();
    }

    if (_obj && !_watcher.expired()) {
      return _obj;
    }

    return nullptr;
  }

  inline const_pointer get() const {
    if (!_weak_ptr.expired()) {
      return _weak_ptr.lock().get();
    }

    if (_obj && !_watcher.expired()) {
      return _obj;
    }

    return nullptr;
  }

  inline pointer operator->() { return get(); }
  inline const_pointer operator->() const { return get(); }

  inline operator pointer() { return get(); }
  inline operator const_pointer() const { return get(); }

  inline void swap(weak_managed_ptr& sp) noexcept {
    _watcher.swap(sp._watcher);
    std::swap(_obj, sp._obj);
  }

  inline bool operator==(const weak_managed_ptr& sp) const noexcept {
    bool valid = is_valid();
    bool sp_valid = sp.is_valid();

    if (valid != sp_valid) {
      return false;
    }

    return (!valid && !sp_valid) ? true : (get() == sp.get());
  }

  inline bool operator==(const pointer obj) const noexcept { return get() == obj; }

  inline bool operator==(std::nullptr_t) const noexcept { return !is_valid(); }

private:
  std::weak_ptr<value_type> _weak_ptr;
  weak_handle_type _watcher;
  pointer _obj = nullptr;

  template <class U>
  friend class weak_managed_ptr;
};
MTS_END_NAMESPACE
