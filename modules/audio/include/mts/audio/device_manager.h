///
/// BSD 3-Clause License
///
/// Copyright (c) 2022, Alexandre Arsenault
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// * Redistributions of source code must retain the above copyright notice, this
///   list of conditions and the following disclaimer.
///
/// * Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// * Neither the name of the copyright holder nor the names of its
///   contributors may be used to endorse or promote products derived from
///   this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#pragma once
#include "mts/config.h"
#include "mts/error.h"
#include "mts/flags.h"
#include "mts/print.h"
#include <string>
#include <vector>
#include <ostream>

MTS_BEGIN_NAMESPACE
enum class audio_device_error {
  no_device_found,
  invalid_device,
  device_disconnected,
  memory_error,
  invalid_parameter,
  invalid_use,
  driver_error,
  system_error,
  thread_error
};

const std::error_category& audio_device_error_category() noexcept;
inline std::error_code make_error_code(mts::audio_device_error ec) noexcept {
  return { static_cast<int>(ec), audio_device_error_category() };
}

enum class audio_device_format {
  unknown = mts::get_flag_value(0),
  sint8 = mts::get_flag_value(1),
  sint16 = mts::get_flag_value(2),
  sint24 = mts::get_flag_value(3),
  sint32 = mts::get_flag_value(4),
  float32 = mts::get_flag_value(5),
  float64 = mts::get_flag_value(6)
};

enum class audio_device_id : std::size_t {};

using audio_device_index = std::size_t;

struct audio_device_info {
  std::string name;
  std::string manufacturer;
  audio_device_id id;
  std::vector<std::size_t> sample_rates;
  std::size_t current_sample_rate = 0;
  std::size_t preferred_sample_rate = 0;
  std::size_t output_channels = 0;
  std::size_t input_channels = 0;
  std::size_t duplex_channels = 0;
  bool is_default_output = false;
  bool is_default_input = false;
  /// Bit mask of supported data formats.
  audio_device_format native_formats = audio_device_format::unknown;
};

enum class audio_device_stream_status {
  ok = mts::get_flag_value(0),

  /// Input data was discarded because of an overflow condition at the driver.
  input_overflow = mts::get_flag_value(1),

  /// The output buffer ran low, likely causing a gap in the output sound.
  output_underflow = mts::get_flag_value(2)
};

enum class audio_device_callback_result { ok, stop_and_drain, abort };

using audio_device_callback = audio_device_callback_result (*)( //
    void* output_buffer, //
    void* input_buffer, //
    std::size_t buffer_size, //
    double stream_time, //
    audio_device_stream_status status, //
    void* user_data //
);

class audio_device_manager {
public:
  enum class engine_type { system_default, core_audio };
  using device_format = audio_device_format;
  using device_info = audio_device_info;
  using callback_result = audio_device_callback_result;
  using stream_status = audio_device_stream_status;

  struct stream_parameters {
    /// Device index in range [0, get_audio_device_count()[.
    audio_device_index device_index = 0;

    /// Number of channels.
    std::size_t channel_count = 0;

    /// First channel index on device.
    std::size_t first_channel = 0;
  };

  audio_device_manager(engine_type etype = engine_type::system_default);
  ~audio_device_manager();

  std::error_code init();

  inline bool is_valid() const noexcept { return (bool)_engine; }

  /// @brief   Queries for the number of audio devices available.
  std::size_t get_audio_device_count(std::error_code& ec);

  ///
  std::size_t get_default_input_device(std::error_code& ec);

  ///
  std::size_t get_default_output_device(std::error_code& ec);

  // This function performs a system query of available devices each time it
  // is called, thus supporting devices connected \e after instantiation.

  /// @brief   Get the device info from specified index.
  /// @details Any device integer between 0 and get_audio_device_count() - 1 is valid.
  device_info get_audio_device_info(audio_device_index index, std::error_code& ec);

  device_info get_audio_device_info(audio_device_id device_id, std::error_code& ec);

  /// @brief   Get a list of all devices information.
  /// @warning This function performs a system query of available devices each time it called
  ///          to support new device connections during execution.
  ///          It would probably be a good idea for the caller to keep this result in a cache
  ///          and to call get_audio_device_count() before using it to make sure to count
  ///          hasn't change.
  std::vector<device_info> get_audio_device_list(std::error_code& ec);

