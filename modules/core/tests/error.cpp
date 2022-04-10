#include <gtest/gtest.h>
#include "mts/error.h"

namespace {
TEST(error, minimum) {
  {
    mts::error_result er;
    EXPECT_FALSE(er);
  }

  {
    mts::error_result er(mts::errc::unknown);
    EXPECT_TRUE(er);
  }
}

} // namespace
