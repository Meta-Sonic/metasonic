#include <gtest/gtest.h>
#include "mts/graphics/size.h"
#include "mts/graphics/rect.h"
#include "mts/graphics/color.h"
#include "mts/graphics/image.h"
#include "mts/graphics/path.h"

#include "mts/ui/object.h"
#include "mts/ui/mouse_event.h"
#include "mts/ui/object.h"
#include "mts/ui/view.h"
#include "mts/print.h"

namespace {
TEST(graphics, simple) {
  mts::rect s0 = { 1, 2, 3, 4 };
  EXPECT_EQ(s0.x, 1);
  EXPECT_EQ(s0.y, 2);
  EXPECT_EQ(s0.width, 3);
  EXPECT_EQ(s0.height, 4);

  EXPECT_EQ(s0.position, mts::point<float>(1, 2));
  EXPECT_EQ(s0.size, mts::size(3, 4));

  mts::color c0 = 0x12345678;
  EXPECT_EQ(c0.red(), 0x12);
  EXPECT_EQ(c0.green(), 0x34);
  EXPECT_EQ(c0.blue(), 0x56);
  EXPECT_EQ(c0.alpha(), 0x78);
  EXPECT_EQ(c0.to_string(), "#12345678");

  mts::color::float_rgba r = { 1, 0.2, 0.3, 0.4 };
  mts::color c1 = r;
  EXPECT_EQ(c1.red(), 0xFF);
  EXPECT_EQ(c1.green(), 0.2 * 0xFF);

  mts::image img;
  mts::path pp;
}

} // namespace
