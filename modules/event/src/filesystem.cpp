#include "mts/event/filesystem.h"
#include "event_private.h"

#include <array>

MTS_BEGIN_NAMESPACE

void file_handle::close() const {
  if (fd > 0) {
    uv_fs_t close_req;
    // synchronous
    uv_fs_close(nullptr, &close_req, fd, nullptr);
  }
}

class open_file_task::request_impl : public event::request_base {
public:
  request_impl(io_context& ctx, std::unique_ptr<open_file_task> __task)
      : request_base(ctx)
      , _task(std::move(__task)) {
    request()->data = this;
  }

  virtual ~request_impl() override { uv_fs_req_cleanup(&_req); }

  virtual type* request() override { return (type*)&_req; }

  virtual void start() override {
    uv_fs_open(&context().loop(), (uv_fs_t*)request(), _task->_filepath.c_str(), _task->_flags, 0, &request_impl::run);
  }

private:
  uv_fs_t _req;
  std::unique_ptr<open_file_task> _task;

  static void run(uv_fs_t* req) { ((request_impl*)(req->data))->internal_run(req); }

  void internal_run(uv_fs_t* req) {
    if (req->result < 0) {
      _task->run(context(), file_handle{ -1, event::to_error_code(req->result) });
    }
    else {
      _task->run(context(), file_handle{ (int)req->result, std::error_code() });
    }

    context().remove_request(id());
  }
};
class read_file_task::request_impl : public event::request_base {
public:
  request_impl(io_context& ctx, std::unique_ptr<read_file_task> __task)
      : request_base(ctx)
      , _task(std::move(__task)) {
    request()->data = this;
  }

  virtual ~request_impl() override { uv_fs_req_cleanup(&_req); }

  virtual type* request() override { return (type*)&_req; }

  virtual void start() override {
    _uv_buffer = uv_buf_init(_buffer.data(), _buffer.size());
    uv_fs_read(&context().loop(), (uv_fs_t*)request(), _task->_fd.fd, &_uv_buffer, 1, -1, &request_impl::run);
  }

private:
  uv_fs_t _req;
  std::unique_ptr<read_file_task> _task;
  uv_buf_t _uv_buffer;
  std::array<char, 1024> _buffer;

  static void run(uv_fs_t* req) { ((request_impl*)(req->data))->internal_run(req); }

  void internal_run(uv_fs_t* req) {
    if (req->result < 0) {
      _task->run(context(), file_handle{ _task->_fd.fd, event::to_error_code(req->result) }, mts::byte_view());
    }
    else if (req->result == 0) {
      // Done reading,
      // Should close.
      _task->run(context(), file_handle{ _task->_fd.fd, std::error_code() }, mts::byte_view());

      //            uv_fs_t close_req;
      // synchronous
      //            uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
    }
    else if (req->result > 0) {
      size_t length = req->result;
      _task->run(context(), file_handle{ _task->_fd.fd, std::error_code() }, mts::byte_view(_buffer.data(), length));
    }

    context().remove_request(id());
  }
};

class read_file_buffer_task::request_impl : public event::request_base {
public:
  request_impl(io_context& ctx, std::unique_ptr<read_file_buffer_task> __task)
      : request_base(ctx)
      , _task(std::move(__task)) {
    request()->data = this;
  }

  virtual ~request_impl() override { uv_fs_req_cleanup(&_req); }

  virtual type* request() override { return (type*)&_req; }

  virtual void start() override {
    _uv_buffer = uv_buf_init(_task->_buffer.data(), _task->_buffer.size());
    uv_fs_read(&context().loop(), (uv_fs_t*)request(), _task->_fd.fd, &_uv_buffer, 1, -1, &request_impl::run);
  }

private:
  uv_fs_t _req;
  std::unique_ptr<read_file_buffer_task> _task;
  uv_buf_t _uv_buffer;
  //  std::array<char, 1024> _buffer;

  static void run(uv_fs_t* req) { ((request_impl*)(req->data))->internal_run(req); }

  void internal_run(uv_fs_t* req) {
    if (req->result < 0) {
      _task->run(context(), file_handle{ _task->_fd.fd, event::to_error_code(req->result) }, mts::memory_range<char>());
    }
    else if (req->result == 0) {
      // Done reading,
      // Should close.
      _task->run(context(), file_handle{ _task->_fd.fd, std::error_code() }, mts::memory_range<char>());

      //            uv_fs_t close_req;
      // synchronous
      //            uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
    }
    else if (req->result > 0) {
      size_t length = req->result;
      _task->run(context(), file_handle{ _task->_fd.fd, std::error_code() },
          mts::memory_range<char>(_task->_buffer.data(), length));
    }

    context().remove_request(id());
  }
};

class file_stat_task::request_impl : public event::request_base {
public:
  request_impl(io_context& ctx, std::unique_ptr<file_stat_task> __task)
      : request_base(ctx)
      , _task(std::move(__task)) {
    request()->data = this;
  }

  virtual ~request_impl() override { uv_fs_req_cleanup(&_req); }

  virtual type* request() override { return (type*)&_req; }

  virtual void start() override {
    uv_fs_fstat(&context().loop(), (uv_fs_t*)request(), _task->_fd.fd, &request_impl::run);
  }

private:
  uv_fs_t _req;
  std::unique_ptr<file_stat_task> _task;

  static void run(uv_fs_t* req) { ((request_impl*)(req->data))->internal_run(req); }

  void internal_run(uv_fs_t* req) {
    if (req->result != 0) {
      _task->run(context(), file_handle{ _task->_fd.fd, event::to_error_code(req->result) }, fs_stat{});
    }
    else {
      fs_stat st;
      st = *(fs_stat*)&req->statbuf;
      _task->run(context(), file_handle{ _task->_fd.fd, std::error_code() }, st);
    }
    context().remove_request(id());
  }
};

void open_file_task::spawn(io_context& ctx, std::unique_ptr<open_file_task> t) {
  ctx.add_request(std::make_unique<request_impl>(ctx, std::move(t)));
}

void read_file_task::spawn(io_context& ctx, std::unique_ptr<read_file_task> t) {
  ctx.add_request(std::make_unique<request_impl>(ctx, std::move(t)));
}

void read_file_buffer_task::spawn(io_context& ctx, std::unique_ptr<read_file_buffer_task> t) {
  ctx.add_request(std::make_unique<request_impl>(ctx, std::move(t)));
}

void file_stat_task::spawn(io_context& ctx, std::unique_ptr<file_stat_task> t) {
  ctx.add_request(std::make_unique<request_impl>(ctx, std::move(t)));
}

MTS_END_NAMESPACE
