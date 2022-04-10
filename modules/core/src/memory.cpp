#include "mts/memory.h"

#include <memory>

#if __MTS_UNISTD__
  #include <unistd.h>
#endif // __MTS_UNISTD__

#if __MTS_MACOS__
  #include <sys/sysctl.h>
#endif // __MTS_MACOS__

#if __MTS_WINDOWS__
  #include <stdlib.h>
  #include <Windows.h>
  #include <sysinfoapi.h>
#endif // __MTS_WINDOWS__

#if __MTS_LINUX__
  #include <cstdio>
#endif // __MTS_LINUX__

MTS_BEGIN_SUB_NAMESPACE(memory)
void* malloc(std::size_t size) { return std::malloc(size); }
void* realloc(void* ptr, std::size_t new_size) { return std::realloc(ptr, new_size); }
void free(void* ptr) { std::free(ptr); }

namespace {
inline constexpr std::size_t default_page_size = 4096;
inline constexpr std::size_t default_cache_size = 64;

//
// get_page_size.
//

#if __MTS_UNISTD__
inline std::size_t get_page_size_impl() {
  long pagesize = sysconf(_SC_PAGE_SIZE);
  return pagesize >= 0 ? (std::size_t)pagesize : default_page_size;
}

#elif __MTS_WINDOWS__
inline std::size_t get_page_size_impl() {
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  return sys_info.dwPageSize >= 0 ? (std::size_t)sys_info.dwPageSize : default_page_size;
}

#else
inline std::size_t get_page_size_impl() {
  #warning Unsupported platform for page size
  return default_page_size;
}
#endif

//
// get_cache_size.
// https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size
//

#if __MTS_MACOS__
inline std::size_t get_cache_size_impl() {
  std::size_t line_size = 0;
  std::size_t sizeof_line_size = sizeof(line_size);
  sysctlbyname("hw.cachelinesize", &line_size, &sizeof_line_size, nullptr, 0);
  return line_size > 0 ? line_size : default_cache_size;
}

#elif __MTS_WINDOWS__
inline std::size_t get_cache_size_impl() {
  DWORD buffer_size = 0;

  if (!GetLogicalProcessorInformation(nullptr, &buffer_size)) {
    return default_cache_size;
  }

  SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer
      = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)_VMTS::memory::malloc(buffer_size);

  if (!buffer) {
    return default_cache_size;
  }

  if (!GetLogicalProcessorInformation(&buffer[0], &buffer_size)) {
    _VMTS::memory::free(buffer);
    return default_cache_size;
  }

  std::size_t line_size = default_cache_size;
  const std::size_t size = buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

  for (std::size_t i = 0; i < size; i++) {
    if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
      line_size = buffer[i].Cache.LineSize;
      break;
    }
  }

  _VMTS::memory::free(buffer);
  return line_size > 0 ? line_size : default_cache_size;
}

#elif __MTS_LINUX__
inline std::size_t get_cache_size_impl() {
  std::FILE* p = std::fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");

  if (!p) {
    return default_cache_size;
  }

  unsigned int i = 0;
  bool is_valid = std::fscanf(p, "%u", &i) == 1;
  std::fclose(p);

  return is_valid ? (std::size_t)i : default_cache_size;
}
#else
inline std::size_t get_cache_size_impl() {
  #warning Unsupported platform for cache size
  return default_cache_size;
}
#endif

} // namespace.

std::size_t get_page_size() {
  // In C++11, the following is guaranteed to perform thread-safe initialisation.
  static std::size_t size = get_page_size_impl();
  return size;
}

std::size_t get_cache_size() {
  // In C++11, the following is guaranteed to perform thread-safe initialisation.
  static std::size_t size = get_cache_size_impl();
  return size;
}

MTS_END_SUB_NAMESPACE(memory)
