#include "mts/event/io_context.h"
#include "event_private.h"

#include <mutex>
#include <unordered_map>

MTS_BEGIN_NAMESPACE

namespace event {
request_base::request_base(io_context& ctx)
    : _ctx(ctx)
    , _id(ctx.generate_request_id()) {}

void request_base::cancel() { uv_cancel(request()); }

//
//
//
class request_list {
public:
  template <typename _Request, std::enable_if_t<std::is_base_of_v<request_base, _Request>, bool> = true>
  inline _Request* add(std::unique_ptr<_Request> req) {
    _Request* r = req.get();
    {
      std::scoped_lock<mutex> lock(_mutex);
      _requests.emplace(req->id(), std::move(req));
    }
    return r;
  }

  inline request_base* add(std::unique_ptr<request_base> req) {
    request_base* r = req.get();
    {
      std::scoped_lock<mutex> lock(_mutex);
      _requests.emplace(req->id(), std::move(req));
    }
    return r;
  }

  inline void remove(request_id rid) {
    {
      std::scoped_lock<mutex> lock(_mutex);
      if (auto it = _requests.find(rid); it != _requests.end()) {
        _requests.erase(it);
      }
    }
  }

  void cancel_all() {
    std::scoped_lock<mutex> lock(_mutex);
    for (auto& req : _requests) {
      uv_cancel(req.second->request());
    }
  }

  void clear() {
    std::scoped_lock<mutex> lock(_mutex);
    _requests.clear();
  }

  inline std::unordered_map<request_id, std::unique_ptr<request_base>>& requests() { return _requests; }

private:
  using mutex = std::recursive_mutex;
  std::unordered_map<request_id, std::unique_ptr<request_base>> _requests;
  mutex _mutex;
};
} // namespace event.

class io_context::pimpl {
public:
  pimpl(io_context& ctx)
      : _context(ctx) {}

  void start_prepare() {
    uv_prepare_init(&_context.loop(), &_prepare);
    _prepare.data = this;
    uv_prepare_start(&_prepare, &pimpl::prepare_callback);
  }

  void stop_prepare() { uv_prepare_stop(&_prepare); }

  static void prepare_callback(uv_prepare_t* handle) { ((pimpl*)handle->data)->_context.on_prepare(); }

  io_context& _context;
  uv_prepare_t _prepare;
};

io_context::io_context() {

  _requests = std::make_unique<event::request_list>();

  _run_loop = std::make_unique<run_loop>();
  uv_loop_init(_run_loop.get());

  _pimpl = std::make_unique<pimpl>(*this);
  //    _pimpl->start_prepare();
}

io_context::~io_context() {

  uv_stop(_run_loop.get());
  _requests->cancel_all();

  while (_run_loop->active_reqs.count) {
    uv_run(_run_loop.get(), UV_RUN_NOWAIT);
  }

  // This should already be empty at this point.
  _requests->clear();

  uv_loop_close(_run_loop.get());
}

std::size_t io_context::active_requests() const { return _run_loop->active_reqs.count; }
void io_context::run() { uv_run(_run_loop.get(), UV_RUN_DEFAULT); }
void io_context::stop() { uv_stop(_run_loop.get()); }

void io_context::remove_request(request_id rid) { _requests->remove(rid); }

void io_context::add_request(std::unique_ptr<event::request_base> req) { _requests->add(std::move(req))->start(); }

void io_context::on_prepare() {}

MTS_END_NAMESPACE
