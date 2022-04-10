#include <gtest/gtest.h>
#include "mts/stack_vector.h"
#include "mts/pointer.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {
class A {
public:
  A() = default;
  A(const A&) = default;
  A(A&&) = delete;

  A& operator=(const A&) = default;
  A& operator=(A&&) = delete;
};

class B {
public:
  B() = default;
  B(int) {}
  B(const B&) = default;
  //    B(B&&) = default;

  B& operator=(const B&) = default;
  B& operator=(B&&) = delete;
};

// TEST(stack_vector, cc) {
//     std::array<B, 1> a = {B()};
////    a[0] = B();
////  EXPECT_EQ(a.size(), 5);
//
////    B b;
////    a.push_back(std::move(b));
////    EXPECT_EQ(a.size(), 1);
//
//}

// template <class T, typename ... Args>
// using resize_t = decltype(T::resize(std::declval<Args>()...));
//
// template <class T>
// using has_resize = _VFST::is_detected<resize_t, mts::stack_vector<T, 5>, std::size_t>;

// template <class T>
// using init_t = decltype(std::declval<T>().init());

template <class T, typename... Args>
using construct_t = decltype(std::declval<T&>() = (std::move(std::declval<T&>(std::declval<Args>()...))));

template <class T, typename... Args>
using resize_t = decltype(std::declval<T>().resize(std::declval<Args>()...));

template <class T>
using has_default_resize = mts::is_detected<resize_t, mts::stack_vector<T, 5>, std::size_t>;

template <class T>
using has_copy_resize = mts::is_detected<resize_t, mts::stack_vector<T, 5>, std::size_t, const T&>;

template <class T>
using has_move_construct = mts::is_detected<construct_t, mts::stack_vector<T, 5>, mts::stack_vector<T, 5>>;

// if constexpr (has_prefix::value) {
//   using is_invocable = std::is_invocable_r<std::ostream&, decltype(Descriptor::prefix), std::ostream&>;
//   if constexpr (is_invocable::value) {
//     Descriptor::prefix(stream);
//   }

TEST(stack_vector, dadad) {

  {
    class A {
    public:
      A() = default;
      A(const A&) = default;
      A(A&&) = delete;

      A& operator=(const A&) = delete;
      A& operator=(A&&) = delete;
    };

    EXPECT_FALSE(has_default_resize<A>::value);
    EXPECT_FALSE(has_copy_resize<A>::value);
  }

  {
    class A {
    public:
      A() = default;
      A(const A&) = default;
      A(A&&) = delete;

      A& operator=(const A&) = delete;
      A& operator=(A&&) = default;
    };

    EXPECT_TRUE(has_default_resize<A>::value);
    EXPECT_FALSE(has_copy_resize<A>::value);
    //        EXPECT_FALSE(has_move_construct<A>::value);

    //      mts::stack_vector<A, 5> a;
    //      mts::stack_vector<A, 5> b = a;
    EXPECT_TRUE((std::is_copy_constructible_v<mts::stack_vector<A, 5>>));
    EXPECT_TRUE((std::is_move_constructible_v<mts::stack_vector<A, 5>>));
  }

  {
    class A {
    public:
      A() = default;
      A(const A&) = default;
      A(A&&) = default;

      A& operator=(const A&) = delete;
      A& operator=(A&&) = delete;
    };

    EXPECT_FALSE(has_default_resize<A>::value);
    EXPECT_FALSE(has_copy_resize<A>::value);
    EXPECT_TRUE((std::is_copy_constructible_v<mts::stack_vector<A, 5>>));
    EXPECT_TRUE((std::is_move_constructible_v<mts::stack_vector<A, 5>>));

    //    mts::stack_vector<A, 5> a;
    //    mts::stack_vector<A, 5> b = std::move(a);
    //        a
  }

  {
    class A {
    public:
      A() = delete;
      A(int) {}
      A(const A&) = delete;
      A(A&&) = default;

      A& operator=(const A&) = delete;
      A& operator=(A&&) = delete;
    };

    //        EXPECT_FALSE(has_default_resize<A>::value);
    //      EXPECT_FALSE(has_copy_resize<A>::value);
    //            EXPECT_FALSE((std::is_copy_constructible_v<mts::stack_vector<A, 5>>));
    //        EXPECT_FALSE((std::is_move_constructible_v<mts::stack_vector<A, 5>>));

    mts::stack_vector<A, 1> a = { A(1) };
    //          mts::stack_vector<A, 1> b = a;
    //        a
  }
}

TEST(stack_vector, hjk) {

  {
    mts::stack_vector<int, 5> a = { 23, 21, 12 };
    mts::stack_vector<int, 5> b(a);

    EXPECT_EQ(b.size(), 3);
    EXPECT_TRUE(a == b);

    EXPECT_TRUE(std::is_copy_constructible_v<int>);
  }
}

TEST(stack_vector, insert) {

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    EXPECT_EQ(a.size(), 3);

    a.insert(a.begin() + 1, 4);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 4);
    EXPECT_EQ(a[2], 2);
    EXPECT_EQ(a[3], 3);

    a.insert(a.begin(), 5);
    EXPECT_EQ(a[0], 5);
    EXPECT_EQ(a[1], 1);
    EXPECT_EQ(a[2], 4);
    EXPECT_EQ(a[3], 2);
    EXPECT_EQ(a[4], 3);
  }

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    EXPECT_EQ(a.size(), 3);

    a.insert(a.end(), 4);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
    EXPECT_EQ(a[3], 4);

    a.insert(a.end() - 1, 5);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
    EXPECT_EQ(a[3], 5);
    EXPECT_EQ(a[4], 4);
  }
}

TEST(stack_vector, copy) {

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    mts::stack_vector<int, 5> b = a;

    EXPECT_EQ(b.size(), 3);
    EXPECT_TRUE(a == b);
  }

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    mts::stack_vector<int, 5> b;
    b.push_back(8);

    b = a;
    EXPECT_EQ(b.size(), 3);
    EXPECT_TRUE(a == b);
  }
}

TEST(stack_vector, move) {

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    mts::stack_vector<int, 5> b = std::move(a);
    ;
    EXPECT_EQ(b.size(), 3);
    EXPECT_EQ(a.size(), 0);
    EXPECT_TRUE(a.end() == &a[0]);
    EXPECT_TRUE(a != b);
  }

  {
    mts::stack_vector<int, 5> a = { 1, 2, 3 };
    mts::stack_vector<int, 5> b = { 45, 5 };
    b = std::move(a);
    EXPECT_EQ(b.size(), 3);
    EXPECT_EQ(a.size(), 0);
    EXPECT_TRUE(a.end() == &a[0]);
    EXPECT_TRUE(a != b);
  }
}
} // namespace
