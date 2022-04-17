#include "mts/print.h"
#include "mts/audio/device.h"
#include <thread>

mts::audio_device_manager::callback_result saw( void *outputBuffer, void * /*inputBuffer*/, std::size_t nBufferFrames,
         double streamTime, mts::audio_device_manager::stream_status status, void *data )
{
    float *buffer = (float *) outputBuffer;
 

    static float phase = 0;
    for(std::size_t i = 0; i < nBufferFrames; i++) {
        float value = std::sin(2.0f * M_PI * 400.0f * phase / 44100.0f);
        phase++;
        if(phase >= 44100) {
            phase = 0;
        }
        
        *buffer++ = value;
        *buffer++ = value;
    }

    if ( status != mts::audio_device_manager::stream_status::ok) {
      std::cout << "Stream underflow detected!" << std::endl;
    }

    return mts::audio_device_manager::callback_result::ok;
}

int main(int argc, const char* argv[]) {
  
    std::error_code ec;
    
    mts::audio_device_manager dm;
    if(std::error_code ec = dm.init()) {
        mts::errprint(ec.message());
        return -1;
    }
    std::size_t count = dm.get_audio_device_count(ec);
    
    if (ec) {
        mts::errprint(ec.message());
        return -1;
    }
    
    mts::print(count);

    std::vector<mts::audio_device_manager::device_info> devices = dm.get_audio_device_list(ec);
    if (ec) {
        mts::errprint(ec.message());
        return -1;
    }
    
    for (std::size_t i = 0; i < devices.size(); i++) {
        const mts::audio_device_manager::device_info& device = devices[i];
        mts::print<mts::colon_string>(i, device.name);
        mts::print<mts::equal_string>("    manufacturer", device.manufacturer);
        mts::print<mts::equal_string>("    sample_rates", device.sample_rates);
        mts::print<mts::equal_string>("    current_sample_rate", device.current_sample_rate);
        mts::print<mts::equal_string>("    preferred_sample_rate", device.preferred_sample_rate);
        mts::print<mts::equal_string>("    input_channels", device.input_channels);
        mts::print<mts::equal_string>("    output_channels", device.output_channels);
        mts::print<mts::equal_string>("    duplex_channels", device.duplex_channels);
//        mts::print(device.name, device.manufacturer, device.sample_rates, device.current_sample_rate, device.output_channels, device.input_channels);
    }

    
    mts::print<mts::equal_string>("default_input_device", dm.get_default_input_device(ec));
    mts::print<mts::equal_string>("default_output_device", dm.get_default_output_device(ec));
    
    
    
    
    
    
    
    
    std::size_t device = dm.get_default_output_device(ec);
    if (ec) {
        mts::errprint(ec.message());
        return -1;
    }
    
    mts::audio_device_manager::stream_parameters oParams;
    oParams.device_id = device;
    oParams.channel_count = 2;
    oParams.first_channel = 0;


    // An error in the openStream() function can be detected either by
    // checking for a non-zero return value OR by a subsequent call to
    // isStreamOpen().
    std::size_t buffer_size = 512;
    if (mts::error_result ec = dm.open_stream(&oParams, nullptr, mts::audio_device_format::float32, 44100, buffer_size, &saw)) {
        mts::errprint(ec.message());
        return -1;
    }
    
    if (!dm.is_stream_open()) {
        mts::errprint("SALKJSKALJSKLAJSA");
        return -1;
    }
    
    
    if(std::error_code ec = dm.start_stream()) {
        mts::errprint(ec.message());
        return -1;
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    mts::print("DONE");
    dm.stop_stream();
    
//    if ( dac.openStream( &oParams, NULL, FORMAT, fs, &bufferFrames, &saw, (void *)data, &options ) )
//      goto cleanup;
//    if ( dac.isStreamOpen() == false ) goto cleanup;

    //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
    
    // Stream is open ... now start it.
//    if ( dac.startStream() ) goto cleanup;
//
//    if ( checkCount ) {
//      while ( dac.isStreamRunning() == true ) SLEEP( 100 );
//    }
//    else {
//      std::cout << "\nPlaying ... quit with Ctrl-C (buffer size = " << bufferFrames << ").\n";
//
//      // Install an interrupt handler function.
//      done = false;
//      (void) signal(SIGINT, finish);
//
//      while ( !done && dac.isStreamRunning() ) SLEEP( 100 );
//
//      // Block released ... stop the stream
//      if ( dac.isStreamRunning() )
//        dac.stopStream();  // or could call dac.abortStream();
//    }

  return 0;
}

//std::vector<std::size_t> sample_rates;
//  std::size_t current_sample_rate= 0;
//  std::size_t preferred_sample_rate= 0;
//  std::size_t output_channels = 0;
//  std::size_t input_channels= 0;
//  std::size_t duplex_channels= 0;
//  bool is_probed = false;
//    bool is_default_output = false;
//    bool is_default_input = false;
//    audio_device_format native_formats
