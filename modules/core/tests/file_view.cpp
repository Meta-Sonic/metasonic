#include <gtest/gtest.h>
#include "mts/file_view.h"

namespace {
TEST(file_view, minimum) {
  mts::file_view fv;
  EXPECT_FALSE(fv.open(MTS_TEST_RESOURCES_DIRECTORY "/banana.txt"));
  EXPECT_EQ(fv.str(), "BANANA");
}

} // namespace
