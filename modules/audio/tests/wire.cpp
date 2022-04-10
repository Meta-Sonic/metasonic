#include <gtest/gtest.h>
#include "mts/audio/wire.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <span>

namespace {
TEST(audio_wire, extent) {

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };

    mts::wire<float> w0;
    mts::wire<float> w1(vec);
    mts::wire<float> w2(w1);

    mts::wire<const float> cw0;
    mts::wire<const float> cw1(vec);
    mts::wire<const float> cw2(w1);
    mts::wire<const float> cw3(cw1);

    w0 = w1;
    w0 = w2;

    w0 = mts::wire<float>(vec);

    w0 = vec;

    float arr[] = { 6, 7, 8, 9, 10 };
    w0 = arr;
    EXPECT_EQ(w0[0], arr[0]);

    cw0 = w2;
    cw0 = cw1;
    //        mts::wire<float> w2(w1.subspan(2));
    //        EXPECT_EQ(w2.size(), 3);
    //      EXPECT_EQ(w1[0], 1);
  }

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    mts::wire<float> w1(vec);
    mts::wire<float> w2(w1.subwire(2));
    EXPECT_EQ(w2.size(), 3);
    //      EXPECT_EQ(w1[0], 1);
  }

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    mts::wire<float> w1(vec);
    mts::wire<float, 3> w2(w1.subwire(2));
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 3);
  }

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    mts::wire<const float> w1(vec);
    mts::wire<const float, 3> w2(w1.subwire(2));
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 3);

    w2 = w1.subwire(0);
    EXPECT_EQ(w2[0], 1);
  }
}

TEST(audio_wire, fixed) {
  std::vector<float> vec = { 1, 2, 3, 4, 5 };

  {
    mts::wire<float, 5> w1(vec.data());
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<float, 3> w1(vec.data());
    EXPECT_EQ(w1.size(), 3);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<float, 3> w1(vec);
    EXPECT_EQ(w1.size(), 3);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<const float, 3> w1(vec);
    EXPECT_EQ(w1.size(), 3);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<float, 3> w1(vec);
    mts::wire<const float, 3> w2(w1);
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 1);
  }

  {
    mts::wire<float> w1(vec);
    mts::wire<const float, 3> w2(w1);
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 1);
  }

  {
    mts::wire<const float> w1(vec);
    mts::wire<const float, 2> w2(w1);
    EXPECT_EQ(w2.size(), 2);
    EXPECT_EQ(w2[0], 1);
  }
}

TEST(audio_wire, simple) {

  std::vector<float> vec = { 1, 2, 3, 4, 5 };
  //

  {
    mts::wire<float> w1(vec.data(), vec.size());
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<const float> w1(vec.data(), vec.size());
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<float> w1(vec);
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<const float> w1(vec);
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);
  }

  {
    mts::wire<float> w1(vec.data(), vec.size());
    mts::wire<const float> w4(w1);
    EXPECT_EQ(w4[0], 1);
  }

  {
    mts::wire<float> w1(vec.data(), vec.size());
    mts::wire<const float> w2(w1);
    mts::wire<const float> w3(w2);
    EXPECT_EQ(w3[0], 1);
  }

  {
    mts::wire<float, 3> w1(vec);
    mts::wire<float> w2(w1);
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 1);
  }

  {
    mts::wire<float, 3> w1(vec);
    mts::wire<const float> w2(w1);
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 1);
  }

  {
    mts::wire<const float, 3> w1(vec);
    mts::wire<const float> w2(w1);
    EXPECT_EQ(w2.size(), 3);
    EXPECT_EQ(w2[0], 1);
  }
}

TEST(audio_wire, clear) {

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    mts::wire<float> w1(vec);
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);

    w1.clear();
    EXPECT_EQ(w1[0], 0);
  }
}

TEST(audio_wire, fill) {

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    mts::wire<float> w1(vec);
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);

    w1.fill(2);
    EXPECT_EQ(w1[0], 2);
  }
}

TEST(audio_wire, copy) {

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    std::vector<float> vec2(5, 0);

    mts::wire<const float> w1(vec);
    EXPECT_EQ(w1.size(), 5);
    EXPECT_EQ(w1[0], 1);

    mts::wire<float> w2(vec2);
    w2.copy(w1);
    EXPECT_EQ(w2[0], 1);

    mts::wire<float> w3(vec2);
    w3.copy(mts::wire<float>(vec));
    EXPECT_EQ(w3[0], 1);
  }

  {
    std::vector<float> vec = { 1, 2, 3, 4, 5 };
    std::vector<float> vec2(5, 0);

    mts::wire<const float, 3> w1(vec);
    EXPECT_EQ(w1.size(), 3);
    EXPECT_EQ(w1[0], 1);

    mts::wire<float> w2(vec2);
    w2.copy(w1);
    EXPECT_EQ(w2[0], 1);
    EXPECT_EQ(w2[1], 2);
    EXPECT_EQ(w2[2], 3);
    EXPECT_EQ(w2[3], 0);
    EXPECT_EQ(w2[4], 0);
  }
}
} // namespace
