#include <gtest/gtest.h>
#include "mts/util.h"
#include "mts/pointer.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {
TEST(util, minimum) {
  EXPECT_EQ(mts::minimum(0, 1, 2), 0);
  EXPECT_EQ(mts::minimum(2, 1, 0), 0);
  EXPECT_EQ(mts::minimum(0, -1, 2), -1);
}

} // namespace
