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
#include "mts/byte_vector.h"
#include "mts/filesystem.h"
#include "mts/file_view.h"
#include "mts/memory_range.h"
#include "mts/int24_t.h"
#include "mts/util.h"
#include "mts/audio/wire.h"

#include <vector>
#include <cstring>

namespace mts {

template <typename _Tp>
struct audio_data {
  using value_type = _Tp;

  mts::audio_buffer<_Tp> buffer;
  std::size_t sample_rate;
};

namespace wav {
  enum class format { unknown, pcm_8_bit, pcm_16_bit, pcm_24_bit, pcm_32_bit, ieee_32_bit, ieee_64_bit };

  ///
  /// Load wav file.
  ///

  enum class load_error {
    no_error,
    unable_to_open_file,
    invalid_file,
    invalid_format_section,
    invalid_data_section,
    unsupported_compression,
    unsupported_channel_count,
    unsupported_bit_depth,
    unsupported_format,
    inconsistent_header,
  };

  inline const char* error_to_string(load_error err);

  inline std::size_t format_to_bit_depth(format f);

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data);

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data, format& f);

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data, std::size_t maximum_loaded_samples);

  template <typename _T>
  load_error load(
      const mts::filesystem::path& file_path, audio_data<_T>& au_data, std::size_t maximum_loaded_samples, format& f);

  template <typename _T>
  load_error load(const mts::byte_view& data, audio_data<_T>& au_data);

  template <typename _T>
  load_error load(const mts::byte_view& data, audio_data<_T>& au_data, format& f);

  ///
  /// Save audio file.
  ///

  enum class save_error {
    no_error,
    unable_to_open_file,
    invalid_sampling_rate,
    empty_channel,
    empty_buffer,
    format_error,
    file_size_error,
    data_size_error,
    unsupported_bit_depth,
  };

  inline const char* error_to_string(save_error err);

  template <typename _Tp>
  save_error save(const mts::filesystem::path& file_path, const audio_data<_Tp>& data_view, format e_format);

} // namespace wav.

//
//
//
//
// Implementation.
//
//
//
//

namespace wav {
  inline std::size_t format_to_bit_depth(format f) {
    switch (f) {
    case format::unknown:
      return 0;
    case format::pcm_8_bit:
      return 8;
    case format::pcm_16_bit:
      return 16;
    case format::pcm_24_bit:
      return 24;
    case format::pcm_32_bit:
      return 32;
    case format::ieee_32_bit:
      return 32;
    case format::ieee_64_bit:
      return 64;
    }

    return 0;
  }

  inline const char* error_to_string(load_error err) {
    switch (err) {
    case load_error::no_error:
      return "";
    case load_error::unable_to_open_file:
      return "Unable to open file";
    case load_error::invalid_file:
      return "Not a wav file";
    case load_error::invalid_format_section:
      return "Invalid format section";
    case load_error::invalid_data_section:
      return "Invalid data section";
    case load_error::unsupported_compression:
      return "Unsupported compression";
    case load_error::unsupported_channel_count:
      return "Unsupported channel count";
    case load_error::unsupported_bit_depth:
      return "Unsupported bit depth";
    case load_error::unsupported_format:
      return "Unsupported format";
    case load_error::inconsistent_header:
      return "Inconsistent header";
    }

    return "";
  }

  const char* error_to_string(save_error err) {
    switch (err) {
    case save_error::no_error:
      return "";
    case save_error::unable_to_open_file:
      return "Unable to open file";
    case save_error::invalid_sampling_rate:
      return "Invalid sampling rate";
    case save_error::empty_channel:
      return "Empty channel";
    case save_error::empty_buffer:
      return "Empty buffer";
    case save_error::format_error:
      return "Format error";
    case save_error::file_size_error:
      return "File size error";
    case save_error::data_size_error:
      return "Data size error";
    case save_error::unsupported_bit_depth:
      return "Unsupported bit depth";
    }

    return "";
  }

  namespace detail {
    enum format : std::int16_t {
      pcm = 0x0001,
      ieee_float = 0x0003,
    };

    constexpr std::string_view riff_header_id = "RIFF";
    constexpr std::string_view wave_header_id = "WAVE";
    constexpr std::string_view format_header_id = "fmt ";
    constexpr std::string_view data_header_id = "data";
    constexpr std::string_view fact_header_id = "fact";

