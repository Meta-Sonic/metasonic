#include "mts/audio/device_manager.h"
#include "device_engine.h"
#include "mts/assert.h"
#include "mts/int24_t.h"
#include "mts/util.h"

#if __MTS_MACOS__
  #include "native/core_audio_device_engine.h"
#endif // __MTS_MACOS__

MTS_BEGIN_NAMESPACE
namespace {
class audio_device_error_category : public std::error_category {
public:
  audio_device_error_category() noexcept = default;
  virtual ~audio_device_error_category() noexcept override = default;
  virtual const char* name() const noexcept override { return "mts::audio_device"; }

  virtual std::string message(int ev) const override {
    switch (static_cast<_VMTS::audio_device_error>(ev)) {
    case audio_device_error::no_device_found:
      return "no_device_found";
    case audio_device_error::invalid_device:
      return "invalid_device";
    case audio_device_error::device_disconnected:
      return "device_disconnected";
    case audio_device_error::memory_error:
      return "memory_error";
    case audio_device_error::invalid_parameter:
      return "invalid_parameter";
    case audio_device_error::invalid_use:
      return "invalid_use";
    case audio_device_error::driver_error:
      return "driver_error";
    case audio_device_error::system_error:
      return "system_error";
    case audio_device_error::thread_error:
      return "thread_error";
    default:
      mts_error("Unrecognized error");
      return "Unrecognized error";
    }
  }
};
inline static const audio_device_error_category s_error_code_category;
} // namespace.

const std::error_category& audio_device_error_category() noexcept { return s_error_code_category; }

audio_device_manager::audio_device_manager(engine_type etype) {
#if __MTS_MACOS__
  if (mts::is_one_of(etype, engine_type::system_default, engine_type::core_audio)) {
    _engine = std::make_unique<core_audio_engine>();
  }
#endif // __MTS_MACOS__
}

audio_device_manager::~audio_device_manager() {}

std::error_code audio_device_manager::init() { return _engine->init(); }

std::size_t audio_device_manager::get_audio_device_count(std::error_code& ec) {
  return _engine->get_audio_device_count(ec);
}

std::size_t audio_device_manager::get_default_input_device(std::error_code& ec) {
  return _engine->get_default_input_device(ec);
}

std::size_t audio_device_manager::get_default_output_device(std::error_code& ec) {
  return _engine->get_default_output_device(ec);
}

audio_device_manager::device_info audio_device_manager::get_audio_device_info(
    audio_device_index index, std::error_code& ec) {
  return _engine->get_audio_device_info(index, ec);
}

audio_device_manager::device_info audio_device_manager::get_audio_device_info(
    audio_device_id device_id, std::error_code& ec) {
  return _engine->get_audio_device_info(device_id, ec);
}

std::vector<audio_device_manager::device_info> audio_device_manager::get_audio_device_list(std::error_code& ec) {
  return _engine->get_audio_device_list(ec);
}

mts::error_result audio_device_manager::open_stream(const stream_parameters* output_params,
    const stream_parameters* input_params, device_format format, std::size_t sample_rate, std::size_t& buffer_size,
    audio_device_callback callback, void* user_data) {

  return _engine->open_stream(output_params, input_params, format, sample_rate, buffer_size, callback, user_data);
}

void audio_device_manager::close_stream() { return _engine->close_stream(); }
std::error_code audio_device_manager::start_stream() { return _engine->start_stream(); }
std::error_code audio_device_manager::stop_stream() { return _engine->stop_stream(); }
std::error_code audio_device_manager::abort_stream() { return _engine->abort_stream(); }

std::size_t audio_device_manager::get_stream_sample_rate() const { return _engine->get_stream_sample_rate(); }

std::size_t audio_device_manager::get_stream_latency() const { return _engine->get_stream_latency(); }

bool audio_device_manager::is_stream_open() const { return _engine->is_stream_open(); }
bool audio_device_manager::is_stream_running() const { return _engine->is_stream_running(); }
double audio_device_manager::get_stream_time() { return _engine->get_stream_time(); }

// inline RtAudio::Api RtAudio :: getCurrentApi( void ) { return rtapi_->getCurrentApi(); }
// inline unsigned int RtAudio :: getDeviceCount( void ) { return rtapi_->getDeviceCount(); }
// inline RtAudio::DeviceInfo RtAudio :: getDeviceInfo( unsigned int device ) { return rtapi_->getDeviceInfo( device );
// } inline unsigned int RtAudio :: getDefaultInputDevice( void ) { return rtapi_->getDefaultInputDevice(); } inline
// unsigned int RtAudio :: getDefaultOutputDevice( void ) { return rtapi_->getDefaultOutputDevice(); } inline void
// RtAudio :: closeStream( void ) { return rtapi_->closeStream(); } inline RtAudioErrorType RtAudio :: startStream( void
// ) { return rtapi_->startStream(); } inline RtAudioErrorType RtAudio :: stopStream( void )  { return
// rtapi_->stopStream(); } inline RtAudioErrorType RtAudio :: abortStream( void ) { return rtapi_->abortStream(); }
// inline bool RtAudio :: isStreamOpen( void ) const { return rtapi_->isStreamOpen(); }
// inline bool RtAudio :: isStreamRunning( void ) const { return rtapi_->isStreamRunning(); }
// inline long RtAudio :: getStreamLatency( void ) { return rtapi_->getStreamLatency(); }

// void print_audio_device_info(const audio_device_info& device) {
//   mts::print<mts::equal_string>("name", device.name);
//   mts::print<mts::equal_string>("id", device.id);
//   mts::print<mts::equal_string>("manufacturer", device.manufacturer);
//   mts::print<mts::equal_string>("sample_rates", device.sample_rates);
//   mts::print<mts::equal_string>("current_sample_rate", device.current_sample_rate);
//   mts::print<mts::equal_string>("preferred_sample_rate", device.preferred_sample_rate);
//   mts::print<mts::equal_string>("input_channels", device.input_channels);
//   mts::print<mts::equal_string>("output_channels", device.output_channels);
//   mts::print<mts::equal_string>("duplex_channels", device.duplex_channels);
//   mts::print<mts::equal_string>("is_default_input", device.is_default_input);
//   mts::print<mts::equal_string>("is_default_output", device.is_default_output);
// }
MTS_END_NAMESPACE
