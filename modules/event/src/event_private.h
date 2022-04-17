#pragma once
#include "mts/event/io_context.h"
#include "mts/print.h"
#include "mts/util.h"
#include "uv.h"

MTS_BEGIN_NAMESPACE

namespace event {
class request_base {
public:
  struct type;

  request_base(io_context& ctx);

  virtual ~request_base() = default;

  inline io_context& context() { return _ctx; }
  inline const io_context& context() const { return _ctx; }

  inline request_id id() const { return _id; }

  virtual type* request() = 0;
  virtual void start() = 0;

  void cancel();

private:
  io_context& _ctx;
  request_id _id;
};

inline std::error_code to_error_code(int err) { return std::error_code(std::abs(err), std::generic_category()); }
struct request_base::type : uv_req_t {};
} // namespace event.

struct io_context::run_loop : uv_loop_t {};

MTS_END_NAMESPACE
