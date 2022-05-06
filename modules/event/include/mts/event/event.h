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

  void run();
  void stop();

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
