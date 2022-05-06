#pragma once
#include "mts/config.h"
#include "mts/event/common.h"
#include "mts/event/task.h"
#include "mts/event/filesystem.h"
#include <atomic>

MTS_BEGIN_NAMESPACE

class io_context {
public:
  struct run_loop;

  io_context();
  ~io_context();

  inline run_loop& loop() { return *_run_loop; }

  void run();
  void stop();

  std::size_t active_requests() const;

  void add_request(std::unique_ptr<event::request_base> req);
  void remove_request(request_id rid);

  template <typename BaseType, std::enable_if_t<std::is_base_of_v<task_base, BaseType>, bool> = true>
  void spawn(std::unique_ptr<BaseType> t) {
    BaseType::spawn(*this, std::move(t));
  }

  template <typename BaseType, typename... Args, std::enable_if_t<std::is_base_of_v<task_base, BaseType>, bool> = true>
  void spawn(Args&&... args) {
    BaseType::spawn(*this, std::forward<Args>(args)...);
  }

private:
  friend class event::request_base;
  class pimpl;

  std::unique_ptr<run_loop> _run_loop;
  std::unique_ptr<pimpl> _pimpl;
  std::unique_ptr<event::request_list> _requests;
  std::atomic<request_id> _request_id_counter = 0;

  inline request_id generate_request_id() { return _request_id_counter.fetch_add(1); }

  void on_prepare();
};

MTS_END_NAMESPACE
