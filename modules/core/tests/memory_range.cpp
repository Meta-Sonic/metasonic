#include <gtest/gtest.h>
#include "mts/memory_range.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {

TEST(memory_range, vector) {
  std::vector<float> vec = { 1, 2, 3 };
  const std::vector<float>& cvec = vec;
  mts::memory_range<float> r(vec);

  EXPECT_EQ(r[0], 1);
  EXPECT_EQ(r[1], 2);
  EXPECT_EQ(r[2], 3);

  mts::memory_range<const float> cr(cvec);

  mts::byte_range br = r.as_byte_range();
  mts::byte_view bv = r.as_byte_view();
  ;
  EXPECT_EQ(bv.size(), sizeof(float) * 3);

  float a = bv.as<float>(0);
  EXPECT_EQ(a, 1);

  const float& ar = bv.as_ref<float>(0);
  EXPECT_EQ(ar, 1);
}

TEST(memory_range, convert) {
  std::array<std::int16_t, 1> arr = { std::numeric_limits<std::int16_t>::max() };
  mts::byte_view bv(mts::memory_range<std::int16_t>(arr).as_byte_view());

  float aaa = bv.as_from<float, mts::pcm::pcm_16>(0);
  EXPECT_GE(aaa, 0.9999f);

  mts::byte_view bv2(arr.data(), arr.size());
  EXPECT_EQ(bv2.as<std::int16_t>(0), std::numeric_limits<std::int16_t>::max());

  mts::byte_view bv3(arr);
  mts::byte_view bv4(bv3);
  mts::byte_view bv5(bv);
  EXPECT_EQ(bv.as<std::int16_t>(0), std::numeric_limits<std::int16_t>::max());

  std::vector<int> vec = { 1, 2, 3 };
  mts::byte_view bv6(vec);

  std::vector<float> vecf = { 1, 2, 3 };
  mts::byte_view bv7(vecf);
}

TEST(memory_range, minimum) {
  std::array<int, 3> arr;
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  mts::memory_range<int> r(arr);

  EXPECT_EQ(r[0], 1);
  EXPECT_EQ(r[1], 2);
  EXPECT_EQ(r[2], 3);

  const std::array<int, 3>& arr2 = arr;
  mts::memory_range<const int> r2(arr2);

  mts::memory_range<const int> r3(r2);
  mts::memory_range<const int> r4(r);
}

TEST(memory_range, asdad) {
  std::array<int, 3> arr;
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  mts::memory_range<int> r(arr.data(), arr.size());
  EXPECT_EQ(r[0], 1);
  EXPECT_EQ(r[1], 2);
  EXPECT_EQ(r[2], 3);

  mts::memory_range<int>::iterator it = r.begin();
  *it = 23;
  EXPECT_EQ(r[0], 23);
  EXPECT_EQ(arr[0], 23);
  //    r[0] = 2;
}

TEST(memory_range, const_int) {
  std::array<int, 3> arr;
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  {
    mts::memory_range<const int> r(arr.data(), arr.size());
    EXPECT_EQ(r[0], 1);
    EXPECT_EQ(r[1], 2);
    EXPECT_EQ(r[2], 3);

    {
      mts::memory_range<const int>::iterator it = r.begin();
      mts::memory_range<const int>::const_iterator cit = r.begin();
    }

    {
      const mts::memory_range<const int>& rr = r;
      mts::memory_range<int>::const_iterator it = rr.begin();
    }
  }

  {
    mts::memory_range<int> r(arr.data(), arr.size());
    EXPECT_EQ(r[0], 1);
    EXPECT_EQ(r[1], 2);
    EXPECT_EQ(r[2], 3);

    {
      mts::memory_range<int>::iterator it = r.begin();
      *it = 23;
    }

    {
      const mts::memory_range<int>& rr = r;
      mts::memory_range<int>::const_iterator it = rr.begin();
    }
  }
}

} // namespace
