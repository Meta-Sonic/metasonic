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
#include "mts/ui/object.h"
#include "mts/ui/view.h"
#include "mts/ui/window.h"
#include "mts/ui/event.h"

MTS_BEGIN_NAMESPACE
class native_view : public mts::object, private event_manager_proxy {
public:
  native_view(view& __view);
  virtual ~native_view();

  virtual void set_bounds(const _VMTS::rect<int>& bounds) = 0;
  virtual _VMTS::rect<int> get_bounds() const = 0;

  virtual _VMTS::padding<int> get_frame_padding() const = 0;

  virtual void set_title(const std::string& title) = 0;

  inline view& get_view() {

    mts_assert(_view_ptr.is_valid(), "");
    return _view;
  }
  inline const view& get_view() const {

    mts_assert(_view_ptr.is_valid(), "");
    return _view;
  }

  inline window* get_window() {
    return get_view().type() == view::type::window ? static_cast<window*>(&get_view()) : nullptr;
  }

  inline const window* get_window() const {
    return get_view().type() == view::type::window ? static_cast<const window*>(&get_view()) : nullptr;
  }

  //  virtual void set_size_constrain(const _VMTS::rect& bounds) = 0;

  virtual void set_type(window::type __type) = 0;
  virtual void set_visible(bool visible) = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void close() = 0;
  virtual void center() = 0;
  virtual void grab_focus() = 0;

  virtual bool has_focus() const = 0;

  virtual void set_cursor(cursor_type ct) { _VMTS::unused(ct); }

  virtual bool dragged_after_creation() const { return false; }

  /// Sets this window to either be always-on-top or normal.
  /// Some kinds of window might not be able to do this, so should return false.
  virtual bool set_always_on_top(bool value) = 0;

  /// Brings the window to the top, optionally also giving it keyboard focus.
  virtual void to_front(bool with_focus) = 0;

  /// Moves the window to be just behind another one.
  virtual void to_behind(native_view* other) = 0;

  virtual void repaint(const _VMTS::rect<float>& area) = 0;

  virtual _VMTS::point<int> get_screen_position() const = 0;

  _VMTS::size<int> get_size_constrain(const _VMTS::size<int>& frame_size) const;

  void set_view_under_mouse(_VMTS::weak_managed_ptr<view> v, const mouse_event& evt);
  void set_selected_view(_VMTS::weak_managed_ptr<view> v, const mouse_event& evt);
  void set_focused_view(_VMTS::weak_managed_ptr<view> v);

  _VMTS::weak_managed_ptr<view> get_focused_view() const { return _focused_view; }

  inline void release_selected_view(view* v) {
    if (_selected_view == v) {
      _selected_view = nullptr;
    }
  }

  void handle_mouse_move(const mouse_event::common& cevt);
  bool handle_mouse_left_down(const mouse_event::common& cevt);
  bool handle_mouse_left_up(const mouse_event::common& cevt);
  bool handle_mouse_right_down(const mouse_event::common& cevt);
  bool handle_mouse_right_up(const mouse_event::common& cevt);
  bool handle_mouse_left_drag(const mouse_event::common& cevt);
  bool handle_mouse_right_drag(const mouse_event::common& cevt);
  bool handle_mouse_wheel(const mouse_event::common& cevt, const scroll_event& sevt);

  void handle_focus_gained();
  void handle_focus_lost();

  bool handle_key_down(const key_event& kevt);
  bool handle_key_up(const key_event& kevt);

  //  void handle_paint(_VMTS::context& g);
  void handle_paint(mts::context& g, const mts::memory_range<const _VMTS::rect<float>>& dirty_rects);
  void handle_bounds_changed();

  bool has_main_thread();
  void handle_set_main_thread();

  static bool has_child_in_rect(const view* v, const _VMTS::rect<int>& bounds);

  static bool is_valid_native_view(_VMTS::not_null<native_view*> nv);

private:
  view& _view;
  _VMTS::weak_managed_ptr<view> _view_ptr;

  static _VMTS::weak_managed_ptr<view> _selected_view;
  static _VMTS::weak_managed_ptr<view> _focused_view;
  static _VMTS::weak_managed_ptr<view> _view_under_mouse;

  struct found_view_info {
    //    view* view;
    _VMTS::weak_managed_ptr<view> view;
    _VMTS::point<float> relative_position;

    inline explicit operator bool() const { return view != nullptr; }
  };

  found_view_info find_view_under_position(view* v, const _VMTS::point<float>& p);

  friend class view;
  friend class window;
  static void add_to_desktop(view* v, window::type type, window::style_flags flags, void* nativeWindowToAttachTo);

  //  static void handle_paint_all(_VMTS::context& g, view* v, const _VMTS::point<float>& delta);
  static void handle_paint(mts::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta);
};

MTS_END_NAMESPACE
