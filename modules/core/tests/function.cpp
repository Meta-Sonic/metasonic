#include <gtest/gtest.h>
#include "mts/function.h"

namespace {

template <typename Fct>
void call_bingo_t(Fct fct) {
  fct(35);
}

void call_bingo(std::function<void(int)> fct) { fct(35); }

void call_foo(std::function<void(int, int)> fct) { fct(45, 46); }

template <typename Fct>
void call_bar(Fct&& fct) {
  static const std::string str = "bar";
  fct(str, 46);
}

template <typename Fct>
void call_unique(Fct&& fct) {
  fct(std::make_unique<int>(55), 46);
}

void on_my_bingo_1(int a) { EXPECT_EQ(a, 35); }

void on_my_bingo_2(int a, int b) {
  EXPECT_EQ(a, 35);
  EXPECT_EQ(b, 32);
}

class my_class {
public:
  void on_my_bingo_1(int a) { EXPECT_EQ(a, 35); }

  void on_my_bingo_2(int a, int b) {
    EXPECT_EQ(a, 35);
    EXPECT_EQ(b, 32);
  }

  void on_my_foo_1(int a, int b) {
    EXPECT_EQ(a, 45);
    EXPECT_EQ(b, 46);
  }

  void on_my_foo_2(int a, int b, std::string& str) {
    EXPECT_EQ(a, 45);
    EXPECT_EQ(b, 46);
    EXPECT_EQ(str, "foo");
    str = "bingo";
  }

  void on_bar(const std::string& str, int a) const {
    EXPECT_EQ(str, "bar");
    EXPECT_EQ(a, 46);
  }

  void on_bar_extra_1(const std::string& str, int a, int b) const {
    EXPECT_EQ(str, "bar");
    EXPECT_EQ(a, 46);
  }

  void on_unique(std::unique_ptr<int> u, int a) {
    EXPECT_EQ(*u, 55);
    EXPECT_EQ(a, 46);
  }
};

TEST(core, function) {

  call_bingo(mts::bind(&on_my_bingo_1));
  call_bingo(mts::bind(&on_my_bingo_2, 32));

  int z = 90;
  call_bingo(mts::bind(
      [&z](int a, int b) {
        EXPECT_EQ(a, 35);
        EXPECT_EQ(b, 23);
        EXPECT_EQ(z, 90);
        z = 91;
      },
      23));
  EXPECT_EQ(z, 91);

  call_bingo(mts::bind(std::function<void(int, int)>([&z](int a, int b) {
    EXPECT_EQ(a, 35);
    EXPECT_EQ(b, 23);
    EXPECT_EQ(z, 91);
    z = 92;
  }),
      23));
  EXPECT_EQ(z, 92);

  my_class mc;

  call_bingo(mts::bind(&my_class::on_my_bingo_1, &mc));
  call_bingo(mts::bind(&my_class::on_my_bingo_2, &mc, 32));

  call_foo(mts::bind<mts::after_placeholders>(&my_class::on_my_foo_1, &mc));

  std::string str = "foo";
  call_foo(mts::bind(&my_class::on_my_foo_2, &mc, std::ref(str)));
  EXPECT_EQ(str, "bingo");

  call_bingo(mts::bind<mts::before_placeholders>(
      [](int b, int a) {
        EXPECT_EQ(a, 35);
        EXPECT_EQ(b, 23);
      },
      23));

  call_bar(mts::bind([](const std::string& str, int a) {
    EXPECT_EQ(str, "bar");
    EXPECT_EQ(a, 46);
  }));

  const auto& mcc = mc;
  call_bar(mts::bind(&my_class::on_bar, &mc));
  call_bar(mts::bind(&my_class::on_bar, mc));
  call_bar(mts::bind(&my_class::on_bar, mcc));
  call_bar(mts::bind(&my_class::on_bar_extra_1, &mc, 22));

  call_unique(mts::bind(&my_class::on_unique, &mc));

  using ta = std::tuple<int, float, int>;
  using tb = mts::tuple_slice_t<0, 2, ta>;
  static_assert(std::tuple_size_v<tb> == 2, "");
}

} // namespace