    template <typename _T, typename _ConvertType>
    inline void convert_pcm(mts::audio_buffer<_T>& buffers, const mts::byte_view& data, std::int16_t n_byte_per_block) {
      using value_type = _T;
      using wire = mts::wire<value_type>;

      std::size_t n_channel = buffers.channel_size();
      std::size_t n_samples = buffers.buffer_size();
      constexpr value_type denom = value_type(1.0) / (1L << (8L * sizeof(_ConvertType) - 1L));

      if (n_channel == 1) {
        if (n_byte_per_block == sizeof(_ConvertType)) {
          const _ConvertType* v = data.data<_ConvertType>();
          wire(buffers.channel_wire(0)).template assign_from<_ConvertType>(v) *= denom;
          return;
        }

        for (std::size_t i = 0; i < n_samples; i++) {
          buffers[0][i] = ((value_type) * (const _ConvertType*)&data[n_byte_per_block * i]) * denom;
        }

        return;
      }

      if (n_channel == 2) {
        if (n_byte_per_block == 2 * sizeof(_ConvertType)) {
          const _ConvertType* v = data.data<_ConvertType>();
          wire(buffers.channel_wire(0)).template assign_from<_ConvertType>(v, 2) *= denom;
          wire(buffers.channel_wire(1)).template assign_from<_ConvertType>(v + 1, 2) *= denom;
          return;
        }

        for (std::size_t i = 0; i < n_samples; i++) {
          std::size_t dt = (n_byte_per_block * i);
          for (std::size_t channel = 0; channel < n_channel; channel++) {
            std::size_t s_index = dt + channel * sizeof(_ConvertType);
            buffers[channel][i] = ((value_type) * (const _ConvertType*)&data[s_index]) * denom;
          }
        }

        return;
      }

      for (std::size_t i = 0; i < n_samples; i++) {
        std::size_t dt = n_byte_per_block * i;
        for (std::size_t channel = 0; channel < n_channel; channel++) {
          std::size_t s_index = dt + channel * sizeof(_ConvertType);
          buffers[channel][i] = ((value_type) * (const _ConvertType*)&data[s_index]) * denom;
        }
      }
    }

    template <typename _T, typename _ConvertType>
    inline void convert_ieee(
        mts::audio_buffer<_T>& buffers, const mts::byte_view& data, std::int16_t n_byte_per_block) {
      using value_type = _T;
      using wire = mts::wire<value_type>;

      std::size_t n_channel = buffers.channel_size();
      std::size_t n_samples = buffers.buffer_size();

      if (n_channel == 1) {
        if (n_byte_per_block == sizeof(_ConvertType)) {

          const _ConvertType* v = data.data<_ConvertType>();
          wire(buffers.channel_wire(0)).template assign_from<_ConvertType>(v);
          return;
        }

        for (std::size_t i = 0; i < n_samples; i++) {
          buffers[0][i] = *(const _ConvertType*)&data[n_byte_per_block * i];
        }

        return;
      }

      if (n_channel == 2) {
        if (n_byte_per_block == 2 * sizeof(_ConvertType)) {
          const _ConvertType* v = data.data<_ConvertType>();
          wire(buffers.channel_wire(0)).template assign_from<_ConvertType>(v, 2);
          wire(buffers.channel_wire(1)).template assign_from<_ConvertType>(v + 1, 2);
          return;
        }

        for (std::size_t i = 0; i < n_samples; i++) {
          std::size_t s_index = n_byte_per_block * i;
          buffers[0][i] = *(const _ConvertType*)&data[s_index];
          buffers[1][i] = *(const _ConvertType*)&data[s_index + sizeof(_ConvertType)];
        }

        return;
      }

      for (std::size_t i = 0; i < n_samples; i++) {
        std::size_t dt = n_byte_per_block * i;
        for (std::size_t channel = 0; channel < n_channel; channel++) {
          std::size_t s_index = dt + channel * sizeof(_ConvertType);
          buffers[channel][i] = *(const _ConvertType*)&data[s_index];
        }
      }
    }
  } // namespace detail.

