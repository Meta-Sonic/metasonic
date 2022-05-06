
#pragma once
#include "mts/config.h"
#include "mts/function.h"
#include "mts/event/common.h"

MTS_BEGIN_NAMESPACE

class task : public task_base {
public:
  virtual ~task() = default;

protected:
  virtual void run(io_context& ctx) = 0;

private:
  friend class io_context;
  class request_impl;

  template <typename Fct>
  class task_t;

  static void spawn(io_context& ctx, std::unique_ptr<task> t);

  template <typename Fct, typename... Args>
  inline static void spawn(io_context& ctx, Fct&& fct, Args&&... args);
};

template <typename Fct>
class task::task_t : public task {
public:
  inline task_t(Fct&& fct)
      : _fct(std::forward<Fct>(fct)) {}

  virtual ~task_t() override = default;

protected:
  virtual void run(io_context& ctx) override { _fct(ctx); }

private:
  Fct _fct;
};

template <typename Fct, typename... Args>
inline void task::spawn(io_context& ctx, Fct&& fct, Args&&... args) {
  spawn(ctx, std::unique_ptr<task>(new task_t(mts::bind(std::forward<Fct>(fct), std::forward<Args>(args)...))));
}
MTS_END_NAMESPACE
