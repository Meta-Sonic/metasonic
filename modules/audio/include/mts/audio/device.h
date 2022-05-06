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
#include <string>
#include <vector>

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

struct audio_device_info {
  std::string name;
  std::string manufacturer;
  std::vector<std::size_t> sample_rates;
  std::size_t current_sample_rate = 0;
  std::size_t preferred_sample_rate = 0;
  std::size_t output_channels = 0;
  std::size_t input_channels = 0;
  std::size_t duplex_channels = 0;
  bool is_probed = false;
  bool is_default_output = false;
  bool is_default_input = false;
  audio_device_format native_formats = audio_device_format::unknown; /// Bit mask of supported data formats.
};

class audio_device_manager {
public:
  enum class engine_type { system_default, core_audio };
  using device_format = audio_device_format;
  using device_info = audio_device_info;

  enum class callback_result { ok, stop_and_drain, abort };

  enum class stream_status {
    ok = mts::get_flag_value(0),
    input_overflow = mts::get_flag_value(1), // Input data was discarded because of an overflow condition at the driver.
    output_underflow = mts::get_flag_value(2) // The output buffer ran low, likely causing a gap in the output sound.

  };

  struct stream_parameters {
    std::size_t device_id = 0; /*!< Device index (0 to getDeviceCount() - 1). */
    std::size_t channel_count = 0; /*!< Number of channels. */
    std::size_t first_channel = 0; /*!< First channel index on device (default = 0). */
  };

  typedef callback_result (*callback)(void* output_buffer, void* input_buffer, std::size_t buffer_size,
      double streamTime, stream_status status, void* userData);

  audio_device_manager();
  ~audio_device_manager();

  std::error_code init();

  std::size_t get_audio_device_count(std::error_code& ec);
  std::size_t get_default_input_device(std::error_code& ec);
  std::size_t get_default_output_device(std::error_code& ec);

  device_info get_audio_device_info(std::size_t index, std::error_code& ec);
  std::vector<device_info> get_audio_device_list(std::error_code& ec);

  //! A public function for opening a stream with the specified parameters.
  /*!
    An RTAUDIO_SYSTEM_ERROR is returned if a stream cannot be
    opened with the specified parameters or an error occurs during
    processing.  An RTAUDIO_INVALID_USE is returned if a stream
    is already open or any invalid stream parameters are specified.

    \param outputParameters Specifies output stream parameters to use
           when opening a stream, including a device ID, number of channels,
           and starting channel number.  For input-only streams, this
           argument should be NULL.  The device ID is an index value between
           0 and getDeviceCount() - 1.
    \param inputParameters Specifies input stream parameters to use
           when opening a stream, including a device ID, number of channels,
           and starting channel number.  For output-only streams, this
           argument should be NULL.  The device ID is an index value between
           0 and getDeviceCount() - 1.
    \param format An RtAudioFormat specifying the desired sample data format.
    \param sampleRate The desired sample rate (sample frames per second).
    \param bufferFrames A pointer to a value indicating the desired
           internal buffer size in sample frames.  The actual value
           used by the device is returned via the same pointer.  A
           value of zero can be specified, in which case the lowest
           allowable value is determined.
    \param callback A client-defined function that will be invoked
           when input data is available and/or output data is needed.
    \param userData An optional pointer to data that can be accessed
           from within the callback function.
    \param options An optional pointer to a structure containing various
           global stream options, including a list of OR'ed RtAudioStreamFlags
           and a suggested number of stream buffers that can be used to
           control stream latency.  More buffers typically result in more
           robust performance, though at a cost of greater latency.  If a
           value of zero is specified, a system-specific median value is
           chosen.  If the RTAUDIO_MINIMIZE_LATENCY flag bit is set, the
           lowest allowable value is used.  The actual value used is
           returned via the structure argument.  The parameter is API dependent.
  */

  mts::error_result open_stream(stream_parameters* output_params, stream_parameters* input_params, device_format format,
      std::size_t sample_rate, std::size_t& buffer_size, callback cb, void* user_data = nullptr);

  //! A function that closes a stream and frees any associated stream memory.
  /*!
    If a stream is not open, an RTAUDIO_WARNING will be passed to the
    user-provided errorCallback function (or otherwise printed to
    stderr).
  */
  void close_stream();

  //! A function that starts a stream.
  /*!
    An RTAUDIO_SYSTEM_ERROR is returned if an error occurs during
    processing. An RTAUDIO_WARNING is returned if a stream is not open
    or is already running.
  */
  std::error_code start_stream();

  //! Stop a stream, allowing any samples remaining in the output queue to be played.
  /*!
    An RTAUDIO_SYSTEM_ERROR is returned if an error occurs during
    processing.  An RTAUDIO_WARNING is returned if a stream is not
    open or is already stopped.
  */
  std::error_code stop_stream();

  //! Stop a stream, discarding any samples remaining in the input/output queue.
  /*!
    An RTAUDIO_SYSTEM_ERROR is returned if an error occurs during
    processing.  An RTAUDIO_WARNING is returned if a stream is not
    open or is already stopped.
  */
  std::error_code abort_stream();

  //! Returns true if a stream is open and false if not.
  bool is_stream_open() const;

  //! Returns true if the stream is running and false if it is stopped or not open.
  bool is_stream_running() const;

  //! Returns the number of seconds of processed data since the stream was started.
  /*!
    The stream time is calculated from the number of sample frames
    processed by the underlying audio system, which will increment by
    units of the audio buffer size. It is not an absolute running
    time. If a stream is not open, the returned value may not be
    valid.
  */
  double get_stream_time();

  //! Set the stream time to a time in seconds greater than or equal to 0.0.
  //    void setStreamTime( double time );

  //! Returns the internal stream latency in sample frames.
  /*!
    The stream latency refers to delay in audio input and/or output
    caused by internal buffering by the audio system and/or hardware.
    For duplex streams, the returned value will represent the sum of
    the input and output latencies.  If a stream is not open, the
    returned value will be invalid.  If the API does not report
    latency, the return value will be zero.
  */
  //    long getStreamLatency( void );

  //! Returns actual sample rate in use by the (open) stream.
  /*!
    On some systems, the sample rate used may be slightly different
    than that specified in the stream parameters.  If a stream is not
    open, a value of zero is returned.
  */
  //    unsigned int getStreamSampleRate( void );

  //! Set a client-defined function that will be invoked when an error or warning occurs.
  //    void setErrorCallback( RtAudioErrorCallback errorCallback );

  //! Specify whether warning messages should be output or not.
  /*!
    The default behaviour is for warning messages to be output,
    either to a client-defined error callback function (if specified)
    or to stderr.
  */
  //    void showWarnings( bool value = true );

  class engine;

private:
  std::unique_ptr<engine> _engine;
};

MTS_DECLARE_ENUM_CLASS_FLAGS(audio_device_format);
MTS_DECLARE_ENUM_CLASS_FLAGS(audio_device_manager::stream_status);

MTS_END_NAMESPACE

namespace std {
template <>
struct is_error_code_enum<_VMTS::audio_device_error> : std::true_type {};
inline std::error_code make_error_code(_VMTS::audio_device_error ec) noexcept { return _VMTS::make_error_code(ec); }
} // namespace std.
