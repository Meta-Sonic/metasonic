#include <gtest/gtest.h>
#include "mts/meta.h"

namespace {
struct abc {
  int a;
  float b;
  std::string c;
};

struct my_combined_type {
  abc a;
  std::string b;
};

using my_meta_type = mts::meta::object<mts::meta::property<"a", int>, mts::meta::property<"b", float>,
    mts::meta::property<"c", std::string>>;

struct shit_type {
  int a;
  float b;
  std::string c;
};
} // namespace.

MTS_DECLARE_DISCOVERABLE(abc, a, b, c);
MTS_DECLARE_DISCOVERABLE(my_combined_type, a, b);

namespace {

template <mts::string_literal Key, class T, typename ValueType,
    mts::enable_if_t<mts::is_reflectable_v<T>> = mts::enabled>
void set_value_to(T& obj, ValueType&& value) {
  if constexpr (mts::meta::has<T, Key>()) {
    mts::meta::get<Key>(obj) = std::forward<ValueType>(value);
  }
}

TEST(core, meta) {
  EXPECT_TRUE(mts::is_reflectable_v<abc>);
  EXPECT_TRUE(mts::is_reflectable_v<my_combined_type>);
  EXPECT_TRUE(mts::is_reflectable_v<my_meta_type>);
  EXPECT_FALSE(mts::is_reflectable_v<int>);
  EXPECT_FALSE(mts::is_reflectable_v<shit_type>);

  EXPECT_TRUE(mts::is_discoverable_v<abc>);
  EXPECT_TRUE(mts::is_discoverable_v<my_combined_type>);
  EXPECT_FALSE(mts::is_discoverable_v<my_meta_type>);
  EXPECT_FALSE(mts::is_discoverable_v<int>);
  EXPECT_FALSE(mts::is_discoverable_v<shit_type>);

  EXPECT_FALSE(mts::is_meta_object_v<abc>);
  EXPECT_FALSE(mts::is_meta_object_v<my_combined_type>);
  EXPECT_TRUE(mts::is_meta_object_v<my_meta_type>);
  EXPECT_FALSE(mts::is_meta_object_v<int>);
  EXPECT_FALSE(mts::is_meta_object_v<shit_type>);

  EXPECT_TRUE(mts::is_meta_object_v<mts::meta::discoverable_t<abc>>);
  EXPECT_TRUE(mts::is_meta_object_v<mts::meta::discoverable_t<my_combined_type>>);

  EXPECT_TRUE((mts::meta::has<abc, "a">()));
  EXPECT_TRUE((mts::meta::has<abc, "b">()));
  EXPECT_TRUE((mts::meta::has<abc, "c">()));
  EXPECT_FALSE((mts::meta::has<abc, "d">()));

  EXPECT_TRUE((mts::meta::has<my_meta_type, "a">()));
  EXPECT_TRUE((mts::meta::has<my_meta_type, "b">()));
  EXPECT_TRUE((mts::meta::has<my_meta_type, "c">()));
  EXPECT_FALSE((mts::meta::has<my_meta_type, "d">()));

  EXPECT_TRUE((mts::meta::has_all<abc, "a", "b", "c">()));
  EXPECT_TRUE((mts::meta::has_all<my_meta_type, "a", "b", "c">()));
  EXPECT_FALSE((mts::meta::has_all<int, "a", "b", "c">()));
  EXPECT_FALSE((mts::meta::has_all<shit_type, "a", "b", "c">()));
  EXPECT_TRUE((mts::meta::has_all<my_combined_type, "a", "b">()));

  {
    my_combined_type a0;
    EXPECT_TRUE((mts::meta::has<my_combined_type, "a.a">()));
    EXPECT_TRUE((mts::meta::has_all<my_combined_type, "a", "a.b", "a.a">()));
  }

  {
    abc a0 = { 1, 2.2f, "alex" };
    EXPECT_EQ(mts::meta::get<"a">(a0), 1);
    EXPECT_EQ(mts::meta::get<"b">(a0), 2.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "alex");

    mts::meta::get<"a">(a0) = 2;
    mts::meta::get<"b">(a0) = 3.2f;
    mts::meta::get<"c">(a0) = "peter";
    EXPECT_EQ(mts::meta::get<"a">(a0), 2);
    EXPECT_EQ(mts::meta::get<"b">(a0), 3.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "peter");

    set_value_to<"a">(a0, 3);
    set_value_to<"b">(a0, 4.2f);
    set_value_to<"c">(a0, "john");
    set_value_to<"d">(a0, "abc"); // Does nothing.
    EXPECT_EQ(mts::meta::get<"a">(a0), 3);
    EXPECT_EQ(mts::meta::get<"b">(a0), 4.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "john");
  }

  {
    my_meta_type a0 = { 1, 2.2f, "alex" };
    EXPECT_EQ(mts::meta::get<"a">(a0), 1);
    EXPECT_EQ(mts::meta::get<"b">(a0), 2.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "alex");

    mts::meta::get<"a">(a0) = 2;
    mts::meta::get<"b">(a0) = 3.2f;
    mts::meta::get<"c">(a0) = "peter";
    EXPECT_EQ(mts::meta::get<"a">(a0), 2);
    EXPECT_EQ(mts::meta::get<"b">(a0), 3.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "peter");

    set_value_to<"a">(a0, 3);
    set_value_to<"b">(a0, 4.2f);
    set_value_to<"c">(a0, "john");
    set_value_to<"d">(a0, "abc"); // Does nothing.
    EXPECT_EQ(mts::meta::get<"a">(a0), 3);
    EXPECT_EQ(mts::meta::get<"b">(a0), 4.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "john");
  }

  {
    auto a0 = mts::meta::make_object<"a", "b", "c">(1, 2.2f, std::string("alex"));
    EXPECT_EQ(mts::meta::get<"a">(a0), 1);
    EXPECT_EQ(mts::meta::get<"b">(a0), 2.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "alex");

    mts::meta::get<"a">(a0) = 2;
    mts::meta::get<"b">(a0) = 3.2f;
    mts::meta::get<"c">(a0) = "peter";
    EXPECT_EQ(mts::meta::get<"a">(a0), 2);
    EXPECT_EQ(mts::meta::get<"b">(a0), 3.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "peter");

    set_value_to<"a">(a0, 3);
    set_value_to<"b">(a0, 4.2f);
    set_value_to<"c">(a0, "john");
    set_value_to<"d">(a0, "abc"); // Does nothing.
    EXPECT_EQ(mts::meta::get<"a">(a0), 3);
    EXPECT_EQ(mts::meta::get<"b">(a0), 4.2f);
    EXPECT_EQ(mts::meta::get<"c">(a0), "john");
  }

  {
    my_meta_type a0 = { 1, 2.2f, "alex" };
    float* b = mts::meta::get_as(a0, "b");
    EXPECT_NE(b, nullptr);
    *b = 89.7f;

    EXPECT_EQ(mts::meta::get<"b">(a0), 89.7f);
  }

  {
    abc a0 = { 1, 2.2f, "alex" };
    float* b = mts::meta::get_as(a0, "b");
    EXPECT_NE(b, nullptr);
    *b = 89.7f;

    EXPECT_EQ(mts::meta::get<"b">(a0), 89.7f);

    mts::meta::get<"c">(a0) = "peter";
    EXPECT_EQ(mts::meta::get<"c">(a0), "peter");
  }
}

class banana {
public:
  template <typename Obj>
  void set_value(const Obj& obj) {
    if constexpr (mts::meta::has<Obj, "a">()) {
      _a = mts::meta::get<"a">(obj);
    }
    else if constexpr (mts::meta::has<Obj, "a2">()) {
      _a = mts::meta::get<"a2">(obj) * 2;
    }
    else if constexpr (mts::meta::has<Obj, "b">()) {
      _b = mts::meta::get<"b">(obj);
    }
    else if constexpr (mts::meta::has<Obj, "b2">()) {
      _b = mts::meta::get<"b2">(obj) * 2;
    }
  }
  // private:
  int _a;
  int _b;
};

TEST(core, meta2) {
  banana b;

  b.set_value(mts::meta::make_object<"a">(32));
  EXPECT_EQ(b._a, 32);

  b.set_value(mts::meta::make_object<"a2">(32));
  EXPECT_EQ(b._a, 64);

  b.set_value(mts::meta::make_object<"b">(33));
  EXPECT_EQ(b._b, 33);

  b.set_value(mts::meta::make_object<"b2">(33));
  EXPECT_EQ(b._b, 66);
}
} // namespace
