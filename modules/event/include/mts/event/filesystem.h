
#pragma once
#include "mts/config.h"
#include "mts/event/common.h"
#include "mts/filesystem.h"
#include "mts/memory_range.h"

MTS_BEGIN_NAMESPACE

using file_t = int;

struct file_handle {
  file_t fd;
  std::error_code ec;

  void close() const;
};

struct timespec_t {
  long tv_sec;
  long tv_nsec;
};

struct fs_stat {
  std::uint64_t st_dev;
  std::uint64_t st_mode;
  std::uint64_t st_nlink;
  std::uint64_t st_uid;
  std::uint64_t st_gid;
  std::uint64_t st_rdev;
  std::uint64_t st_ino;
  std::uint64_t st_size;
  std::uint64_t st_blksize;
  std::uint64_t st_blocks;
  std::uint64_t st_flags;
  std::uint64_t st_gen;
  timespec_t st_atim;
  timespec_t st_mtim;
  timespec_t st_ctim;
  timespec_t st_birthtim;
};

///
///
///
class open_file_task : public task_base {
public:
  inline open_file_task(const mts::filesystem::path& filepath, int flags)
      : _filepath(filepath)
      , _flags(flags) {}
  virtual ~open_file_task() = default;

protected:
  virtual void run(io_context& ctx, const file_handle& fd) = 0;

private:
  friend class io_context;
  class request_impl;
  mts::filesystem::path _filepath;
  int _flags = 0;

  static void spawn(io_context& ctx, std::unique_ptr<open_file_task> t);

  template <typename Fct>
  static void spawn(io_context& ctx, const mts::filesystem::path& filepath, int flags, Fct&& fct) {
    class open_file_task_t : public open_file_task {
    public:
      open_file_task_t(const mts::filesystem::path& filepath, int flags, Fct&& fct)
          : open_file_task(filepath, flags)
          , _fct(std::move(fct)) {}

      virtual ~open_file_task_t() override = default;

    protected:
      virtual void run(io_context& ctx, const file_handle& fd) override { _fct(ctx, fd); }

    private:
      Fct _fct;
    };

    spawn(ctx, std::unique_ptr<open_file_task>(new open_file_task_t(filepath, flags, std::move(fct))));
  }
};

///
///
///
class read_file_task : public task_base {
public:
  inline read_file_task(const file_handle& fd)
      : _fd(fd) {}
  virtual ~read_file_task() = default;

protected:
  virtual void run(io_context& ctx, const file_handle& fd, mts::byte_view data) = 0;

private:
  friend class io_context;
  class request_impl;
  file_handle _fd;

  static void spawn(io_context& ctx, std::unique_ptr<read_file_task> t);

  template <typename Fct>
  static void spawn(io_context& ctx, const file_handle& fd, Fct&& fct) {
    class read_file_task_t : public read_file_task {
    public:
      read_file_task_t(const file_handle& fd, Fct&& fct)
          : read_file_task(fd)
          , _fct(std::move(fct)) {}

      virtual ~read_file_task_t() override = default;

    protected:
      virtual void run(io_context& ctx, const file_handle& fd, mts::byte_view data) override { _fct(ctx, fd, data); }

    private:
      Fct _fct;
    };

    spawn(ctx, std::unique_ptr<read_file_task>(new read_file_task_t(fd, std::move(fct))));
  }
};

///
///
///
class read_file_buffer_task : public task_base {
public:
  inline read_file_buffer_task(const file_handle& fd, mts::memory_range<char> buffer)
      : _fd(fd)
      , _buffer(buffer) {}
  virtual ~read_file_buffer_task() = default;

protected:
  virtual void run(io_context& ctx, const file_handle& fd, mts::memory_range<char> data) = 0;

private:
  friend class io_context;
  class request_impl;
  file_handle _fd;
  mts::memory_range<char> _buffer;

  static void spawn(io_context& ctx, std::unique_ptr<read_file_buffer_task> t);

  template <typename Fct>
  static void spawn(io_context& ctx, const file_handle& fd, mts::memory_range<char> buffer, Fct&& fct) {
    class read_file_buffer_task_t : public read_file_buffer_task {
    public:
      read_file_buffer_task_t(const file_handle& fd, mts::memory_range<char> buffer, Fct&& fct)
          : read_file_buffer_task(fd, buffer)
          , _fct(std::move(fct)) {}

      virtual ~read_file_buffer_task_t() override = default;

    protected:
      virtual void run(io_context& ctx, const file_handle& fd, mts::memory_range<char> data) override {
        _fct(ctx, fd, data);
      }

    private:
      Fct _fct;
    };

    spawn(ctx, std::unique_ptr<read_file_buffer_task>(new read_file_buffer_task_t(fd, buffer, std::move(fct))));
  }
};

///
///
///
class file_stat_task : public task_base {
public:
  inline file_stat_task(const file_handle& fd)
      : _fd(fd) {}
  virtual ~file_stat_task() = default;

protected:
  virtual void run(io_context& ctx, const file_handle& fd, const fs_stat& st) = 0;

private:
  friend class io_context;
  class request_impl;
  file_handle _fd;

  static void spawn(io_context& ctx, std::unique_ptr<file_stat_task> t);

  template <typename Fct>
  static void spawn(io_context& ctx, const file_handle& fd, Fct&& fct) {
    class file_stat_task_t : public file_stat_task {
    public:
      file_stat_task_t(const file_handle& fd, Fct&& fct)
          : file_stat_task(fd)
          , _fct(std::move(fct)) {}

      virtual ~file_stat_task_t() override = default;

    protected:
      virtual void run(io_context& ctx, const file_handle& fd, const fs_stat& st) override { _fct(ctx, fd, st); }

    private:
      Fct _fct;
    };

    spawn(ctx, std::unique_ptr<file_stat_task>(new file_stat_task_t(fd, std::move(fct))));
  }
};
MTS_END_NAMESPACE