  /// @brief   Open a stream with the specified parameters.
  /// @warning An error is returned if a stream was already opened or if any
  ///          invalid stream parameters were specified.
  /// @param   output_params
  ///          Specifies output stream parameters to use when opening a stream.
  ///          For input-only streams, this argument should be nullptr.
  /// @param   input_params
  ///          Specifies input stream parameters to use when opening a stream.
  ///          For output-only streams, this argument should be nullptr.
  /// @param   format
  ///          Format of the sample data (Should probably always be device_format::float32).
  /// @param   sample_rate
  ///          The desired sample rate (sample frames per second).
  /// @param   buffer_size
  ///          A reference to the desired internal buffer size in sample frames.
  ///          The actual value used by the device will be set.
  ///          A value of zero can be specified, in which case the lowest allowable value is determined.
  /// @param   callback
  ///          A client-defined function that will be invoked when input data is available
  ///          and/or output data is needed.
  /// @param   user_data
  ///          An optional pointer to data that can be accessed from within the callback function.
  mts::error_result open_stream(const stream_parameters* output_params, const stream_parameters* input_params,
      device_format format, std::size_t sample_rate, std::size_t& buffer_size, audio_device_callback callback,
      void* user_data = nullptr);

  inline mts::error_result open_output_stream(const stream_parameters& output_params, device_format format,
      std::size_t sample_rate, std::size_t& buffer_size, audio_device_callback callback, void* user_data = nullptr) {
    return open_stream(&output_params, nullptr, format, sample_rate, buffer_size, callback, user_data);
  }

  inline mts::error_result open_input_stream(const stream_parameters& input_params, device_format format,
      std::size_t sample_rate, std::size_t& buffer_size, audio_device_callback callback, void* user_data = nullptr) {
    return open_stream(nullptr, &input_params, format, sample_rate, buffer_size, callback, user_data);
  }

  void close_stream();

  ///
  std::error_code start_stream();

  ///
  std::error_code stop_stream();

  ///
  std::error_code abort_stream();

  ///
  bool is_stream_open() const;

  ///
  bool is_stream_running() const;

  /// @brief   Get the number of seconds of processed data since the stream started.
  double get_stream_time();

  /// @brief   Get the stream latency in sample frames.
  ///
  /// @details The stream latency refers to delay in audio input and/or output caused by
  ///          internal buffering by the audio system and/or hardware.
  ///          For duplex streams, the returned value will represent the sum of the input and output latencies.
  ///
  /// @warning If a stream is not open or the API doesn't report latency, the returned value will be zero.
  std::size_t get_stream_latency() const;

  /// @brief   Get the stream sample rate.
  ///
  /// @details On some systems, the sample rate used may be slightly different than that specified in
  ///          the stream parameters.
  ///
  /// @warning If a stream is not open, a value of zero is returned.
  std::size_t get_stream_sample_rate() const;

private:
  class engine;
  friend class audio_engine;

  std::unique_ptr<engine> _engine;
};

MTS_DECLARE_ENUM_CLASS_FLAGS(audio_device_format);
MTS_DECLARE_ENUM_CLASS_FLAGS(audio_device_stream_status);

MTS_END_NAMESPACE

namespace std {
template <>
struct is_error_code_enum<mts::audio_device_error> : std::true_type {};
inline std::error_code make_error_code(mts::audio_device_error ec) noexcept { return mts::make_error_code(ec); }

template <class CharT, class TraitsT>
inline std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& s, mts::audio_device_id id) {
  return s << (std::size_t)id;
}

inline std::ostream& operator<<(std::ostream& s, const mts::audio_device_info& device) {
  mts::basic_print<mts::equal_string>(s, "name", device.name);
  mts::basic_print<mts::equal_string>(s, "id", device.id);
  mts::basic_print<mts::equal_string>(s, "manufacturer", device.manufacturer);
  mts::basic_print<mts::equal_string>(s, "sample_rates", device.sample_rates);
  mts::basic_print<mts::equal_string>(s, "current_sample_rate", device.current_sample_rate);
  mts::basic_print<mts::equal_string>(s, "preferred_sample_rate", device.preferred_sample_rate);
  mts::basic_print<mts::equal_string>(s, "input_channels", device.input_channels);
  mts::basic_print<mts::equal_string>(s, "output_channels", device.output_channels);
  mts::basic_print<mts::equal_string>(s, "duplex_channels", device.duplex_channels);
  mts::basic_print<mts::equal_string>(s, "is_default_input", device.is_default_input);
  mts::basic_print<mts::equal_string>(s, "is_default_output", device.is_default_output);

  return s;
}

} // namespace std.
