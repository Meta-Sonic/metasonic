#include <gtest/gtest.h>
#include "mts/audio/buffer.h"
#include "mts/audio/bus.h"
#include <array>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

namespace {
TEST(audio_interleaved_buffer, simple) {
  mts::interleaved_audio_buffer<float> data(3, 2);

  EXPECT_EQ(data.channel_size(), 2);
  EXPECT_EQ(data.buffer_size(), 3);

  data[0] = 1;
  data[1] = 2;
  data[2] = 3;
  data[3] = 4;
  data[4] = 5;
  data[5] = 6;

  EXPECT_EQ(data[0], 1);
  EXPECT_EQ(data[1], 2);
  EXPECT_EQ(data[2], 3);
  EXPECT_EQ(data[3], 4);
  EXPECT_EQ(data[4], 5);
  EXPECT_EQ(data[5], 6);

  {
    mts::interleaved_audio_bus<float> bus(data);
    EXPECT_EQ(bus.channel_size(), 2);
    EXPECT_EQ(bus.buffer_size(), 3);
    EXPECT_EQ(bus[0], 1);
    EXPECT_EQ(bus[1], 2);
    EXPECT_EQ(bus[2], 3);
    EXPECT_EQ(bus[3], 4);
    EXPECT_EQ(bus[4], 5);
    EXPECT_EQ(bus[5], 6);
  }
}

TEST(audio_buffer, simple) {
  using buffer_type = mts::audio_buffer<float>;
  buffer_type data(3, 2);
  buffer_type::buffer_pointer buffers = data.data();

  data[0][0] = 1;
  data[0][1] = 2;
  data[0][2] = 3;

  data[1][0] = 4;
  data[1][1] = 5;
  data[1][2] = 6;

  EXPECT_EQ(data[0][0], 1);
  EXPECT_EQ(data[0][1], 2);
  EXPECT_EQ(data[0][2], 3);

  EXPECT_EQ(data[1][0], 4);
  EXPECT_EQ(data[1][1], 5);
  EXPECT_EQ(data[1][2], 6);
}

TEST(audio_buffer, ccc) {
  mts::audio_buffer<float> data1(3, 2);
  //    mts::audio::buffer<const float&> data2(3, 2);

  //  buffer_type::buffer_pointer buffers = data.data();
  //
  //  data[0][0] = 1;
  //  data[0][1] = 2;
  //  data[0][2] = 3;
  //
  //  data[1][0] = 4;
  //  data[1][1] = 5;
  //  data[1][2] = 6;
  //
  //  EXPECT_EQ(data[0][0], 1);
  //  EXPECT_EQ(data[0][1], 2);
  //  EXPECT_EQ(data[0][2], 3);
  //
  //  EXPECT_EQ(data[1][0], 4);
  //  EXPECT_EQ(data[1][1], 5);
  //  EXPECT_EQ(data[1][2], 6);
}

TEST(audio_bus, fixed_size) {
  using buffer_type = mts::audio_buffer<float>;
  buffer_type data(3, 2);
  data[0][0] = 1;
  data[0][1] = 2;
  data[0][2] = 3;

  // Mono.
  {
    mts::audio_bus<float> bus(data);
    mts::audio_bus<float, 2> stereo1(bus);

    mts::audio_bus<float, 1> mono1(bus);
    mts::audio_bus<float, 1> mono2(data);
    mts::audio_bus<float, 1> mono3(stereo1);
    mts::audio_bus<const float, 1> cmono1(mono1);
    mts::audio_bus<const float, 1> cmono2(data);
    mts::audio_bus<const float, 1> cmono3(stereo1);

    EXPECT_EQ(bus.channel_size(), 2);
    EXPECT_EQ(mono1.channel_size(), 1);
    EXPECT_EQ(mono2.channel_size(), 1);
    EXPECT_EQ(mono3.channel_size(), 1);
    EXPECT_EQ(cmono1.channel_size(), 1);
    EXPECT_EQ(cmono2.channel_size(), 1);
    EXPECT_EQ(cmono3.channel_size(), 1);

    EXPECT_EQ(bus[0][0], 1);
    EXPECT_EQ(mono1[0][0], 1);
    EXPECT_EQ(mono2[0][0], 1);
    EXPECT_EQ(mono3[0][0], 1);
    EXPECT_EQ(cmono1[0][0], 1);
    EXPECT_EQ(cmono2[0][0], 1);
    EXPECT_EQ(cmono3[0][0], 1);
  }

  // Stereo.
  {
    mts::audio_bus<float> bus(data);
    mts::audio_bus<float, 2> stereo1(bus);
    mts::audio_bus<float, 2> stereo2(data);
    mts::audio_bus<const float, 2> cstereo1(stereo1);
    mts::audio_bus<const float, 2> cstereo2(data);

    EXPECT_EQ(bus.channel_size(), 2);
    EXPECT_EQ(stereo1.channel_size(), 2);
    EXPECT_EQ(stereo2.channel_size(), 2);
    EXPECT_EQ(cstereo1.channel_size(), 2);
    EXPECT_EQ(cstereo2.channel_size(), 2);

    EXPECT_EQ(bus[0][0], 1);
    EXPECT_EQ(stereo1[0][0], 1);
    EXPECT_EQ(stereo2[0][0], 1);
    EXPECT_EQ(cstereo1[0][0], 1);
    EXPECT_EQ(cstereo2[0][0], 1);
  }

  {
    mts::audio_bus<float, 2> stereo(data);
    mts::audio_bus<float> bus1(stereo.data(), stereo.buffer_size(), stereo.channel_size());
    mts::audio_bus<float> bus2(stereo.data(), stereo.buffer_size(), 1);
    EXPECT_EQ(bus1.channel_size(), 2);
    EXPECT_EQ(bus1[0][0], 1);

    EXPECT_EQ(bus2.channel_size(), 1);
    EXPECT_EQ(bus2[0][0], 1);
  }

  {
    mts::audio_bus<float, 1> mono(data);
    mts::audio_bus<float> bus(mono.data(), mono.buffer_size(), mono.channel_size());
    EXPECT_EQ(bus.channel_size(), 1);
    EXPECT_EQ(bus[0][0], 1);
  }
}

TEST(audio_bus, assign) {
  using buffer_type = mts::audio_buffer<float>;
  buffer_type data(3, 2);
  data[0][0] = 1;
  data[0][1] = 2;
  data[0][2] = 3;

  data[1][0] = -1;
  data[1][1] = -2;
  data[1][2] = -3;

  mts::audio_bus<float> bus(data);

  EXPECT_EQ(bus[0][0], 1);
  EXPECT_EQ(bus[0][1], 2);
  EXPECT_EQ(bus[0][2], 3);

  EXPECT_EQ(bus[1][0], -1);
  EXPECT_EQ(bus[1][1], -2);
  EXPECT_EQ(bus[1][2], -3);

  bus[0][0] = 4;
  bus[0][1] = 5;
  bus[0][2] = 6;

  bus[1][0] = -4;
  bus[1][1] = -5;
  bus[1][2] = -6;

  EXPECT_EQ(data[0][0], 4);
  EXPECT_EQ(data[0][1], 5);
  EXPECT_EQ(data[0][2], 6);

  EXPECT_EQ(data[1][0], -4);
  EXPECT_EQ(data[1][1], -5);
  EXPECT_EQ(data[1][2], -6);
}

} // namespace
