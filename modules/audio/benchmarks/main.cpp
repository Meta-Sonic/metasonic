#include <benchmark/benchmark.h>
#include "mts/audio/buffer.h"
#include "mts/audio/audio_file.h"

namespace {
void SomeFunction(mts::byte_view fdata) {
  mts::audio_data<float> data;
  mts::wav::load_error err = mts::wav::load(fdata, data);
}
} // namespace

static void BM_SomeFunction(benchmark::State& state) {
  // Perform setup here
  mts::filesystem::path path = MTS_TEST_RESOURCES_DIRECTORY "/trumpet.wav";

  mts::file_view file;
  file.open(path);

  for (auto _ : state) {
    // This code gets timed
    SomeFunction(file.content());
  }
}

// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);

// Run the benchmark
BENCHMARK_MAIN();
