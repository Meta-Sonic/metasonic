#include <gtest/gtest.h>
#include "mts/print.h"
#include "mts/event/io_context.h"
#include "mts/function.h"

namespace {

void task_callback(mts::io_context& ctx, std::string& str) { EXPECT_EQ(str, "task"); }

class my_class {
public:
  my_class(int) {}
  my_class(const my_class&) = delete;
  my_class(my_class&&) = delete;

  my_class& operator=(const my_class&) = delete;
  my_class& operator=(my_class&&) = delete;
  void task_callback(mts::io_context& ctx, std::string& str) { EXPECT_EQ(str, "task"); }
};

TEST(event, task) {
  std::string str = "banana";
  std::string str2 = "task";
  int counter = 0;
  mts::io_context ctx;
  ctx.spawn<mts::task>([&counter](mts::io_context& ctx) { counter++; });

  ctx.spawn<mts::task>([&counter](mts::io_context& ctx, const std::string& str) { counter++; }, std::cref(str));

  ctx.spawn<mts::task>(
      [&counter](mts::io_context& ctx, std::string& str) {
        counter++;
        str = "bingo";
      },
      std::ref(str));

  ctx.spawn<mts::task>(&task_callback, std::ref(str2));

  my_class mc(2);
  ctx.spawn<mts::task>(&my_class::task_callback, mc, std::ref(str2));

  ctx.spawn<mts::task>(mts::bind(&my_class::task_callback, &mc, std::ref(str2)));

  ctx.run();
  EXPECT_EQ(counter, 3);
  EXPECT_EQ(str, "bingo");
}
} // namespace
