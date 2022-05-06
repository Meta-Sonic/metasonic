#pragma once
#include "mts/ui/view.h"

#include "mts/ui/popup_window.h"

class settings_view : public mts::view {
public:
  settings_view();

  virtual ~settings_view() override;
  static void show(_VMTS::view& parent, const _VMTS::point<int>& position);

protected:
  //    virtual void view_visibility_changed(mts::view& v) override;
  virtual void bounds_changed(const mts::view::bounds_status& bs) override;
  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) override;

private:
  _VMTS::weak_managed_ptr<mts::window> _settings_window;
};

class settings_window : public mts::window, private mts::view::listener {
public:
  virtual ~settings_window() override;

  //  void close();

  static mts::managed_ptr<settings_window> create(
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
  bool _close_on_escape = true;
  bool _close_on_return_key = false;

  settings_window(mts::view& __view, mts::managed_ptr<mts::view>&& content);
};
