
#pragma once
#include "mts/config.h"
#include "mts/event/common.h"
#include "mts/event/filesystem.h"
#include "mts/memory_range.h"

MTS_BEGIN_NAMESPACE
class fstream {
public:
  fstream() {}

  void open(io_context& ctx, const mts::filesystem::path& filepath, std::function<void(fstream&)> callback);

  template <typename Fct, typename... Args>
  void open(io_context& ctx, const mts::filesystem::path& filepath, Fct&& callback, Args&&... args) {
    if constexpr (sizeof...(Args) == 0) {
      open(ctx, filepath, std::function<void(fstream&)>(callback));
    }
    else {
      open(ctx, filepath,
          std::bind(callback, std::placeholders::_1, std::reference_wrapper<std::remove_reference_t<Args>>(args)...));
    }
  }

  bool open(const mts::filesystem::path& filepath);

  std::size_t read(mts::memory_range<char> buffer);

  void read(
      io_context& ctx, mts::memory_range<char> buffer, std::function<void(fstream&, mts::memory_range<char>)> callback);

  template <typename T>
  using type_t = std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<std::remove_reference_t<T>>, T>;

  template <typename Fct, typename... Args>
  void read(io_context& ctx, mts::memory_range<char> buffer, Fct&& callback, Args&&... args) {
    if constexpr (sizeof...(Args) == 0) {
      read(ctx, buffer, std::function<void(fstream&, mts::memory_range<char>)>(callback));
    }
    else {
      read(ctx, buffer, std::bind(callback, std::placeholders::_1, std::placeholders::_2, std::forward<Args>(args)...));
    }
  }

  inline bool is_open() const { return _handle.fd != -1; }

  inline const std::error_code& error() const { return _handle.ec; }

private:
  mts::filesystem::path _path;
  mts::file_handle _handle = { -1, std::error_code() };
};
MTS_END_NAMESPACE
