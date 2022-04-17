#include <gtest/gtest.h>
#include "mts/print.h"
#include "mts/event/io_context.h"

namespace {
inline const mts::filesystem::path filepath = MTS_TEST_RESOURCES_DIRECTORY "/banana.txt";
inline constexpr std::size_t file_size = 12;

void read_file_callback(mts::io_context& ctx, const mts::file_handle& fd, mts::byte_view data) {
  static int counter = 0;

  EXPECT_FALSE(fd.ec);
  if (fd.ec) {
    mts::errprint(fd.ec.message());
    fd.close();
    return;
  }

  if (data.empty()) {
    EXPECT_EQ(counter, 1);
    fd.close();
    return;
  }

  counter++;
  EXPECT_EQ(std::string_view((const char*)data.data(), data.size()), "BANANA Bacon");
  ctx.spawn<mts::read_file_task>(fd, &read_file_callback);
}

TEST(event, fs) {
  mts::io_context ctx;
  ctx.spawn<mts::open_file_task>(filepath, O_RDONLY, [&](mts::io_context& ctx, const mts::file_handle& fd) {
    EXPECT_FALSE(fd.ec);
    if (fd.ec) {
      mts::errprint(fd.ec.message());
      return;
    }

    ctx.spawn<mts::read_file_task>(fd, &read_file_callback);
  });

  ctx.run();
}

TEST(event, fssize) {
  mts::io_context ctx;
  ctx.spawn<mts::open_file_task>(filepath, O_RDONLY, [&](mts::io_context& ctx, const mts::file_handle& fd) {
    if (fd.ec) {
      mts::errprint(fd.ec.message());
      return;
    }

    ctx.spawn<mts::file_stat_task>(fd, [](mts::io_context& ctx, const mts::file_handle& fd, const mts::fs_stat& st) {
      EXPECT_FALSE(fd.ec);
      if (fd.ec) {
        mts::errprint(fd.ec.message());
        return;
      }

      EXPECT_EQ(st.st_size, file_size);
      fd.close();
    });
  });

  ctx.run();
}
} // namespace
