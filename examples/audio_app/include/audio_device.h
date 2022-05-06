#pragma once
#include "mts/print.h"
#include "mts/audio/device_manager.h"
#include "mts/audio/buffer.h"
#include "mts/audio/bus.h"

class audio_device {
public:
  audio_device();

  void init();

  inline const std::vector<mts::audio_device_info>& get_device_list() const { return _devices; }

  void set_output_device(std::size_t index, std::size_t sample_rate, std::size_t buffer_size);

private:
  mts::audio_device_manager _device;
  mts::audio_buffer<float> _output_buffer;
  std::vector<mts::audio_device_info> _devices;

  mts::audio_device_manager::callback_result callback(mts::interleaved_audio_bus<float> output,
      mts::interleaved_audio_bus<const float> input, std::size_t buffer_size, double streamTime,
      mts::audio_device_manager::stream_status status);

  static mts::audio_device_manager::callback_result scallback(void* outputBuffer, void* inputBuffer,
      std::size_t buffer_size, double streamTime, mts::audio_device_manager::stream_status status, void* data) {
    return ((audio_device*)data)
        ->callback(mts::interleaved_audio_bus<float>((float*)outputBuffer, buffer_size, 2),
            mts::interleaved_audio_bus<const float>((float*)inputBuffer, 0, 0), buffer_size, streamTime, status);
  }
};
