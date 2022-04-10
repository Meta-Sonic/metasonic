#include <gtest/gtest.h>
#include "mts/waitable.h"
#include <thread>

namespace {
TEST(waitable, wait) {
  int value = 1;
  mts::waitable wt;
  mts::waitable w1;
  mts::waitable w2;

  std::thread t([&]() {
    w1.wait();
    EXPECT_EQ(value, 2);

    wt.notify();

    w2.wait();
    EXPECT_EQ(value, 3);
  });

  value = 2;
  w1.notify();

  wt.wait();

  value = 3;
  w2.notify();

  t.join();
}

TEST(waitable, wait_for) {
  int value = 1;
  mts::waitable wt;
  mts::waitable w1;
  mts::waitable w2;

  std::thread t([&]() {
    w1.wait();
    EXPECT_EQ(value, 2);

    wt.notify();

    w2.wait_for(std::chrono::milliseconds(1));
    EXPECT_EQ(value, 3);
  });

  value = 2;
  w1.notify();

  wt.wait();

  std::this_thread::sleep_for(std::chrono::microseconds(1));
  value = 3;

  t.join();
}
} // namespace
