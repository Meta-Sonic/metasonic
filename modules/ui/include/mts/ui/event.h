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
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <functional>

MTS_BEGIN_NAMESPACE
class event {
public:
  event() = default;
  virtual ~event() = default;
  virtual void call() = 0;
};

template <typename _Fct>
class generic_event : public event {
public:
  generic_event(_Fct&& fct)
      : _callback(std::move(fct)) {}

  virtual ~generic_event() override = default;

  virtual void call() override { _callback(); }

private:
  _Fct _callback;
};

class event_manager_proxy;

class event_manager {
public:
  static void post(std::shared_ptr<event> evt);

  template <typename _EventType, typename... Args>
  static inline void post(Args&&... args) {
    post(std::make_shared<_EventType>(std::forward<Args>(args)...));
  }

  template <typename Fct>
  static inline void async(Fct&& fct) {
    post(std::make_shared<generic_event<Fct>>(std::move(fct)));
  }

  static bool is_main_thread();

private:
  event_manager();
  ~event_manager();

  event_manager(const event_manager&) = delete;
  event_manager(event_manager&&) = delete;

  event_manager& operator=(const event_manager&) = delete;
  event_manager& operator=(event_manager&&) = delete;

  friend class event_manager_proxy;

  static event_manager& get_instance();

  static void set_main_thread();
  static bool has_main_thread();

  class pimpl;
  std::unique_ptr<pimpl> _pimpl;
  std::recursive_mutex _mutex;
  std::vector<std::shared_ptr<event>> _events;
  std::thread::id _main_thread_id = std::thread::id();
};

class event_manager_proxy {
protected:
  inline static event_manager& get_instance() { return event_manager::get_instance(); }

  inline static void set_main_thread() { event_manager::set_main_thread(); }

  inline static bool has_main_thread() { return event_manager::has_main_thread(); }
};
MTS_END_NAMESPACE
