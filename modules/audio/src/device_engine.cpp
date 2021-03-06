#include "device_engine.h"
#include "mts/assert.h"
#include "mts/int24_t.h"
#include "mts/util.h"

MTS_BEGIN_NAMESPACE
audio_device_manager::engine::engine() { clear_stream_info(); }

audio_device_manager::engine::~engine() {}

void audio_device_manager::engine::tickStreamTime() {
  // Subclasses that do not provide their own implementation of
  // getStreamTime should call this function once per buffer I/O to
  // provide basic stream time support.

  _stream.streamTime += (_stream.bufferSize * 1.0 / _stream.sampleRate);

  /*
#if defined( HAVE_GETTIMEOFDAY )
  gettimeofday( &stream_.lastTickTimestamp, NULL );
#endif
  */
}

std::size_t audio_device_manager::engine::get_stream_sample_rate() const {
  return is_stream_open() ? _stream.sampleRate : 0;
}

std::size_t audio_device_manager::engine::get_stream_latency() const {
  if (!is_stream_open()) {
    return 0;
  }

  MTS_BEGIN_DISABLE_ENUM_WARNING
  switch (_stream.mode) {
  case OUTPUT:
    return _stream.latency[OUTPUT];
  case INPUT:
    return _stream.latency[INPUT];
  case DUPLEX:
    return _stream.latency[OUTPUT] + _stream.latency[INPUT];

  default:
    return 0;
  }
  MTS_END_DISABLE_ENUM_WARNING
}

void audio_device_manager::engine::setConvertInfo(stream_mode mode, std::size_t firstChannel) {
  if (mode == stream_mode::INPUT) { // convert device to user buffer
    _stream.convertInfo[mode].inJump = _stream.nDeviceChannels[1];
    _stream.convertInfo[mode].outJump = _stream.nUserChannels[1];
    _stream.convertInfo[mode].inFormat = _stream.deviceFormat[1];
    _stream.convertInfo[mode].outFormat = _stream.userFormat;
  }
  else { // convert user to device buffer
    _stream.convertInfo[mode].inJump = _stream.nUserChannels[0];
    _stream.convertInfo[mode].outJump = _stream.nDeviceChannels[0];
    _stream.convertInfo[mode].inFormat = _stream.userFormat;
    _stream.convertInfo[mode].outFormat = _stream.deviceFormat[0];
  }

  if (_stream.convertInfo[mode].inJump < _stream.convertInfo[mode].outJump)
    _stream.convertInfo[mode].channels = _stream.convertInfo[mode].inJump;
  else
    _stream.convertInfo[mode].channels = _stream.convertInfo[mode].outJump;

  // Set up the interleave/deinterleave offsets.
  if (_stream.deviceInterleaved[mode] != _stream.userInterleaved) {
    if ((mode == OUTPUT && _stream.deviceInterleaved[mode]) || (mode == INPUT && _stream.userInterleaved)) {
      for (int k = 0; k < _stream.convertInfo[mode].channels; k++) {
        _stream.convertInfo[mode].inOffset.push_back(k * _stream.bufferSize);
        _stream.convertInfo[mode].outOffset.push_back(k);
        _stream.convertInfo[mode].inJump = 1;
      }
    }
    else {
      for (int k = 0; k < _stream.convertInfo[mode].channels; k++) {
        _stream.convertInfo[mode].inOffset.push_back(k);
        _stream.convertInfo[mode].outOffset.push_back(k * _stream.bufferSize);
        _stream.convertInfo[mode].outJump = 1;
      }
    }
  }
  else { // no (de)interleaving
    if (_stream.userInterleaved) {
      for (int k = 0; k < _stream.convertInfo[mode].channels; k++) {
        _stream.convertInfo[mode].inOffset.push_back(k);
        _stream.convertInfo[mode].outOffset.push_back(k);
      }
    }
    else {
      for (int k = 0; k < _stream.convertInfo[mode].channels; k++) {
        _stream.convertInfo[mode].inOffset.push_back(k * _stream.bufferSize);
        _stream.convertInfo[mode].outOffset.push_back(k * _stream.bufferSize);
        _stream.convertInfo[mode].inJump = 1;
        _stream.convertInfo[mode].outJump = 1;
      }
    }
  }

  // Add channel offset.
  if (firstChannel > 0) {
    if (_stream.deviceInterleaved[mode]) {
      if (mode == OUTPUT) {
        for (int k = 0; k < _stream.convertInfo[mode].channels; k++)
          _stream.convertInfo[mode].outOffset[k] += firstChannel;
      }
      else {
        for (int k = 0; k < _stream.convertInfo[mode].channels; k++)
          _stream.convertInfo[mode].inOffset[k] += firstChannel;
      }
    }
    else {
      if (mode == OUTPUT) {
        for (int k = 0; k < _stream.convertInfo[mode].channels; k++)
          _stream.convertInfo[mode].outOffset[k] += (firstChannel * _stream.bufferSize);
      }
      else {
        for (int k = 0; k < _stream.convertInfo[mode].channels; k++)
          _stream.convertInfo[mode].inOffset[k] += (firstChannel * _stream.bufferSize);
      }
    }
  }
}