  //
  //
  //
  template <typename _T>
  load_error load(
      const mts::byte_view& data, audio_data<_T>& au_data, std::size_t maximum_loaded_samples, format& _format) {

    using value_type = _T;
    using difference_type = mts::byte_view::difference_type;
    using data_type = audio_data<_T>;

    //
    // Header chunk.
    //
    if (std::string_view(data.data<char>(), detail::riff_header_id.size()) != detail::riff_header_id
        || std::string_view(data.data<char>(8), detail::wave_header_id.size()) != detail::wave_header_id) {
      return load_error::invalid_file;
    }

    // Find the start points of key chunks.
    difference_type format_chunk_index = data.find(detail::format_header_id.data(), detail::format_header_id.size());
    if (format_chunk_index == -1) {
      return load_error::invalid_format_section;
    }

    difference_type data_chunk_index = data.find(detail::data_header_id.data(), detail::data_header_id.size());
    if (data_chunk_index == -1) {
      return load_error::invalid_data_section;
    }

    //
    // Format chunk.
    //
    std::size_t f = format_chunk_index;
    std::int16_t audio_format = data.as<std::int16_t>(f + 8);
    std::int16_t n_channel = data.as<std::int16_t>(f + 10);
    std::size_t sr = static_cast<std::size_t>(data.as<std::int32_t>(f + 12));
    std::int32_t n_bytes_per_second = data.as<std::int32_t>(f + 16);
    std::int16_t n_byte_per_block = data.as<std::int16_t>(f + 20);
    std::size_t bit_depth = static_cast<std::size_t>(data.as<std::int16_t>(f + 22));

    int n_bytes_per_sample = int(bit_depth) / 8;
    format e_format = format::unknown;

    // Check that the audio format is PCM.
    if (audio_format != detail::format::pcm && audio_format != detail::format::ieee_float) {
      return load_error::unsupported_compression;
    }

    // Check the number of channels is mono or stereo.
    if (n_channel == 0) {
      return load_error::unsupported_channel_count;
    }

    // Check header data is consistent.
    if ((n_bytes_per_second != (n_channel * sr * bit_depth) / 8)
        || (n_byte_per_block != (n_channel * n_bytes_per_sample))) {
      return load_error::inconsistent_header;
    }

    // Check bit depth is either 8, 16 or 24 bit.
    if (!mts::is_one_of(bit_depth, 8, 16, 24, 32, 64)) {
      return load_error::unsupported_bit_depth;
    }

    if (audio_format == detail::format::pcm) {
      switch (bit_depth) {
      case 8:
        e_format = format::pcm_8_bit;
        break;
      case 16:
        e_format = format::pcm_16_bit;
        break;
      case 24:
        e_format = format::pcm_24_bit;
        break;
      case 32:
        e_format = format::pcm_32_bit;
        break;
      default:
        return load_error::unsupported_format;
      }
    }
    else if (audio_format == detail::format::ieee_float) {
      switch (bit_depth) {
      case 32:
        e_format = format::ieee_32_bit;
        break;
      case 64:
        e_format = format::ieee_64_bit;
        break;
      default:
        return load_error::unsupported_format;
      }
    }

    //
    // Data chunk.
    //
    std::size_t d = data_chunk_index;
    std::int32_t data_chunk_size = data.as<int32_t>(d + 4);
    std::size_t n_samples = data_chunk_size / (n_channel * bit_depth / 8);
    std::size_t samplesStartIndex = data_chunk_index + 8;

    n_samples = mts::minimum(n_samples, maximum_loaded_samples);

    mts::audio_buffer<value_type>& buffers = au_data.buffer;
    buffers.reset(n_samples, n_channel);

    using convert_options = mts::pcm::type;

    switch (e_format) {
    case format::unknown:
      return load_error::unsupported_bit_depth;

    case format::pcm_8_bit: {
      for (std::size_t i = 0; i < n_samples; i++) {
        std::size_t dt = samplesStartIndex + (n_byte_per_block * i);
        for (std::size_t channel = 0; channel < n_channel; channel++) {
          std::size_t s_index = dt + channel * n_bytes_per_sample;
          buffers[channel][i] = data.as<value_type, convert_options::pcm_8>(s_index);
        }
      }
    } break;

    case format::pcm_16_bit: {
      detail::convert_pcm<value_type, std::int16_t>(buffers, data.sub_range(samplesStartIndex), n_byte_per_block);
    } break;

    case format::pcm_24_bit: {
      detail::convert_pcm<value_type, mts::int24_t>(buffers, data.sub_range(samplesStartIndex), n_byte_per_block);
    } break;

    case format::pcm_32_bit: {
      detail::convert_pcm<value_type, std::int32_t>(buffers, data.sub_range(samplesStartIndex), n_byte_per_block);
    } break;

    case format::ieee_32_bit: {
      detail::convert_ieee<value_type, float>(buffers, data.sub_range(samplesStartIndex), n_byte_per_block);
    } break;

    case format::ieee_64_bit: {
      detail::convert_ieee<value_type, double>(buffers, data.sub_range(samplesStartIndex), n_byte_per_block);
    } break;
    }

    au_data.sample_rate = sr;

    _format = e_format;

    return load_error::no_error;
  }

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data) {
    mts::file_view file;
    if (file.open(file_path)) {
      return load_error::unable_to_open_file;
    }

    format e_format;
    return load(mts::byte_view(file.content()), au_data, std::numeric_limits<std::size_t>::max(), e_format);
  }

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data, format& e_format) {
    mts::file_view file;
    if (file.open(file_path)) {
      return load_error::unable_to_open_file;
    }

    std::size_t buffer_size;
    return load(
        mts::byte_view(file.content()), au_data, std::numeric_limits<std::size_t>::max(), buffer_size, e_format);
  }

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data, std::size_t maximum_loaded_samples) {
    mts::file_view file;
    if (file.open(file_path)) {
      return load_error::unable_to_open_file;
    }

    format e_format;

    return load(mts::byte_view(file.content()), au_data, maximum_loaded_samples, e_format);
  }

  template <typename _T>
  load_error load(const mts::filesystem::path& file_path, audio_data<_T>& au_data, std::size_t maximum_loaded_samples,
      format& e_format) {
    mts::file_view file;
    if (file.open(file_path)) {
      return load_error::unable_to_open_file;
    }

    return load(mts::byte_view(file.content()), au_data, maximum_loaded_samples, e_format);
  }

  template <typename _T>
  load_error load(const mts::byte_view& data, audio_data<_T>& au_data) {
    format e_format;
    return load(data, au_data, std::numeric_limits<std::size_t>::max(), e_format);
  }

  template <typename _T>
  load_error load(const mts::byte_view& data, audio_data<_T>& au_data, format& e_format) {
    return load(data, au_data, std::numeric_limits<std::size_t>::max(), e_format);
  }

  //
  // Save.
  //
  template <typename _Tp>
  save_error save(const mts::filesystem::path& file_path, const audio_data<_Tp>& data_view, format e_format) {
    using value_type = _Tp;

    if (data_view.sample_rate == 0) {
      return save_error::invalid_sampling_rate;
    }

    if (data_view.buffer.channel_size() == 0) {
      return save_error::empty_channel;
    }

    if (data_view.buffer.buffer_size() == 0) {
      return save_error::empty_buffer;
    }

    if (e_format == format::unknown) {
      return save_error::format_error;
    }

    //
    mts::byte_vector data;
    std::size_t sampling_rate = data_view.sample_rate;
    std::size_t channel_size = data_view.buffer.channel_size();
    std::size_t buffer_size = data_view.buffer.buffer_size();
    std::size_t bit_depth = format_to_bit_depth(e_format);

    const std::int32_t byte_depth = static_cast<std::int32_t>(bit_depth / 8);
    const std::int32_t data_chunk_size = static_cast<std::int32_t>(buffer_size * (channel_size * byte_depth));
    constexpr std::size_t header_size_without_fixed_part = 4;
    constexpr std::size_t format_fixed_header_size = 8;
    constexpr std::size_t format_pcm_header_size = 16;
    constexpr std::size_t format_ieee_header_size = 16; // 18;
    constexpr std::size_t fact_header_size = 8;
    constexpr std::size_t fact_data_size = 4;
    constexpr std::size_t fact_section_size = fact_header_size + fact_data_size;
    constexpr std::size_t pcm_format_chunk_size = format_fixed_header_size + format_pcm_header_size;
    constexpr std::size_t ieee_format_chunk_size = format_fixed_header_size + format_ieee_header_size;

    //
    // Header chunk.
    //
    data.push_back(detail::riff_header_id);

    // The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
    // chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
    std::int32_t file_size;
    if (mts::is_one_of(e_format, format::ieee_32_bit, format::ieee_64_bit)) {
      //      file_size = header_size_without_fixed_part + ieee_format_chunk_size + 12 + data_chunk_size;
      file_size = header_size_without_fixed_part + ieee_format_chunk_size + 8 + fact_section_size + data_chunk_size;
    }
    else {
      file_size = header_size_without_fixed_part + pcm_format_chunk_size + 8 + data_chunk_size;
    }

    data.push_back<std::int32_t>(file_size);

    data.push_padding();
    data.push_back(detail::wave_header_id);

    //
    // Format chunk.
    //
    data.push_padding();
    data.push_back(detail::format_header_id);

    detail::format w_format;

    if (mts::is_one_of(e_format, format::ieee_32_bit, format::ieee_64_bit)) {
      w_format = detail::format::ieee_float;
      data.push_back<std::int32_t>(format_ieee_header_size); // Format chunk size.
      data.push_back<std::int16_t>(detail::format::ieee_float); // Audio format.
    }
    else {
      w_format = detail::format::pcm;
      data.push_back<std::int32_t>(format_pcm_header_size); // Format chunk size (16 for PCM).
      data.push_back<std::int16_t>(detail::format::pcm); // Audio format.
    }

    data.push_back<std::int16_t>(channel_size);
    data.push_back<std::int32_t>(static_cast<std::int32_t>(sampling_rate));

    // Number of bytes per second.
    data.push_back<std::int32_t>(static_cast<std::int32_t>((channel_size * sampling_rate * byte_depth)));

    // Number of bytes per block.
    data.push_back<std::int16_t>(channel_size * byte_depth);
    data.push_back<std::int16_t>(bit_depth);

    if (w_format == detail::format::ieee_float) {
      //      data.push_back<std::int16_t>(0); // cbSize  2  Size of the extension: 0

      data.push_padding();
      data.push_back(detail::fact_header_id); // Chunk ID: fact.
      data.push_back<std::int32_t>(fact_data_size); // cksize.
      // cksize  4  Chunk size: 4
      data.push_back<std::int32_t>(static_cast<std::int32_t>(channel_size * sampling_rate));
    }

    //
    // Data chunk.
    //
    data.push_padding();
    data.push_back(detail::data_header_id);
    data.push_back<std::int32_t>(data_chunk_size);

    const mts::audio_buffer<_Tp>& buffers = data_view.buffer;

    switch (e_format) {
    case format::pcm_8_bit: {
      data.reserve(data.size() + buffer_size * channel_size);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<value_type, mts::byte_vector::convert_options::pcm_8_bit>(buffers[channel][i]);
        }
      }
    } break;

    case format::pcm_16_bit: {
      data.reserve(data.size() + buffer_size * channel_size * 2);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<value_type, mts::byte_vector::convert_options::pcm_16_bit>(buffers[channel][i]);
        }
      }
    } break;

    case format::pcm_24_bit: {
      data.reserve(data.size() + buffer_size * channel_size * 3);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<value_type, mts::byte_vector::convert_options::pcm_24_bit>(buffers[channel][i]);
        }
      }
    } break;

    case format::pcm_32_bit: {
      data.reserve(data.size() + buffer_size * channel_size * 4);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<value_type, mts::byte_vector::convert_options::pcm_32_bit>(buffers[channel][i]);
        }
      }
    } break;

    case format::ieee_32_bit: {
      data.reserve(data.size() + buffer_size * channel_size * 4);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<float>(buffers[channel][i]);
        }
      }
    } break;

    case format::ieee_64_bit: {
      data.reserve(data.size() + buffer_size * channel_size * 8);
      for (std::size_t i = 0; i < buffer_size; i++) {
        for (std::size_t channel = 0; channel < channel_size; channel++) {
          data.push_back<double>(buffers[channel][i]);
        }
      }
    } break;

    default: {
      return save_error::unsupported_bit_depth;
    } break;
    }

    if (!data.write_to_file(file_path)) {
      return save_error::unable_to_open_file;
    }

    return save_error::no_error;
  }
} // namespace wav
} // namespace mts.
