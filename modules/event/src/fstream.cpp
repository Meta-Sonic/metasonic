#include "mts/event/fstream.h"
#include "event_private.h"

#include <array>

MTS_BEGIN_NAMESPACE
bool fstream::open(const mts::filesystem::path& filepath) {
  _path = filepath;

  uv_fs_t req;
  uv_fs_open(nullptr, &req, _path.c_str(), O_RDONLY, 0, nullptr);
  if (req.result < 0) {
    _handle = file_handle{ -1, event::to_error_code(req.result) };
    return false;
  }

  _handle.fd = req.result;
  return true;
}

void fstream::open(io_context& ctx, const mts::filesystem::path& filepath, std::function<void(fstream&)> callback) {
  _path = filepath;
  ctx.spawn<mts::open_file_task>(
      _path.c_str(), O_RDONLY, [this, cb = std::move(callback)](mts::io_context& ctx, const mts::file_handle& fd) {
        mts::unused(ctx);
        mts::print("opp::run");
        _handle = fd;

        cb(*this);
      });
}

std::size_t fstream::read(mts::memory_range<char> buffer) {
  uv_buf_t uv_buffer = uv_buf_init(buffer.data(), buffer.size());
  uv_fs_t req;
  //    v_fs_read(&context().loop(), (uv_fs_t*)request(), _task->_fd.fd, &_uv_buffer, 1, -1, &request_impl::run);
  uv_fs_read(nullptr, &req, _handle.fd, &uv_buffer, 1, -1, nullptr);

  if (req.result < 0) {
    //      _task->run(context(), file_handle{ _task->_fd.fd, event::to_error_code(req->result) }, mts::byte_view());
    return 0;
  }
  else if (req.result == 0) {
    // Done reading,
    // Should close.
    return 0;
  }
  else {
    size_t length = req.result;
    return length;
  }
}

void fstream::read(
    io_context& ctx, mts::memory_range<char> buffer, std::function<void(fstream&, mts::memory_range<char>)> callback) {
  ctx.spawn<read_file_buffer_task>(
      _handle, buffer, [this, cb = callback](io_context& ctx, const file_handle& fd, mts::memory_range<char> data) {
        mts::unused(ctx);
        _handle = fd;
        cb(*this, mts::memory_range<char>(data.data(), data.size()));
      });
}
MTS_END_NAMESPACE
