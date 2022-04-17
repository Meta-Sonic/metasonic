#include <gtest/gtest.h>
#include "mts/print.h"
#include "mts/event/io_context.h"
#include "mts/event/fstream.h"
namespace {
using read_buffer = std::array<char, 4>;

void read_file_callback(mts::fstream& file, mts::memory_range<char> data, mts::io_context& ctx,
    mts::memory_range<char> buffer, std::vector<char>& vec) {
  mts::print("READ", buffer.size(), data);
  if (file.error()) {
    mts::print("ERRRR");
  }
  else if (data.empty()) {
    mts::print("DONE");
  }
  else {
    vec.insert(vec.end(), data.begin(), data.end());
    file.read(ctx, buffer, &read_file_callback, std::ref(ctx), buffer, std::ref(vec));
  }
}

void open_file_callback(
    mts::fstream& file, mts::io_context& ctx, mts::memory_range<char> buffer, std::vector<char>& vec) {
  mts::print("OPEN", buffer.size());

  if (file.error()) {
    mts::print("ERRRR");
  }
  else {
    file.read(ctx, buffer, &read_file_callback, std::ref(ctx), buffer, std::ref(vec));
  }
}

TEST(event, fstream) {
  std::vector<char> vec;
  read_buffer buffer;
  mts::io_context ctx;
  mts::fstream file;
  //    mts::memory_range<char> mm(buffer.data(), buffer.size());
  //    ;
  //    file.open(ctx, MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", std::bind(&open_file_callback, std::placeholders::_1,
  //    std::ref(buffer)));
  file.open(ctx, MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", &open_file_callback, std::ref(ctx), buffer, std::ref(vec));
  //    file.open(ctx, MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", &open_file_callback);

  ctx.run();

  EXPECT_EQ(std::string_view(vec.data(), vec.size()), "BANANA Bacon");
}

// TEST(event, fstream) {
//     mts::fstream file;
//     if(!file.open(MTS_TEST_RESOURCES_DIRECTORY "/banana.txt")) {
//         mts::errprint(file.error().message());
//         return;
//     }
//
//     mts::print(file.is_open());
// }

// TEST(event, fstream) {
//     mts::io_context ctx;
//     mts::fstream file;
//     file.open(ctx, MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", [](mts::fstream& file) {
//         mts::print("OPEN");
//
//         if(file.error()) {
//             mts::print("ERRRR");
//         }
//         else {
//
//         }
//     });
//     ctx.run();
// }

// class ropp : public mts::read_file_task {
// public:
//   ropp(const mts::file_handle& fd)
//       : mts::read_file_task(fd) {}
//   virtual ~ropp() override = default;
//
// protected:
//   virtual void run(mts::io_context& ctx, const mts::file_handle& fd, mts::byte_view data) override {
//     mts::print("ropp::run");
//
//     if (fd.ec) {
//       mts::errprint(fd.ec.message());
//       return;
//     }
//
//     if (data.empty()) {
//       mts::print("Done reading");
//       return;
//     }
//       mts::print("Ropp cContent", data);
//     ctx.spawn(std::make_unique<ropp>(fd));
//   }
// };
//
// class opp : public mts::open_file_task {
// public:
//   opp(const mts::filesystem::path& filepath, int flags)
//       : open_file_task(filepath, flags) {}
//   virtual ~opp() override = default;
//
// protected:
//   virtual void run(mts::io_context& ctx, const mts::file_handle& fd) override {
//     mts::print("opp::run");
//
//     if (fd.ec) {
//       mts::errprint(fd.ec.message());
//     }
//     else {
//
//       //            ctx.add_read_file(std::make_unique<ropp>(fd));
//
//       ctx.spawn<mts::read_file_task>(fd, [](mts::io_context& ctx, const mts::file_handle& fd, mts::byte_view data) {
//         mts::print("ropp::run");
//
//         if (fd.ec) {
//           mts::errprint(fd.ec.message());
//           return;
//         }
//
//         if (data.empty()) {
//           mts::print("Done reading");
//           return;
//         }
//
//         mts::print("Content", data);
//         ctx.spawn(std::make_unique<ropp>(fd));
//       });
//
//       //            ctx.add_read_file(fd, [](mts::io_context& ctx, const mts::filesystem_result& fd,
//       //            mts::byte_view data) {
//       //                mts::print("ropp::run");
//       //
//       //                if (fd.ec) {
//       //                    mts::errprint(fd.ec.message());
//       //                    return;
//       //                }
//       //
//       //                if(data.empty()) {
//       //                    mts::print("Done reading");
//       //                    return;
//       //                }
//       //
//       //                mts::print("Content", data);
//       //                ctx.add_read_file(std::make_unique<ropp>(fd));
//       //            });
//     }
//   }
// };
//
// TEST(event, simple) {
//
//   {
//     mts::io_context ctx;
//     ctx.spawn<mts::task>([](mts::io_context& ctx) { mts::print("Task 1"); });
//     ctx.spawn<mts::task>([](mts::io_context& ctx) { mts::print("Task 2"); });
//
//       std::array<char, 4> buffer;
//
//     //                ctx.add_task([]() {
//     //                    mts::print("Task 1");
//     //                });
//
//     //        ctx.add_task([]() {
//     //            mts::print("Task 2");
//     //        });
//
//     //        ctx.spawn(std::make_unique<opp>(MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", O_RDONLY));
//
//     ctx.spawn<mts::open_file_task>(
//         MTS_TEST_RESOURCES_DIRECTORY "/banana.txt", O_RDONLY, [&](mts::io_context& ctx, const mts::file_handle& fd) {
//           mts::print("opp::run");
//
//           if (fd.ec) {
//             mts::errprint(fd.ec.message());
//           }
//           else {
//
//             //            ctx.add_read_file(std::make_unique<ropp>(fd));
//
//             ctx.spawn<mts::file_stat_task>(
//                 fd, [](mts::io_context& ctx, const mts::file_handle& fd, const mts::fs_stat& st) {
//                   mts::print("ropp::ft", st.st_size);
//                 });
//
//               ctx.spawn<mts::read_file_buffer_task>(
//                   fd, mts::memory_range<char>(buffer.data(), buffer.size()), [](mts::io_context& ctx, const
//                   mts::file_handle& fd, mts::byte_view data) {
//                     mts::print("ropp::run");
//
//                     if (fd.ec) {
//                       mts::errprint(fd.ec.message());
//                       return;
//                     }
//
//                     if (data.empty()) {
//                       mts::print("Done reading");
//                       return;
//                     }
//
//                     mts::print("Content--", data);
//                     ctx.spawn(std::make_unique<ropp>(fd));
//                   });
//
////            ctx.spawn<mts::read_file_task>(
////                fd, [](mts::io_context& ctx, const mts::file_handle& fd, mts::byte_view data) {
////                  mts::print("ropp::run");
////
////                  if (fd.ec) {
////                    mts::errprint(fd.ec.message());
////                    return;
////                  }
////
////                  if (data.empty()) {
////                    mts::print("Done reading");
////                    return;
////                  }
////
////                  mts::print("Content", data);
////                  ctx.spawn(std::make_unique<ropp>(fd));
////                });
//          }
//        });
//
//    ctx.run();
//  }
//
//  //    {
//  //        mts::io_context ctx;
//  //        ctx.add_task(std::unique_ptr<mts::task>(new mts::task_t(ctx, []() {
//  //            mts::print("BANANANNA");
//  //        })));
//  //
//  //        ctx.add_task([]() {
//  //            mts::print("----AAAABANANANNA");
//  //        });
//  //
//  //        ctx.run();
//  //
//  //        ctx.add_task([]() {
//  //            mts::print("----AAAABANANANNA");
//  //        });
//  //    }
//  //
//  //    mts::print("----AFTER");
//}

} // namespace
