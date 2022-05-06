
#pragma once
#include "mts/config.h"
#include "mts/error.h"
#include <memory>

MTS_BEGIN_NAMESPACE

namespace event {
class request_base;
class request_list;
} // namespace event.

class io_context;
using request_id = std::size_t;

struct task_base {};

// struct filesystem_result {
//   int fd;
//   std::error_code ec;
// };

MTS_END_NAMESPACE
