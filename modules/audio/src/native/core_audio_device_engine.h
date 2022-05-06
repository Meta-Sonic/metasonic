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
#include "../device_engine.h"

MTS_BEGIN_NAMESPACE
class core_audio_engine : public audio_engine {
public:
  core_audio_engine();

  virtual ~core_audio_engine() override;
  virtual std::error_code init() override;
  virtual std::size_t get_audio_device_count(std::error_code& ec) override;
  virtual std::size_t get_default_input_device(std::error_code& ec) override;

  virtual std::size_t get_default_output_device(std::error_code& ec) override;

  virtual audio_device_info get_audio_device_info(audio_device_index index, std::error_code& ec) override;

  virtual audio_device_info get_audio_device_info(audio_device_id device_id, std::error_code& ec) override;

  virtual std::vector<audio_device_manager::device_info> get_audio_device_list(std::error_code& ec) override;

  virtual void close_stream() override;
  virtual std::error_code start_stream() override;
  virtual std::error_code stop_stream() override;
  virtual std::error_code abort_stream() override;

protected:
  virtual mts::error_result probe_device_open(std::size_t device, stream_mode mode, std::size_t channels,
      std::size_t firstChannel, std::size_t sampleRate, audio_device_format format, std::size_t& bufferSize) override;

private:
  struct impl;
};

MTS_END_NAMESPACE
