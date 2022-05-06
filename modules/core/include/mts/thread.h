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
#include "mts/waitable.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

MTS_BEGIN_NAMESPACE

MTS_API void set_thread_name(std::thread::native_handle_type handle, const char* name);
MTS_API void kill_thread(std::thread::native_handle_type handle);

class thread {
  struct impl;

public:
  using handle = std::thread::native_handle_type;
  class proxy;

  class callback {
  public:
    virtual ~callback() = default;

  protected:
    virtual void run(const proxy&) = 0;

  private:
    friend struct impl;
  };

  template <typename Fct>
  class callback_t : public callback {
  public:
    callback_t(Fct&& f)
        : fct(std::forward<Fct>(f)) {}

    virtual ~callback_t() = default;

  protected:
    virtual void run(const proxy& p) override { fct(p); }

  private:
    Fct fct;
  };

  class proxy {
  public:
    proxy() = delete;
    ~proxy() noexcept = default;

    proxy& operator=(const proxy&) = delete;
    proxy& operator=(proxy&&) = delete;

    bool running() const noexcept;
    bool should_stop() const noexcept;
    std::thread::id id() const noexcept;

    handle native_handle() const noexcept;

    void yield() const;

    void wait_for(const std::chrono::nanoseconds& wait_time) const;

    void wait_until(const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& timeout) const;

    template <class Rep, class Period>
    inline void wait_for(const std::chrono::duration<Rep, Period>& wait_time) const {
      wait_for(std::chrono::duration_cast<std::chrono::nanoseconds>(wait_time));
    }

    template <class DurationType>
    inline void wait_until(const std::chrono::time_point<std::chrono::system_clock, DurationType>& timeout) const {
      wait_until(std::chrono::time_point_cast<std::chrono::nanoseconds>(timeout));
    }

  private:
    inline proxy(impl& d) noexcept
        : _impl(d) {}

    proxy(const proxy&) noexcept = default;
    proxy(proxy&&) noexcept = default;

    impl& _impl;

    friend class thread;

    template <typename Fct>
    friend class callback_t;
  };

  thread() noexcept = default;

  thread(std::unique_ptr<callback> cb);

  /// @struct is_thread_callback
  /// Trait to construct a thread from a function pointer, lamda or functor.
  /// It is valid when Fct is :
  /// 1. not a thread
  /// 2. not a std::unique_ptr
  /// 3. is callable with (const proxy&)
  template <class Fct, class = void>
  struct is_thread_callback : std::false_type {};

  template <class Fct>
  struct is_thread_callback<Fct,
      std::void_t<std::enable_if_t<mts::is_different_v<Fct, thread>, std::nullptr_t>,
          std::enable_if_t<!_VMTS::is_unique_ptr_v<Fct>, std::nullptr_t>,
          decltype(std::declval<Fct>()(std::declval<const proxy&>()))>> : std::true_type {};

  template <typename Fct, std::enable_if_t<is_thread_callback<Fct>::value, std::nullptr_t> = nullptr>
  inline thread(Fct&& fct)
      : thread(std::make_unique<callback_t<Fct>>(std::forward<Fct>(fct))) {}

  thread(const thread&) = delete;

  inline thread(thread&& t) noexcept
      : _impl(std::move(t._impl)) {}

  ~thread();

  thread& operator=(const thread&) = delete;

  /// If *this still has an associated running thread (i.e. joinable() == true), call std::terminate().
  /// Otherwise, assigns the state of other to *this and sets other to a default constructed state.
  thread& operator=(thread&& t);

  void notify();

  void signal_stop();

  void join();

  bool stop(const std::chrono::nanoseconds& wait_ns);

  template <class Rep, class Period>
  inline bool stop(const std::chrono::duration<Rep, Period>& wait_time) {
    return stop(std::chrono::duration_cast<std::chrono::nanoseconds>(wait_time));
  }

  inline bool joinable() const noexcept { return (bool)_impl; }

  bool running() const noexcept;

  bool should_stop() const noexcept;

  std::thread::id id() const noexcept;

  handle native_handle() const noexcept;

private:
  std::shared_ptr<impl> _impl;
};

MTS_END_NAMESPACE
