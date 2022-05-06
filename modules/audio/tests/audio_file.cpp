#include <gtest/gtest.h>
#include "mts/print.h"
#include "mts/audio/buffer.h"
#include "mts/audio/audio_file.h"

namespace {
TEST(audio_interleaved_buffer, simple) {
  mts::filesystem::path path = MTS_TEST_RESOURCES_DIRECTORY "/trumpet.wav";

  mts::file_view fdata;
  fdata.open(path);

  mts::wav::format format;
  mts::audio_data<float> data;
  mts::wav::load_error err = mts::wav::load(fdata.content(), data, format);
  EXPECT_EQ(format, mts::wav::format::pcm_24_bit);
  //    mts::print(mts::filesystem::exists(path), mts::wav::error_to_string(err), data.sample_rate,
  //    data.buffer.buffer_size());

  mts::wav::save(MTS_TEST_RESOURCES_DIRECTORY "/trumpet2.wav", data, mts::wav::format::ieee_32_bit);
}

} // namespace