void audio_device_manager::engine::convertBuffer(void* outBuffer, void* inBuffer, convert_info& info) {
  // This function does format conversion, input/output channel compensation, and
  // data interleaving/deinterleaving.  24-bit integers are assumed to occupy
  // the lower three bytes of a 32-bit integer.

  // Clear our duplex device output buffer if there are more device outputs than user outputs
  if (outBuffer == _stream.deviceBuffer && _stream.mode == DUPLEX && info.outJump > info.inJump) {
    memset(outBuffer, 0, _stream.bufferSize * info.outJump * format_bytes(info.outFormat));
  }

  int j;
  if (info.outFormat == audio_device_format::float64) {
    double* out = (double*)outBuffer;

    if (info.inFormat == audio_device_format::sint8) {
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (double)in[info.inOffset[j]] / 128.0;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint16) {
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (double)in[info.inOffset[j]] / 32768.0;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (double)((int)in[info.inOffset[j]]) / 8388608.0;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (double)in[info.inOffset[j]] / 2147483648.0;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (double)in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      // Channel compensation and/or (de)interleaving only.
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
  else if (info.outFormat == audio_device_format::float32) {
    float* out = (float*)outBuffer;

    if (info.inFormat == audio_device_format::sint8) {
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (float)in[info.inOffset[j]] / 128.f;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint16) {
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (float)in[info.inOffset[j]] / 32768.f;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (float)((int)in[info.inOffset[j]]) / 8388608.f;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (float)in[info.inOffset[j]] / 2147483648.f;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      // Channel compensation and/or (de)interleaving only.
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (float)in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
  else if (info.outFormat == audio_device_format::sint32) {
    std::int32_t* out = (std::int32_t*)outBuffer;
    if (info.inFormat == audio_device_format::sint8) {
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)in[info.inOffset[j]];
          out[info.outOffset[j]] <<= 24;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint16) {
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)in[info.inOffset[j]];
          out[info.outOffset[j]] <<= 16;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)in[info.inOffset[j]];
          out[info.outOffset[j]] <<= 8;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      // Channel compensation and/or (de)interleaving only.
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          // Use llround() which returns `long long` which is guaranteed to be at least 64 bits.
          out[info.outOffset[j]]
              = (std::int32_t)std::min(std::llround(in[info.inOffset[j]] * 2147483648.f), 2147483647LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]]
              = (std::int32_t)std::min(std::llround(in[info.inOffset[j]] * 2147483648.0), 2147483647LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
  else if (info.outFormat == audio_device_format::sint24) {
    mts::int24_t* out = (mts::int24_t*)outBuffer;
    if (info.inFormat == audio_device_format::sint8) {
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)(in[info.inOffset[j]] << 16);
          // out[info.outOffset[j]] <<= 16;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint16) {
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)(in[info.inOffset[j]] << 8);
          // out[info.outOffset[j]] <<= 8;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      // Channel compensation and/or (de)interleaving only.
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)(in[info.inOffset[j]] >> 8);
          // out[info.outOffset[j]] >>= 8;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)std::min(std::llround(in[info.inOffset[j]] * 8388608.f), 8388607LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int32_t)std::min(std::llround(in[info.inOffset[j]] * 8388608.0), 8388607LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
  else if (info.outFormat == audio_device_format::sint16) {
    std::int16_t* out = (std::int16_t*)outBuffer;
    if (info.inFormat == audio_device_format::sint8) {
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int16_t)in[info.inOffset[j]];
          out[info.outOffset[j]] <<= 8;
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint16) {
      // Channel compensation and/or (de)interleaving only.
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int16_t)(((int)in[info.inOffset[j]]) >> 8);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int16_t)((in[info.inOffset[j]] >> 16) & 0x0000ffff);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int16_t)std::min(std::llround(in[info.inOffset[j]] * 32768.f), 32767LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (std::int16_t)std::min(std::llround(in[info.inOffset[j]] * 32768.0), 32767LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
  else if (info.outFormat == audio_device_format::sint8) {
    signed char* out = (signed char*)outBuffer;
    if (info.inFormat == audio_device_format::sint8) {
      // Channel compensation and/or (de)interleaving only.
      signed char* in = (signed char*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = in[info.inOffset[j]];
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    if (info.inFormat == audio_device_format::sint16) {
      std::int16_t* in = (std::int16_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (signed char)((in[info.inOffset[j]] >> 8) & 0x00ff);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint24) {
      mts::int24_t* in = (mts::int24_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (signed char)(((int)in[info.inOffset[j]]) >> 16);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::sint32) {
      std::int32_t* in = (std::int32_t*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (signed char)((in[info.inOffset[j]] >> 24) & 0x000000ff);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float32) {
      float* in = (float*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (signed char)std::min(std::llround(in[info.inOffset[j]] * 128.f), 127LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
    else if (info.inFormat == audio_device_format::float64) {
      double* in = (double*)inBuffer;
      for (std::size_t i = 0; i < _stream.bufferSize; i++) {
        for (j = 0; j < info.channels; j++) {
          out[info.outOffset[j]] = (signed char)std::min(std::llround(in[info.inOffset[j]] * 128.0), 127LL);
        }
        in += info.inJump;
        out += info.outJump;
      }
    }
  }
}

//! Protected common method used to perform byte-swapping on buffers.
void audio_device_manager::engine::byteSwapBuffer(void* buffer, std::size_t samples, audio_device_format format) {

  std::int8_t* ptr = (std::int8_t*)buffer;
  if (format == audio_device_format::sint16) {
    for (std::size_t i = 0; i < samples; i++) {
      // Swap 1st and 2nd bytes.
      std::int8_t val = *(ptr);
      *(ptr) = *(ptr + 1);
      *(ptr + 1) = val;

      // Increment 2 bytes.
      ptr += 2;
    }
  }
  else if (format == audio_device_format::sint32 || format == audio_device_format::float32) {
    for (std::size_t i = 0; i < samples; i++) {
      // Swap 1st and 4th bytes.
      std::int8_t val = *(ptr);
      *(ptr) = *(ptr + 3);
      *(ptr + 3) = val;

      // Swap 2nd and 3rd bytes.
      ptr += 1;
      val = *(ptr);
      *(ptr) = *(ptr + 1);
      *(ptr + 1) = val;

      // Increment 3 more bytes.
      ptr += 3;
    }
  }
  else if (format == audio_device_format::sint24) {
    for (std::size_t i = 0; i < samples; i++) {
      // Swap 1st and 3rd bytes.
      std::int8_t val = *(ptr);
      *(ptr) = *(ptr + 2);
      *(ptr + 2) = val;

      // Increment 2 more bytes.
      ptr += 2;
    }
  }
  else if (format == audio_device_format::float64) {
    for (std::size_t i = 0; i < samples; i++) {
      // Swap 1st and 8th bytes
      std::int8_t val = *(ptr);
      *(ptr) = *(ptr + 7);
      *(ptr + 7) = val;

      // Swap 2nd and 7th bytes
      ptr += 1;
      val = *(ptr);
      *(ptr) = *(ptr + 5);
      *(ptr + 5) = val;

      // Swap 3rd and 6th bytes
      ptr += 1;
      val = *(ptr);
      *(ptr) = *(ptr + 3);
      *(ptr + 3) = val;

      // Swap 4th and 5th bytes
      ptr += 1;
      val = *(ptr);
      *(ptr) = *(ptr + 1);
      *(ptr + 1) = val;

      // Increment 5 more bytes.
      ptr += 5;
    }
  }
}

void audio_device_manager::engine::clear_stream_info() {
  _stream.mode = UNINITIALIZED;
  _stream.state = stream_state::STREAM_CLOSED;
  _stream.sampleRate = 0;
  _stream.bufferSize = 0;
  _stream.nBuffers = 0;
  _stream.userFormat = audio_device_format::unknown;
  _stream.userInterleaved = true;
  _stream.streamTime = 0.0;
  _stream.apiHandle = 0;
  _stream.deviceBuffer = 0;
  _stream.callbackInfo.callback = nullptr;
  _stream.callbackInfo.userData = 0;
  _stream.callbackInfo.isRunning = false;
  _stream.callbackInfo.deviceDisconnected = false;

  for (int i = 0; i < 2; i++) {
    _stream.device[i] = 11111;
    _stream.doConvertBuffer[i] = false;
    _stream.deviceInterleaved[i] = true;
    _stream.doByteSwap[i] = false;
    _stream.nUserChannels[i] = 0;
    _stream.nDeviceChannels[i] = 0;
    _stream.channelOffset[i] = 0;
    _stream.deviceFormat[i] = audio_device_format::unknown;
    _stream.latency[i] = 0;
    _stream.userBuffer[i] = 0;
    _stream.convertInfo[i].channels = 0;
    _stream.convertInfo[i].inJump = 0;
    _stream.convertInfo[i].outJump = 0;
    _stream.convertInfo[i].inFormat = audio_device_format::unknown;
    _stream.convertInfo[i].outFormat = audio_device_format::unknown;
    _stream.convertInfo[i].inOffset.clear();
    _stream.convertInfo[i].outOffset.clear();
  }
}

std::size_t audio_device_manager::engine::format_bytes(audio_device_format format) {

  switch (format) {
  case audio_device_format::sint8:
    return 1;

  case audio_device_format::sint16:
    return 2;

  case audio_device_format::sint24:
    return 3;

  case audio_device_format::sint32:
    return 4;
  case audio_device_format::float32:
    return 4;
  case audio_device_format::float64:
    return 8;

  case audio_device_format::unknown:
  default:
    return 0;
  }
}

mts::error_result audio_device_manager::engine::open_stream(const stream_parameters* output_params,
    const stream_parameters* input_params, device_format format, std::size_t sample_rate, std::size_t& buffer_size,
    audio_device_callback callback, void* user_data) {

  if (_stream.state != stream_state::STREAM_CLOSED) {
    return mts::make_error_code(mts::audio_device_error::invalid_use);
  }

  // Clear stream information potentially left from a previously open stream.
  clear_stream_info();

  if (!output_params && !input_params) {
    //        errorText_ = "RtApi::openStream: input and output StreamParameters structures are both NULL!";
    return mts::make_error_code(mts::audio_device_error::invalid_use);
  }

  if (output_params && output_params->channel_count < 1) {
    //        errorText_ = "RtApi::openStream: a non-NULL output StreamParameters structure cannot have an nChannels
    //        value less than one.";
    return mts::make_error_code(mts::audio_device_error::invalid_use);
  }

  if (input_params && input_params->channel_count < 1) {
    //        errorText_ = "RtApi::openStream: a non-NULL input StreamParameters structure cannot have an nChannels
    //        value less than one.";
    return mts::make_error_code(mts::audio_device_error::invalid_use);
  }

  if (format_bytes(format) == 0) {
    //        errorText_ = "RtApi::openStream: 'format' parameter value is undefined.";
    return mts::make_error_code(mts::audio_device_error::invalid_use);
  }

  std::error_code ec;
  unsigned int nDevices = get_audio_device_count(ec);

  if (ec) {
    return ec;
  }

  unsigned int oChannels = 0;
  if (output_params) {
    oChannels = output_params->channel_count;
    if (output_params->device_index >= nDevices) {
      //          errorText_ = "RtApi::openStream: output device parameter value is invalid.";
      return mts::make_error_code(mts::audio_device_error::invalid_use);
    }
  }
  //
  unsigned int iChannels = 0;
  if (input_params) {
    iChannels = input_params->channel_count;
    if (input_params->device_index >= nDevices) {
      //          errorText_ = "RtApi::openStream: input device parameter value is invalid.";
      return mts::make_error_code(mts::audio_device_error::invalid_use);
    }
  }

  if (oChannels > 0) {

    if (mts::error_result er = probe_device_open(output_params->device_index, OUTPUT, oChannels,
            output_params->first_channel, sample_rate, format, buffer_size)) {
      return er;
    }
  }

  if (iChannels > 0) {

    if (mts::error_result er = probe_device_open(input_params->device_index, INPUT, iChannels,
            input_params->first_channel, sample_rate, format, buffer_size)) {
      return er;
    }
  }

  _stream.callbackInfo.callback = callback;
  _stream.callbackInfo.userData = user_data;

  //      if ( options ) options->numberOfBuffers = stream_.nBuffers;
  _stream.state = stream_state::STREAM_STOPPED;

  return std::error_code();
}

MTS_END_NAMESPACE
