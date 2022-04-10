#include "mts/file_view.h"
#include "mts/print.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>

#undef __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
#undef __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP

#if __MTS_WINDOWS__
  #define __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 1
  #define __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP 0

#elif __MTS_UNISTD__
  #define __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 0
  #include <unistd.h>

  #ifdef HAVE_MMAP
    #define __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP HAVE_MMAP
  #elif _POSIX_VERSION >= 199506L
    #define __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP 1
  #else
    #define __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP 0
  #endif

// Copy to memory.
#else
  #define __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 0
  #define __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP 0
#endif

#if __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

#elif __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP
  #include <fcntl.h>
  #include <sys/mman.h>
  #include <sys/types.h>
#endif

MTS_BEGIN_NAMESPACE

struct file_view::impl {
  static std::error_code open(const _VMTS::filesystem::path& file_path, file_view& fv);
  static inline void close(file_view& fv);
};

std::error_code file_view::open(const _VMTS::filesystem::path& file_path) {
  close();
  return impl::open(file_path, *this);
}

void file_view::close() {
  if (_data) {
    impl::close(*this);
  }

  _data = nullptr;
  _size = 0;
}

//
//
//
#if __MTS_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
std::error_code file_view::impl::open(const _VMTS::filesystem::path& file_path, file_view& fv) {
  _VMTS::filesystem::path w_path = _VMTS::filesystem::path(file_path).make_preferred();

  HANDLE hFile = CreateFileW(
      (LPCWSTR)w_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (hFile == INVALID_HANDLE_VALUE) {
    switch (GetLastError()) {
    case ERROR_FILE_NOT_FOUND:
      return std::make_error_code(std::errc::no_such_file_or_directory);
    case ERROR_ACCESS_DENIED:
      return std::make_error_code(std::errc::permission_denied);

    default:
      return _VMTS::make_error_code(_VMTS::errc::unknown);
    }
  }

  DWORD size = GetFileSize(hFile, nullptr);
  if (size == INVALID_FILE_SIZE || size == 0) {
    _VMTS::errprint("FileView : CreateFileA -> INVALID_FILE_SIZE");
    CloseHandle(hFile);
    return _VMTS::make_error_code(_VMTS::errc::unknown);
  }

  HANDLE hMap = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, size, nullptr);
  if (!hMap) {
    _VMTS::errprint("FileView : CreateFileMappingA -> nulll");
    CloseHandle(hFile);
    return _VMTS::make_error_code(_VMTS::errc::unknown);
  }

  pointer data = (pointer)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);

  // We can call CloseHandle here, but it will not be closed until we unmap the view.
  CloseHandle(hMap);
  CloseHandle(hFile);
  _data = data;
  _size = (size_type)size;

  return std::error_code();
}

void file_view::impl::close(file_view& fv) { UnmapViewOfFile(fv._data); }

//
//
//
#elif __MTS_FILE_VIEW_USE_POSIX_MEMORY_MAP
std::error_code file_view::impl::open(const _VMTS::filesystem::path& file_path, file_view& fv) {
  int fd = ::open(file_path.c_str(), O_RDONLY);
  if (fd < 0) {
    return std::make_error_code(static_cast<std::errc>(errno));
  }

  // Get file size.
  off_t size = lseek(fd, 0, SEEK_END);
  if (size <= 0) {
    std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
    ::close(fd);
    return ec;
  }

  // Create file map.
  pointer data = (pointer)mmap(nullptr, (size_type)size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (data == MAP_FAILED) {
    std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
    ::close(fd);
    return ec;
  }

  ::close(fd);
  fv._data = data;
  fv._size = (size_type)size;
  return std::error_code();
}

void file_view::impl::close(file_view& fv) { munmap(fv._data, fv._size); }

//
//
//
#else
std::error_code file_view::impl::open(const _VMTS::filesystem::path& file_path, file_view& fv) {
  std::FILE* fd = std::fopen(file_path.c_str(), "rb");
  if (!fd) {
    return std::make_error_code(static_cast<std::errc>(errno));
  }

  // Get file size.
  std::fseek(fd, 0, SEEK_END);
  std::ptrdiff_t size = std::ftell(fd);
  if (size <= 0) {
    std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
    std::fclose(fd);
    return ec;
  }

  std::rewind(fd);
  pointer data = (pointer)std::malloc(size);
  if (!data) {
    std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
    std::fclose(fd);
    return ec;
  }

  // Copy content into data.
  // std::fread returns the number of objects read successfully.
  if (std::fread(data, size, 1, fd) != 1) {
    std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
    std::free(data);
    std::fclose(fd);
    return ec;
  }

  fv._data = data;
  fv._size = (size_type)size;
  return std::error_code();
}

void file_view::impl::close(file_view& fv) { std::free(fv._data); }
#endif
MTS_END_NAMESPACE
