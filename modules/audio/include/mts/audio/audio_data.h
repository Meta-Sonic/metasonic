/////
///// BSD 3-Clause License
/////
///// Copyright (c) 2022, Alexandre Arsenault
///// All rights reserved.
/////
///// Redistribution and use in source and binary forms, with or without
///// modification, are permitted provided that the following conditions are met:
/////
///// * Redistributions of source code must retain the above copyright notice, this
/////   list of conditions and the following disclaimer.
/////
///// * Redistributions in binary form must reproduce the above copyright notice,
/////   this list of conditions and the following disclaimer in the documentation
/////   and/or other materials provided with the distribution.
/////
///// * Neither the name of the copyright holder nor the names of its
/////   contributors may be used to endorse or promote products derived from
/////   this software without specific prior written permission.
/////
///// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
///// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
///// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
///// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
///// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
///// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
///// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
///// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
///// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
///// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
///// POSSIBILITY OF SUCH DAMAGE.
/////
//
//#pragma once
//#include "dkt/common.h"
//#include "dkt/wire.h"
//#include "fst/mapped_file.h"
//#include <array>
//#include <cstring>
//#include <span>
//#include <vector>
//
// namespace dkt {
// template <typename _Tp>
// class audio_data;
//
// template <typename _Tp>
// class audio_stream_data;
//
// template <typename _Tp>
// class audio_stream_shared_data;
//
/////
///// audio_data_view.
/////
// template <typename _Tp>
// class audio_data_view {
// public:
//   using value_type = _Tp;
//   using size_type = std::size_t;
//   using channel_view = std::span<const value_type>;
//   using multi_channel_view = std::span<const channel_view>;
//   using const_wire = dkt::const_wire_t<value_type>;
//
//   audio_data_view() = default;
//
//   inline audio_data_view(const audio_data_view& v)
//       : _buffers(v._buffers)
//       , _sampling_rate(v._sampling_rate) {}
//
//   inline audio_data_view(audio_data_view&& v)
//       : _buffers(std::move(v._buffers))
//       , _sampling_rate(v._sampling_rate) {
//
//     //    v._buffers = nullptr;
//     v._sampling_rate = 0;
//   }
//
//   inline audio_data_view(size_type sampling_rate, const std::vector<channel_view>& channels)
//       : _buffers(channels)
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data_view(size_type sampling_rate, std::vector<channel_view>&& channels)
//       : _buffers(std::move(channels))
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data_view(const audio_data<value_type>& data);
//
//   inline audio_data_view(const audio_stream_data<value_type>& data);
//
//   inline audio_data_view(const audio_stream_shared_data<value_type>& data);
//
//   template <std::size_t N>
//   inline audio_data_view(size_type sampling_rate, const std::array<channel_view, N>& buffers) {
//     _sampling_rate = sampling_rate;
//
//     _buffers.resize(N);
//     for (std::size_t i = 0; i < N; i++) {
//       _buffers[i] = buffers[i];
//     }
//   }
//
//   inline audio_data_view(size_type sampling_rate, channel_view mono_buffer) {
//     _sampling_rate = sampling_rate;
//     _buffers.resize(1);
//     _buffers[0] = mono_buffer;
//   }
//
//   inline audio_data_view(size_type sampling_rate, channel_view left_buffer, channel_view right_buffer) {
//     _sampling_rate = sampling_rate;
//     _buffers.resize(2);
//     _buffers[0] = left_buffer;
//     _buffers[1] = right_buffer;
//   }
//
//   inline audio_data_view& operator=(const audio_data_view& v) {
//     _buffers = v._buffers;
//     _sampling_rate = v._sampling_rate;
//     return *this;
//   }
//
//   inline audio_data_view& operator=(audio_data_view&& v) {
//     _buffers = std::move(v._buffers);
//     _sampling_rate = v._sampling_rate;
//
//     //    v._buffers = nullptr;
//     v._sampling_rate = 0;
//     return *this;
//   }
//
//   inline audio_data_view& operator=(const audio_data<value_type>& data);
//   inline audio_data_view& operator=(const audio_stream_data<value_type>& data);
//   inline audio_data_view& operator=(const audio_stream_shared_data<value_type>& data);
//
//   inline bool is_valid() const noexcept { return channel_size() > 0 && buffer_size() != 0; }
//
//   inline size_type sampling_rate() const { return _sampling_rate; }
//
//   inline void set_sampling_rate(size_type sampling_rate) noexcept { _sampling_rate = sampling_rate; }
//
//   inline size_type channel_size() const { return _buffers.size(); }
//
//   // Undefined behaviour if not initialized or if channel_size equals zero.
//   inline size_type buffer_size() const { return _buffers[0].size(); }
//
//   inline channel_view get_buffer(size_type index = 0) const noexcept { return _buffers[index]; }
//   inline const_wire get_wire(size_type index = 0) const noexcept { return _buffers[index]; }
//
//   inline multi_channel_view get_buffers() const noexcept { return multi_channel_view(_buffers.data(),
//   channel_size()); }
//
// private:
//   std::vector<channel_view> _buffers;
//   size_type _sampling_rate = 0;
// };
//
/////
///// audio_data.
/////
// template <typename _Tp>
// class audio_data {
// public:
//   using value_type = _Tp;
//   using size_type = typename std::vector<value_type>::size_type;
//   using channel_buffer = std::vector<value_type>;
//   using multi_channel_buffer = std::vector<channel_buffer>;
//   using wire_type = wire_t<value_type>;
//   using const_wire_type = const_wire_t<value_type>;
//
//   static_assert(std::is_scalar<value_type>::value, "Value type must be a scalar type.");
//
//   audio_data() = default;
//
//   inline audio_data(audio_data&& d)
//       : _sampling_rate(d._sampling_rate)
//       , _buffers(std::move(d._buffers)) {
//     d._sampling_rate = 0;
//   }
//
//   inline audio_data(const audio_data& d)
//       : _buffers(d._buffers)
//       , _sampling_rate(d._sampling_rate) {}
//
//   inline audio_data(size_type sampling_rate, const multi_channel_buffer& buffers)
//       : _buffers(buffers)
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data(size_type sampling_rate, multi_channel_buffer&& buffers)
//       : _buffers(std::move(buffers))
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data(size_type sampling_rate, const channel_buffer& buffer)
//       : _buffers{ buffer }
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data(size_type sampling_rate, channel_buffer&& buffer)
//       : _buffers{ std::move(buffer) }
//       , _sampling_rate(sampling_rate) {}
//
//   inline audio_data(audio_data_view<value_type> data_view)
//       : _sampling_rate(sampling_rate) {
//     _buffers.resize(data_view.channel_size);
//
//     size_type size = data_view.buffer_size();
//     for (std::size_t i = 0; i < _buffers.size(); i++) {
//       _buffers[i].resize(size);
//     }
//
//     for (std::size_t i = 0; i < _buffers.size(); i++) {
//       std::memmove(_buffers[i].data(), data_view.get_buffer(i).data(), sizeof(value_type) * size);
//     }
//   }
//
//   inline audio_data& operator=(const audio_data& d) {
//     _sampling_rate = d._sampling_rate;
//     _buffers = d._buffers;
//     return *this;
//   }
//
//   inline audio_data& operator=(audio_data&& d) {
//     _sampling_rate = d._sampling_rate;
//     _buffers = std::move(d._buffers);
//     d._sampling_rate = 0;
//     return *this;
//   }
//
//   inline bool is_valid() const noexcept { return !_buffers.empty(); }
//
//   inline size_type channel_size() const noexcept { return _buffers.size(); }
//
//   inline size_type buffer_size() const noexcept { return _buffers.empty() ? 0 : _buffers[0].size(); }
//
//   inline void set_sampling_rate(size_type sampling_rate) noexcept { _sampling_rate = sampling_rate; }
//   inline size_type sampling_rate() const noexcept { return _sampling_rate; }
//
//   inline const multi_channel_buffer& get_buffers() const noexcept { return _buffers; }
//   inline multi_channel_buffer& get_buffers() noexcept { return _buffers; }
//
//   inline const channel_buffer& get_buffer(size_type index = 0) const noexcept { return _buffers[index]; }
//   inline channel_buffer& get_buffer(size_type index = 0) noexcept { return _buffers[index]; }
//
//   inline wire_type get_wire(size_type index = 0) noexcept { return _buffers[index]; }
//   inline const_wire_type get_wire(size_type index = 0) const noexcept { return _buffers[index]; }
//
//   inline void release() { _buffers = multi_channel_buffer(); }
//
// private:
//   multi_channel_buffer _buffers;
//   size_type _sampling_rate = 0;
// };
//
////
//// audio_stream_data.
////
// template <typename _Tp>
// class audio_stream_data {
// public:
//   using value_type = _Tp;
//   using size_type = std::size_t;
//   using data_view = audio_data_view<value_type>;
//   using channel_view = std::span<const value_type>;
//   using multi_channel_view = std::span<const channel_view>;
//   using const_wire = dkt::const_wire_t<value_type>;
//
//   static_assert(std::is_scalar<value_type>::value, "Value type must be a scalar type.");
//
//   audio_stream_data() = default;
//   audio_stream_data(const audio_stream_data& d) = delete;
//
//   inline audio_stream_data(audio_stream_data&& d)
//       : _file(std::move(d._file))
//       , _data(std::move(d._data)) {}
//
//   inline audio_stream_data(fst::mapped_file&& file, data_view&& data)
//       : _file(std::move(file))
//       , _data(std::move(data)) {}
//
//   audio_stream_data& operator=(const audio_stream_data& d) = delete;
//
//   inline audio_stream_data& operator=(audio_stream_data&& d) {
//     _file = std::move(d._file);
//     _data = std::move(d._data);
//
//     return *this;
//   }
//
//   inline data_view get_data() const { return _data; }
//
// private:
//   fst::mapped_file _file;
//   data_view _data;
// };
//
////
//// audio_multi_stream_data.
////
// template <typename _Tp>
// class audio_multi_stream_data {
// public:
//   using value_type = _Tp;
//   using size_type = std::size_t;
//   using data_view = audio_data_view<value_type>;
//
//   static_assert(std::is_scalar<value_type>::value, "Value type must be a scalar type.");
//
//   audio_multi_stream_data() = default;
//   audio_multi_stream_data(const audio_multi_stream_data& d) = delete;
//
//   inline audio_multi_stream_data(audio_multi_stream_data&& d)
//       : _file(std::move(d._file))
//       , _data(std::move(d._data)) {}
//
//   inline audio_multi_stream_data(fst::mapped_file&& file, std::vector<data_view>&& data)
//       : _file(std::move(file))
//       , _data(std::move(data)) {}
//
//   audio_multi_stream_data& operator=(const audio_multi_stream_data& d) = delete;
//
//   inline audio_multi_stream_data& operator=(audio_multi_stream_data&& d) {
//     _file = std::move(d._file);
//     _data = std::move(d._data);
//
//     return *this;
//   }
//
//   inline size_type size() const { return _data.size(); }
//
//   inline data_view get_data(size_type index) const { return _data[index]; }
//
//   inline data_view operator[](size_type index) const { return _data[index]; }
//
// private:
//   fst::mapped_file _file;
//   std::vector<data_view> _data;
// };
//
/////
///// audio_stream_shared_data.
/////
// template <typename _Tp>
// class audio_stream_shared_data {
// public:
//   using value_type = _Tp;
//   using size_type = std::size_t;
//   using data_view = audio_data_view<value_type>;
//   using channel_view = std::span<const value_type>;
//   using multi_channel_view = std::span<const channel_view>;
//   using const_wire = dkt::const_wire_t<value_type>;
//
//   static_assert(std::is_scalar<value_type>::value, "Value type must be a scalar type.");
//
//   audio_stream_shared_data() = default;
//
//   inline audio_stream_shared_data(const audio_stream_shared_data& d)
//       : _file(d._file)
//       , _data(d._data) {}
//
//   inline audio_stream_shared_data(audio_stream_shared_data&& d)
//       : _file(std::move(d._file))
//       , _data(std::move(d._data)) {}
//
//   inline audio_stream_shared_data(fst::mapped_file&& file, data_view&& data)
//       : _file(std::make_shared<fst::mapped_file>(std::move(file)))
//       , _data(std::move(data)) {}
//
//   inline audio_stream_shared_data(std::shared_ptr<fst::mapped_file> file, data_view&& data)
//       : _file(file)
//       , _data(std::move(data)) {}
//
//   inline audio_stream_shared_data(std::shared_ptr<fst::mapped_file>&& file, data_view&& data)
//       : _file(std::move(file))
//       , _data(std::move(data)) {}
//
//   inline audio_stream_shared_data& operator=(const audio_stream_shared_data& d) {
//     _file = d._file;
//     _data = d._data;
//
//     return *this;
//   }
//
//   inline audio_stream_shared_data& operator=(audio_stream_shared_data&& d) {
//     _file = std::move(d._file);
//     _data = std::move(d._data);
//     return *this;
//   }
//
//   inline data_view get_data() const { return _data; }
//
//   inline std::size_t get_count() const { return _file.use_count(); }
//
// private:
//   std::shared_ptr<fst::mapped_file> _file;
//   data_view _data;
// };
//
/////
///// audio_data_view.
/////
// template <typename _Tp>
// audio_data_view<_Tp>::audio_data_view(const audio_data<_Tp>& data) {
//   _sampling_rate = data.sampling_rate();
//   _buffers.resize(data.channel_size());
//
//   for (std::size_t i = 0; i < _buffers.size(); i++) {
//     _buffers[i] = data.get_buffer(i);
//   }
// }
//
// template <typename _Tp>
// audio_data_view<_Tp>::audio_data_view(const audio_stream_data<_Tp>& data)
//     : audio_data_view(data.get_data()) {}
//
// template <typename _Tp>
// audio_data_view<_Tp>::audio_data_view(const audio_stream_shared_data<_Tp>& data)
//     : audio_data_view(data.get_data()) {}
//
// template <typename _Tp>
// audio_data_view<_Tp>& audio_data_view<_Tp>::operator=(const audio_data<_Tp>& data) {
//   _sampling_rate = data.sampling_rate();
//   _buffers.resize(data.channel_size());
//
//   for (std::size_t i = 0; i < _buffers.size(); i++) {
//     _buffers[i] = data.get_buffer(i);
//   }
//
//   return *this;
// }
//
// template <typename _Tp>
// audio_data_view<_Tp>& audio_data_view<_Tp>::operator=(const audio_stream_data<_Tp>& data) {
//   return operator=(data.get_data());
// }
//
// template <typename _Tp>
// audio_data_view<_Tp>& audio_data_view<_Tp>::operator=(const audio_stream_shared_data<_Tp>& data) {
//   return operator=(data.get_data());
// }
// } // namespace dkt.
