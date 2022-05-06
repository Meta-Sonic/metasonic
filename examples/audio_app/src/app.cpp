#include "app.h"
#include "mts/audio/wire.h"

app::app() {}

app::~app() { mts::print("APP DONE"); }

void app::init() {

  _device.init();

  _window = mts::make_managed<main_window>(&_device);

  _window->set_size({ 700, 500 });
  _window->add_to_desktop_centered();
}

audio_device::audio_device() {}

void audio_device::init() {
  if (std::error_code ec = _device.init()) {
    mts::errprint(ec.message());
    std::abort();
  }

  std::error_code ec;

  _devices = _device.get_audio_device_list(ec);
  if (ec) {
    mts::errprint(ec.message());
    std::abort();
  }

  std::size_t device = _device.get_default_output_device(ec);
  if (ec) {
    mts::errprint(ec.message());
    std::abort();
  }

  mts::audio_device_manager::stream_parameters oParams;
  oParams.device_index = device;
  oParams.channel_count = 2;
  oParams.first_channel = 0;

  // An error in the openStream() function can be detected either by
  // checking for a non-zero return value OR by a subsequent call to
  // isStreamOpen().
  std::size_t buffer_size = 512;
  if (mts::error_result ec = _device.open_stream(
          &oParams, nullptr, mts::audio_device_format::float32, 44100, buffer_size, &audio_device::scallback, this)) {
    mts::errprint(ec.message());
    std::abort();
  }

  if (!_device.is_stream_open()) {
    mts::errprint("SALKJSKALJSKLAJSA");
    std::abort();
  }

  _output_buffer.reset(buffer_size, 2);

  if (std::error_code ec = _device.start_stream()) {
    mts::errprint(ec.message());
    std::abort();
  }
}

void audio_device::set_output_device(std::size_t index, std::size_t sample_rate, std::size_t buffer_size) {
  std::error_code ec;
  std::size_t device = _device.get_default_output_device(ec);
  if (ec) {
    mts::errprint(ec.message());
    std::abort();
  }

  if (_device.is_stream_open()) {
    _device.stop_stream();
    _device.close_stream();
  }

  mts::audio_device_manager::stream_parameters oParams;
  oParams.device_index = device;
  oParams.channel_count = 2;
  oParams.first_channel = 0;

  // An error in the openStream() function can be detected either by
  // checking for a non-zero return value OR by a subsequent call to
  // isStreamOpen().
  if (mts::error_result ec = _device.open_stream(&oParams, nullptr, mts::audio_device_format::float32, sample_rate,
          buffer_size, &audio_device::scallback, this)) {
    mts::errprint(ec.message());
    std::abort();
  }

  if (!_device.is_stream_open()) {
    mts::errprint("SALKJSKALJSKLAJSA");
    std::abort();
  }

  _output_buffer.reset(buffer_size, 2);

  if (std::error_code ec = _device.start_stream()) {
    mts::errprint(ec.message());
    std::abort();
  }
}

mts::audio_device_manager::callback_result audio_device::callback(mts::interleaved_audio_bus<float> output,
    mts::interleaved_audio_bus<const float> input, std::size_t buffer_size, double streamTime,
    mts::audio_device_manager::stream_status status) {

  static float phase = 0;
  for (std::size_t i = 0; i < buffer_size; i++) {
    float value = std::sin(2.0f * M_PI * 400.0f * phase / 44100.0f);

    phase++;
    if (phase >= 44100) {
      phase = 0;
    }

    _output_buffer[0][i] = value;
    _output_buffer[1][i] = value;
  }

  if (status != mts::audio_device_manager::stream_status::ok) {
    mts::warnprint("Stream underflow detected!");
  }

  mts::vec::copy(_output_buffer[0], 1, output.data(), 2, buffer_size);
  mts::vec::copy(_output_buffer[1], 1, output.data() + 1, 2, buffer_size);

  return mts::audio_device_manager::callback_result::ok;
}
