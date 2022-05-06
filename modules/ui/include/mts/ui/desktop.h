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
#include "mts/ui/window.h"
#include "mts/pointer.h"
#include <vector>

MTS_BEGIN_NAMESPACE
struct display {
  _VMTS::rect<int> frame;
  _VMTS::rect<int> visible_frame;
  double dpi;
  double scale;
  bool is_main;
};

class modal_manager {
public:
  void add(const _VMTS::managed_ptr<view>& v);
  void add(_VMTS::managed_ptr<view>&& v);
  void remove(_VMTS::not_null<view*> v);
  bool has(_VMTS::not_null<view*> v);

private:
  std::vector<_VMTS::managed_ptr<view>> _modal_views;
};

class desktop {
public:
  static desktop& get_instance();

  static const std::vector<display>& get_displays();
  static const display* get_main_display();

  static modal_manager& get_modal_manager();

private:
  friend class view;
  friend class window;
  friend class native_view;

  std::vector<view*> _views;
  std::vector<native_view*> _native_views;
  std::vector<display> _displays;
  modal_manager _modal_manager;

  desktop();
  desktop(const desktop&) = delete;
  desktop(desktop&&) = delete;
  ~desktop();
  desktop& operator=(const desktop&) = delete;
  desktop& operator=(desktop&&) = delete;

  void add(_VMTS::not_null<view*> v);
  void remove(_VMTS::not_null<view*> v);

  void add(_VMTS::not_null<native_view*> nv);
  void remove(_VMTS::not_null<native_view*> nv);
  bool has(_VMTS::not_null<native_view*> nv);

  struct native_desktop {
    static void init(desktop* d);
  };
};

// MouseInputSource* getOrCreateMouseInputSource (MouseInputSource::InputSourceType type, int touchIndex = 0)
//{
//     if (type == MouseInputSource::InputSourceType::mouse || type == MouseInputSource::InputSourceType::pen)
//     {
//         for (auto& m : sourceArray)
//             if (type == m.getType())
//                 return &m;
//
//         addSource (0, type);
//     }
//     else if (type == MouseInputSource::InputSourceType::touch)
//     {
//         jassert (touchIndex >= 0 && touchIndex < 100); // sanity-check on number of fingers
//
//         for (auto& m : sourceArray)
//             if (type == m.getType() && touchIndex == m.getIndex())
//                 return &m;
//
//         if (canUseTouch())
//             return addSource (touchIndex, type);
//     }
//
//     return nullptr;
// }

MTS_END_NAMESPACE
