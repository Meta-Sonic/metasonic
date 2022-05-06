#include <gtest/gtest.h>
#include "mts/audio/vector_operations.h"
#include <vector>

namespace {
TEST(audio_vector_operations, clear) {
  {
    std::vector<int> a = { 1, 2, 3, 4, 5 };
    mts::vec::clear(a.data(), 1, a.size());
    EXPECT_EQ(a, (std::vector<int>{ 0, 0, 0, 0, 0 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    mts::vec::clear(a.data(), 2, 5);
    EXPECT_EQ(a, (std::vector<float>{ 0, 2, 0, 4, 0, 6, 0, 8, 0, 10 }));
  }
}

TEST(audio_vector_operations, fill) {
  {
    std::vector<float> a(5, 0);
    mts::vec::fill(a.data(), 1, 3.0f, a.size());
    EXPECT_EQ(a, (std::vector<float>{ 3, 3, 3, 3, 3 }));
  }

  {
    std::vector<float> a(10, 0);
    mts::vec::fill(a.data(), 2, 3.0f, 5);
    EXPECT_EQ(a, (std::vector<float>{ 3, 0, 3, 0, 3, 0, 3, 0, 3, 0 }));
  }
}

TEST(audio_vector_operations, copy) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::copy(a.data(), 1, b.data(), 1, a.size());
    EXPECT_EQ(a, b);
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(10, 0);
    mts::vec::copy(a.data(), 1, b.data(), 2, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(3, 0);
    mts::vec::copy(a.data(), 2, b.data(), 1, 3);
    EXPECT_EQ(b, (std::vector<float>{ 1, 3, 5 }));
  }

  {
    std::vector<float> a = { 1, 0, 2, 0, 3, 0, 4, 0, 5, 0 };
    std::vector<float> b(10, 0);
    mts::vec::copy(a.data(), 2, b.data(), 2, 5);
    EXPECT_EQ(b, (std::vector<float>{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0 }));
  }
}

TEST(audio_vector_operations, clip) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    mts::vec::clip(a.data(), 1, 0.0f, 3.0f, a.data(), 1, a.size());
    EXPECT_EQ(a, (std::vector<float>{ 1, 2, 3, 3, 3 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::clip(a.data(), 1, 0.0f, 3.0f, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1, 2, 3, 3, 3 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(10, 0);
    mts::vec::clip(a.data(), 1, 0.0f, 3.0f, b.data(), 2, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1, 0, 2, 0, 3, 0, 3, 0, 3, 0 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(3, 0);
    mts::vec::clip(a.data(), 2, 0.0f, 3.0f, b.data(), 1, 3);
    EXPECT_EQ(b, (std::vector<float>{ 1, 3, 3 }));
  }
}

TEST(audio_vector_operations, abs) {
  {
    std::vector<float> a = { -1, -2, -3, -4, -5 };
    mts::vec::abs(a.data(), 1, a.data(), 1, a.size());
    EXPECT_EQ(a, (std::vector<float>{ 1, 2, 3, 4, 5 }));
  }

  {
    std::vector<float> a = { -1, -2, -3, -4, -5 };
    std::vector<float> b(5, 0);
    mts::vec::abs(a.data(), 1, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1, 2, 3, 4, 5 }));
  }

  {
    std::vector<float> a = { -1, -2, -3, -4, -5 };
    std::vector<float> b(10, 0);
    mts::vec::abs(a.data(), 1, b.data(), 2, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1, 0, 2, 0, 3, 0, 4, 0, 5, 0 }));
  }

  {
    std::vector<float> a = { -1, -2, -3, -4, -5 };
    std::vector<float> b(3, 0);
    mts::vec::abs(a.data(), 2, b.data(), 1, 3);
    EXPECT_EQ(b, (std::vector<float>{ 1, 3, 5 }));
  }
}

TEST(audio_vector_operations, add) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::add(a.data(), 1, 2.0f, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 3, 4, 5, 6, 7 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(3, 0);
    mts::vec::add(a.data(), 2, 2.0f, b.data(), 1, 3);
    EXPECT_EQ(b, (std::vector<float>{ 3, 5, 7 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(10, 0);
    mts::vec::add(a.data(), 1, 2.0f, b.data(), 2, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 3, 0, 4, 0, 5, 0, 6, 0, 7, 0 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    mts::vec::add(a.data(), 1, 2.0f, a.data(), 1, a.size());
    EXPECT_EQ(a, (std::vector<float>{ 3, 4, 5, 6, 7 }));
  }

  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    mts::vec::add(a.data(), 2, 2.0f, a.data(), 2, 3);
    EXPECT_EQ(a, (std::vector<float>{ 3, 2, 5, 4, 7 }));
  }
}

TEST(audio_vector_operations, mul) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::mul(a.data(), 1, 2.0f, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1.0f * 2.0f, 2.0f * 2.0f, 3.0f * 2.0f, 4.0f * 2.0f, 5.0f * 2.0f }));
  }
}

TEST(audio_vector_operations, div) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::div(a.data(), 1, 2.0f, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 1.0f / 2.0f, 2.0f / 2.0f, 3.0f / 2.0f, 4.0f / 2.0f, 5.0f / 2.0f }));
  }
}

TEST(audio_vector_operations, vdiv) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::vdiv(2.0f, a.data(), 1, b.data(), 1, a.size());
    EXPECT_EQ(b, (std::vector<float>{ 2.0f / 1.0f, 2.0f / 2.0f, 2.0f / 3.0f, 2.0f / 4.0f, 2.0f / 5.0f }));
  }
}

TEST(audio_vector_operations, sin) {
  {
    std::vector<float> a = { 1, 2, 3, 4, 5 };
    std::vector<float> b(5, 0);
    mts::vec::sin(a.data(), b.data(), a.size());

    std::vector<float> vec{ std::sin(1.0f), std::sin(2.0f), std::sin(3.0f), std::sin(4.0f), std::sin(5.0f) };
    for (std::size_t i = 0; i < b.size(); i++) {
      EXPECT_FLOAT_EQ(b[i], vec[i]);
    }
  }
}

} // namespace
