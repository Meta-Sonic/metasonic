#include <gtest/gtest.h>
#include "mts/thread.h"
#include "mts/waitable.h"

namespace {

void thread_join(int& count) {
  int value = 1;

  mts::thread t([&](const mts::thread::proxy& p) {
    //        mts::set_thread_name(p.native_handle(), "KL");
    p.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(value, 2);
    count++;
  });

  value = 2;
  t.notify();
  t.join();
}

TEST(thread, wait) {
  int count = 0;
  for (int i = 0; i < 1000; i++) {
    thread_join(count);
  }

  EXPECT_EQ(count, 1000);
}

void fct(mts::thread::proxy p) {}
void fct2(const mts::thread::proxy& p) {}

TEST(thread, construct) {
  {
    mts::thread t = [](const mts::thread::proxy& p) {};
    t.join();
  }

  {
    mts::thread t = [](mts::thread::proxy p) {};
    t.join();
  }

  {
    std::unique_ptr<int> a;
    mts::thread t = [b = std::move(a)](mts::thread::proxy p) {};
    t.join();
  }

  {
    mts::thread t = [](auto p) {};
    t.join();
  }

  {
    mts::thread t = [](auto p) mutable {};
    t.join();
  }

  {
    mts::thread t = &fct;
    t.join();
  }

  {
    mts::thread t = &fct2;
    t.join();
  }

  {
    mts::thread t = std::function<void(const mts::thread::proxy&)>([](const mts::thread::proxy& p) {});
    t.join();
  }

  {
    mts::thread t = std::function<void(mts::thread::proxy)>([](const mts::thread::proxy& p) {});
    t.join();
  }

  {
    struct A {
      inline void operator()(const mts::thread::proxy& p) const {}
    };

    A a;
    mts::thread t = std::bind(&A::operator(), &a, std::placeholders::_1);
    t.join();
  }

  {
    struct A {
      inline void operator()(const mts::thread::proxy& p) const {}
    };

    mts::thread t = A();
    t.join();
  }

  {
    struct A {
      inline void operator()(const mts::thread::proxy& p) const {}
    };

    A a;
    const A& b = a;
    mts::thread t = b;
    t.join();
  }

  {
    struct A {
      inline void operator()(mts::thread::proxy p) const {}
    };

    A a;
    mts::thread t = a;
    t.join();
  }

  {
    struct A {
      static inline void call(const mts::thread::proxy& p) {}
    };

    mts::thread t = &A::call;
    t.join();
  }

  {
    struct A {
      static inline void call(mts::thread::proxy p) {}
    };

    mts::thread t = &A::call;
    t.join();
  }
}

TEST(thread, move) {

  int value = 1;
  int ret = 0;

  mts::thread t1 = [&](mts::thread::proxy p) {
    p.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(value, 2);
    ret = 33;
  };

  value = 2;
  t1.notify();

  mts::thread t2 = std::move(t1);
  EXPECT_FALSE(t1.joinable());
  EXPECT_TRUE(t2.joinable());
  t2.join();
  EXPECT_EQ(ret, 33);
  EXPECT_FALSE(t2.joinable());
}

TEST(thread, signal_stop) {

  int ret = 0;
  mts::waitable<> w;
  mts::thread t = [&](mts::thread::proxy p) {
    w.notify();

    while (!p.should_stop()) {
      p.wait_for(std::chrono::seconds(1));
    }

    if (p.should_stop()) {
      ret = 33;
    }
  };

  w.wait();
  t.signal_stop();
  t.join();
  EXPECT_EQ(ret, 33);
  EXPECT_FALSE(t.joinable());
}
} // namespace
