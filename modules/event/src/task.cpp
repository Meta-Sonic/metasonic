#include "mts/event/task.h"
#include "event_private.h"

MTS_BEGIN_NAMESPACE

class task::request_impl : public event::request_base {
public:
  inline request_impl(io_context& ctx, std::unique_ptr<task> __task)
      : request_base(ctx)
      , _task(std::move(__task)) {
    request()->data = this;
  }

  virtual ~request_impl() override = default;

  virtual type* request() override { return (type*)&_work; }

  virtual void start() override {
    uv_queue_work(&context().loop(), (uv_work_t*)request(), &request_impl::run, &request_impl::done);
  }

private:
  uv_work_t _work;
  std::unique_ptr<task> _task;

  static void run(uv_work_t* handle) {
    request_impl* rq = (request_impl*)handle->data;
    rq->_task->run(rq->context());
  }

  static void done(uv_work_t* handle, int status) {
    if (status == UV_ECANCELED) {
      mts::print("task_request UV_ECANCELED");
    }

    request_impl* rq = (request_impl*)handle->data;
    rq->context().remove_request(rq->id());
  }
};

void task::spawn(io_context& ctx, std::unique_ptr<task> t) {
  ctx.add_request(std::make_unique<request_impl>(ctx, std::move(t)));
}

MTS_END_NAMESPACE
