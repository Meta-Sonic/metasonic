#include "mts/audio/detail/buffer_allocator.h"
#include "mts/assert.h"
#include "mts/memory.h"

MTS_BEGIN_NAMESPACE
namespace detail {
namespace {
  template <typename T>
  void __deallocate_buffer(T* const* buffers) {
    if (buffers) {
      memory::free((void*)buffers);
    }
  }

  template <typename T>
  T** __allocate_buffer(std::size_t buffer_size, std::size_t channel_size) {

    if (!buffer_size || !channel_size) {
      return nullptr;
    }

    const std::size_t cache_size = memory::get_cache_size();

    const std::size_t buffer_ptr_size = channel_size * sizeof(T*);
    const std::size_t channel_buffer_size = buffer_size * sizeof(T);

    const std::size_t total_buffer_size = channel_size * channel_buffer_size + channel_size * (cache_size - 1);
    const std::size_t total_size = buffer_ptr_size + total_buffer_size;

    void* bytes = memory::malloc(total_size);

    if (bytes == nullptr) {
      return nullptr;
    }

    T** buffers = (T**)bytes;

    void* raw_ptr = ((std::uint8_t*)bytes) + buffer_ptr_size;
    const std::uint8_t* raw_ptr_end = static_cast<std::uint8_t*>(raw_ptr) + total_buffer_size;

    std::size_t space = total_buffer_size;

    for (std::size_t i = 0; i < channel_size; i++) {
      void* ptr = std::align(cache_size, channel_buffer_size, raw_ptr, space);

      mts_assert(ptr != nullptr, "Can't align buffer with cache size.");

      if (ptr == nullptr) {
        __deallocate_buffer(buffers);
        return nullptr;
      }

      buffers[i] = static_cast<T*>(ptr);

      raw_ptr = static_cast<std::uint8_t*>(ptr) + channel_buffer_size;
      space = raw_ptr_end - static_cast<std::uint8_t*>(static_cast<void*>(buffers[i]));
    }

    return buffers;
  }

  template <typename T>
  T** __allocate_interleaved_buffer(std::size_t buffer_size, std::size_t channel_size) {

    if (!buffer_size || !channel_size) {
      return nullptr;
    }

    const std::size_t cache_size = memory::get_cache_size();
    const std::size_t buffer_ptr_size = sizeof(T*);
    const std::size_t channel_buffer_size = buffer_size * channel_size * sizeof(T);
    const std::size_t total_buffer_size = channel_size * buffer_size * sizeof(T) + cache_size - 1;
    const std::size_t total_size = buffer_ptr_size + total_buffer_size;

    void* bytes = memory::malloc(total_size);

    if (bytes == nullptr) {
      return nullptr;
    }

    T** buffers = (T**)bytes;

    void* raw_ptr = ((std::uint8_t*)bytes) + buffer_ptr_size;
    std::size_t space = total_buffer_size;
    void* ptr = std::align(cache_size, channel_buffer_size, raw_ptr, space);

    mts_assert(ptr != nullptr, "Can't align buffer with cache size.");

    if (ptr == nullptr) {
      __deallocate_buffer(buffers);
      return nullptr;
    }

    buffers[0] = static_cast<T*>(ptr);

    return buffers;
  }
} // namespace.

void allocate_audio_buffer(float* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_buffer<float>(buffer_size, channel_size);
}

void allocate_audio_buffer(double* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_buffer<double>(buffer_size, channel_size);
}

void allocate_audio_buffer(long double* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_buffer<long double>(buffer_size, channel_size);
}

void allocate_interleaved_audio_buffer(float* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_interleaved_buffer<float>(buffer_size, channel_size);
}

void allocate_interleaved_audio_buffer(double* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_interleaved_buffer<double>(buffer_size, channel_size);
}

void allocate_interleaved_audio_buffer(
    long double* const*& buffers, std::size_t buffer_size, std::size_t channel_size) {
  buffers = __allocate_interleaved_buffer<long double>(buffer_size, channel_size);
}

void deallocate_audio_buffer(float* const* buffers) { __deallocate_buffer(buffers); }

void deallocate_audio_buffer(double* const* buffers) { __deallocate_buffer(buffers); }

void deallocate_audio_buffer(long double* const* buffers) { __deallocate_buffer(buffers); }
} // namespace detail.

MTS_END_NAMESPACE
