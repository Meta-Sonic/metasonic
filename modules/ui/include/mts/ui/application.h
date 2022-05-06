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
#include "mts/ui/event.h"
#include <memory>

MTS_BEGIN_NAMESPACE
class application_interface;

class application : private mts::event_manager_proxy {
public:
  application() = default;
  virtual ~application();

  template <typename _AppType, typename... Args>
  static int create(int argc, const char* argv[], Args&&... args) {
    return create_application_impl(std::make_shared<_AppType>(std::forward<Args>(args)...), argc, argv);
  }

  static int create(std::shared_ptr<application>&& app, int argc, const char* argv[]) {
    return create_application_impl(std::move(app), argc, argv);
  }

  virtual void init() = 0;
  virtual void shutdown() {}

  static bool is_standalone() { return !app_data.app.expired(); }

private:
  friend class application_interface;
  struct application_data {
    std::weak_ptr<application> app;
  };

  static application_data app_data;

  static int create_application_impl(std::shared_ptr<application>&& app, int argc, const char* argv[]);
};

MTS_END_NAMESPACE
