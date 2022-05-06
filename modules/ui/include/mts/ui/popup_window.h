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

MTS_BEGIN_NAMESPACE
class popup_window : public mts::window, private mts::view::listener {
public:
  virtual ~popup_window() override;

  //  void close();

  static mts::managed_ptr<popup_window> create(
      mts::view& parent, mts::managed_ptr<mts::view>&& content, const mts::point<int>& position);

  inline const mts::managed_ptr<mts::view>& get_content() const { return _content; }

  inline mts::weak_managed_ptr<mts::view> get_weak_content() { return _content; }

  inline void set_close_on_left_up_outside(bool v) { _close_on_left_up_outside = v; }
  inline void set_close_on_escape_key(bool v) { _close_on_escape = v; }
  inline void set_close_on_return_key(bool v) { _close_on_return_key = v; }

protected:
  virtual void view_bounds_changed(mts::view& v, const mts::view::bounds_status& bs) override;
  virtual void view_being_deleted(mts::view& v) override;
  virtual void view_visibility_changed(mts::view& v) override;
  virtual void view_focus_changed(mts::view& v, bool focused) override;

  virtual void mouse_left_up(const mts::mouse_event& evt) override;
  virtual bool key_pressed(const mts::key_event& kevt) override;

private:
  mts::view& _view;
  mts::managed_ptr<mts::view> _content;
  bool _close_on_left_up_outside = false;
  bool _close_on_escape = false;
  bool _close_on_return_key = false;

  popup_window(mts::view& __view, mts::managed_ptr<mts::view>&& content);
};
MTS_END_NAMESPACE
