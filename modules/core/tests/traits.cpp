#include <gtest/gtest.h>
#include "mts/traits.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {
TEST(traits, remove_cvref) {
  EXPECT_TRUE((std::is_same_v<int, int>));
  EXPECT_FALSE((std::is_same_v<int, const int>));
  EXPECT_FALSE((std::is_same_v<int, int&>));
  EXPECT_FALSE((std::is_same_v<int, const int&>));

  EXPECT_TRUE((std::is_same_v<mts::remove_cvref<int>::type, int>));
  EXPECT_TRUE((std::is_same_v<mts::remove_cvref_t<int>, int>));
  EXPECT_TRUE((std::is_same_v<mts::remove_cvref_t<const int>, int>));
  EXPECT_TRUE((std::is_same_v<mts::remove_cvref_t<int&>, int>));
  EXPECT_TRUE((std::is_same_v<mts::remove_cvref_t<const int&>, int>));
}

TEST(traits, is_different_type) {
  EXPECT_TRUE((mts::is_different_v<int, int&>));
  EXPECT_TRUE((mts::is_different_v<int, float>));
  EXPECT_FALSE((mts::is_different_v<int, int>));
}

TEST(traits, is_pair) {

  EXPECT_TRUE((mts::is_pair<std::pair<int, int>>::value));
  EXPECT_TRUE((mts::is_pair<std::pair<int, float>>::value));
  EXPECT_FALSE((mts::is_pair<int>::value));
}

TEST(traits, is_iterable) {
  EXPECT_TRUE((mts::is_iterable_v<std::vector<int>>));
  EXPECT_TRUE((mts::is_iterable_v<const std::vector<int>&>));
  EXPECT_TRUE((mts::is_iterable_v<std::array<int, 3>>));
  EXPECT_TRUE((mts::is_iterable_v<std::list<int>>));
  EXPECT_TRUE((mts::is_iterable_v<std::map<int, int>>));
  EXPECT_TRUE((mts::is_iterable_v<std::unordered_map<int, int>>));
  EXPECT_TRUE((mts::is_iterable_v<std::string>));

  auto isIterableRef = [](auto& it) { return mts::is_iterable_v<decltype(it)>; };
  auto isIterableConstRef = [](const auto& it) { return mts::is_iterable_v<decltype(it)>; };
  auto isConstIterableRef = [](auto& it) { return mts::is_const_iterable_v<decltype(it)>; };
  auto isConstIterableConstRef = [](const auto& it) { return mts::is_const_iterable_v<decltype(it)>; };

  struct A {
    void begin() {}
    void end() {}
  };
  EXPECT_FALSE((mts::is_iterable_v<A>));

  struct B {
    int begin() { return 0; }
    int end() { return 0; }
  };
  EXPECT_FALSE((mts::is_iterable_v<B>));

  {
    struct C {
      int* begin() { return 0; }
      int* end() { return 0; }
    };

    C c;
    const C& c2 = c;

    EXPECT_TRUE((mts::is_iterable_v<C>));
    EXPECT_FALSE((mts::is_iterable_v<const C&>));
    EXPECT_TRUE(isIterableRef(c));
    EXPECT_FALSE(isIterableConstRef(c2));
  }

  {
    struct D {
      int* begin() const { return 0; }
      int* end() const { return 0; }
    };

    D d;
    const D& d2 = d;

    EXPECT_TRUE((mts::is_iterable_v<D>));
    EXPECT_TRUE((mts::is_iterable_v<const D&>));
    EXPECT_TRUE(isIterableRef(d));
    EXPECT_TRUE(isIterableConstRef(d2));
  }

  {
    struct E {
      int* cbegin() const { return 0; }
      int* cend() const { return 0; }
    };

    E e;
    const E& e2 = e;

    EXPECT_TRUE((mts::is_const_iterable_v<E>));
    EXPECT_TRUE((mts::is_const_iterable_v<const E&>));
    EXPECT_FALSE(isIterableRef(e));
    EXPECT_FALSE(isIterableConstRef(e2));
    EXPECT_TRUE(isConstIterableRef(e2));
    EXPECT_TRUE(isConstIterableConstRef(e2));
  }

  // This is probably not a real live situation.
  {
    struct F {
      int* cbegin() { return 0; }
      int* cend() { return 0; }
    };

    F f;
    const F& f2 = f;

    EXPECT_FALSE((mts::is_const_iterable_v<F>));
    EXPECT_FALSE((mts::is_const_iterable_v<const F&>));
    EXPECT_FALSE(isIterableRef(f));
    EXPECT_FALSE(isIterableConstRef(f2));
    EXPECT_FALSE(isConstIterableRef(f2));
    EXPECT_FALSE(isConstIterableConstRef(f2));
  }
}

TEST(traits, is_string_view_const_convertible) {
  EXPECT_TRUE((mts::is_string_view_const_convertible_not_cchar_v<std::string>));
  EXPECT_TRUE((mts::is_string_view_const_convertible_not_cchar_v<const std::string&>));
  EXPECT_TRUE((mts::is_string_view_const_convertible_not_cchar_v<std::string_view>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<const char*>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<char[3]>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<char>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<std::array<char, 3>>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<void*>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<std::string*>));

  // Should work.
  class A {
  public:
    operator std::string_view() const { return ""; }
  };

  // Not const.
  class B {
  public:
    operator std::string_view() { return ""; }
  };

  // Not convertible to std::string_view.
  class C {
  public:
    operator const char*() const { return ""; }
  };

  // Not convertible to std::string_view.
  class D {
  public:
    operator std::string() const { return ""; }
  };

  // Should work.
  class E : public std::string {
  public:
  };

  EXPECT_TRUE((mts::is_string_view_const_convertible_not_cchar_v<A>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<B>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<C>));
  EXPECT_FALSE((mts::is_string_view_const_convertible_not_cchar_v<D>));
  EXPECT_TRUE((mts::is_string_view_const_convertible_not_cchar_v<E>));
}

class AStream {
public:
  friend std::ostream& operator<<(std::ostream& os, const AStream&) { return os; }
};

TEST(traits, has_ostream) {

  EXPECT_TRUE((mts::has_ostream_v<int>));
  EXPECT_TRUE((mts::has_ostream_v<const char*>));
  EXPECT_TRUE((mts::has_ostream_v<std::string>));
  EXPECT_TRUE((mts::has_ostream_v<std::string_view>));

  EXPECT_FALSE((mts::has_ostream_v<std::vector<int>>));

  class B {
  public:
    std::ostream& operator<<(std::ostream& os) const { return os; }
  };

  EXPECT_TRUE((mts::has_ostream_v<AStream>));
  EXPECT_FALSE((mts::has_ostream_v<B>));
}
} // namespace
