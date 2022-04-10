#include <gtest/gtest.h>
#include "mts/enum_array.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {

enum class MyEnum { a, b, c, count };
TEST(enum_array, minimum) {
  using array_type = mts::enum_array<int, MyEnum>;
  array_type arr;
  array_type::const_iterator it = arr.cbegin();
}

} // namespace
