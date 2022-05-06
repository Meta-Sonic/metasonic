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
#include "mts/audio/device_manager.h"

#include <mutex>
#include <thread>

MTS_BEGIN_NAMESPACE
class audio_device_manager::engine {
public:
  engine();
  virtual ~engine();

  virtual std::error_code init() = 0;
  virtual std::size_t get_audio_device_count(std::error_code& ec) = 0;
  virtual std::size_t get_default_input_device(std::error_code& ec) = 0;
  virtual std::size_t get_default_output_device(std::error_code& ec) = 0;

  virtual device_info get_audio_device_info(audio_device_index index, std::error_code& ec) = 0;

  virtual device_info get_audio_device_info(audio_device_id device_id, std::error_code& ec) = 0;

  virtual std::vector<device_info> get_audio_device_list(std::error_code& ec) = 0;

  mts::error_result open_stream(const stream_parameters* output_params, const stream_parameters* input_params,
      device_format format, std::size_t sample_rate, std::size_t& buffer_size, audio_device_callback cb,
      void* user_data = nullptr);

  virtual void close_stream() = 0;
  virtual std::error_code start_stream() = 0;
  virtual std::error_code stop_stream() = 0;
  virtual std::error_code abort_stream() = 0;

  //    long getStreamLatency( void );
  //    unsigned int getStreamSampleRate( void );
  //    virtual double getStreamTime( void ) const { return stream_.streamTime; }
  //    virtual void setStreamTime( double time );

  std::size_t get_stream_sample_rate() const;

  std::size_t get_stream_latency() const;

  virtual double get_stream_time() const { return _stream.streamTime; }

  inline bool is_stream_open() const noexcept { return _stream.state != stream_state::STREAM_CLOSED; }
  inline bool is_stream_running() const noexcept { return _stream.state == stream_state::STREAM_RUNNING; }

  //    void setErrorCallback( RtAudioErrorCallback errorCallback ) { errorCallback_ = errorCallback; }
  //    void showWarnings( bool value ) { showWarnings_ = value; }

protected:
  enum class stream_state { STREAM_STOPPED, STREAM_STOPPING, STREAM_RUNNING, STREAM_CLOSED = -50 };
  enum stream_mode { OUTPUT, INPUT, DUPLEX, UNINITIALIZED = -75 };

  // A protected structure used for buffer conversion.
  struct convert_info {
    int channels;
    int inJump, outJump;
    device_format inFormat, outFormat;
    std::vector<int> inOffset;
    std::vector<int> outOffset;
  };

  // This global structure type is used to pass callback information
  // between the private RtAudio stream structure and global callback
  // handling functions.
  struct callback_info {
    void* object = nullptr; // Used as a "this" pointer.
    //    std::thread::native_handle_type thread{};
    audio_device_callback callback = nullptr;
    void* userData = nullptr;
    void* apiInfo = nullptr; // void pointer for API specific callback information
    bool isRunning = false;
    bool doRealtime = false;
    int priority{};
    bool deviceDisconnected = false;
  };

  // A protected structure for audio streams.
  struct audio_stream {
    unsigned int device[2]; // Playback and record, respectively.
    void* apiHandle; // void pointer for API specific stream handle information
    stream_mode mode; // OUTPUT, INPUT, or DUPLEX.
    stream_state state; // STOPPED, RUNNING, or CLOSED
    void* userBuffer[2]; // Playback and record, respectively.
    void* deviceBuffer;
    bool doConvertBuffer[2]; // Playback and record, respectively.
    bool userInterleaved;
    bool deviceInterleaved[2]; // Playback and record, respectively.
    bool doByteSwap[2]; // Playback and record, respectively.
    unsigned int sampleRate;
    unsigned int bufferSize;
    unsigned int nBuffers;
    unsigned int nUserChannels[2]; // Playback and record, respectively.
    unsigned int nDeviceChannels[2]; // Playback and record channels, respectively.
    unsigned int channelOffset[2]; // Playback and record, respectively.
    unsigned long latency[2]; // Playback and record, respectively.
    device_format userFormat;
    device_format deviceFormat[2]; // Playback and record, respectively.
    std::mutex mutex;
    callback_info callbackInfo;
    convert_info convertInfo[2];
    double streamTime; // Number of elapsed seconds since the stream started.

#if defined(HAVE_GETTIMEOFDAY)
    struct timeval lastTickTimestamp;
#endif

    audio_stream()
        : apiHandle(0)
        , deviceBuffer(0) {
      device[0] = 11111;
      device[1] = 11111;
    }
  };

  audio_stream _stream;

  void clear_stream_info();
  static std::size_t format_bytes(audio_device_format format);

  void tickStreamTime();

  //! Protected common method that sets up the parameters for buffer conversion.
  void setConvertInfo(stream_mode mode, std::size_t firstChannel);
  /*!
    Protected method used to perform format, channel number, and/or interleaving
    conversions between the user and device buffers.
  */
  void convertBuffer(void* outBuffer, void* inBuffer, convert_info& info);

  //! Protected common method used to perform byte-swapping on buffers.
  void byteSwapBuffer(void* buffer, std::size_t samples, audio_device_format format);

  /*!
    Protected, api-specific method that attempts to open a device
    with the given parameters.  This function MUST be implemented by
    all subclasses.  If an error is encountered during the probe, a
    "warning" message is reported and FAILURE is returned. A
    successful probe is indicated by a return value of SUCCESS.
  */
  virtual mts::error_result probe_device_open(std::size_t device, stream_mode mode, std::size_t channels,
      std::size_t firstChannel, std::size_t sampleRate, audio_device_format format, std::size_t& bufferSize)
      = 0;
};

class audio_engine : public audio_device_manager::engine {
public:
};
MTS_END_NAMESPACE
