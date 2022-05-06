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
#include "mts/pointer.h"
#include "mts/util.h"
#include "mts/print.h"
#include "mts/flags.h"
#include "mts/component.h"

#include "mts/graphics/context.h"
#include "mts/graphics/padding.h"
#include "mts/graphics/geometry.h"
#include "mts/graphics/components/geometries.h"
#include "mts/graphics/components/clipping.h"

#include "mts/ui/object.h"
#include "mts/ui/key_event.h"
#include "mts/ui/mouse_event.h"
#include "mts/ui/asset_manager.h"
#include "mts/ui/listener.h"
#include "mts/ui/style.h"

#include "mts/ui/components/property.h"

#include <memory>
#include <vector>
#include <unordered_map>

MTS_BEGIN_NAMESPACE
class native_view;
class window;

enum class cursor_type { normal, move, resize, text };

class view : public mts::object {
public:
  static constexpr std::size_t invalid_index = std::numeric_limits<std::size_t>::max();

  enum class type { view, window };

  view();
  view(const std::string& name);
  virtual ~view() override;

  inline enum type type() const { return _type; }

  //
  // MARK: Bounds.
  //

  struct bounds_status;

  void set_position(const mts::point<int>& pos);
  void set_size(const mts::size<int>& size);
  void set_bounds(const mts::rect<int>& bounds);
  inline void set_x(int x) { set_position({ x, _bounds.y }); }
  inline void set_y(int y) { set_position({ _bounds.x, y }); }
  inline void set_width(int w) { set_size({ w, _bounds.height }); }
  inline void set_height(int h) { set_size({ _bounds.width, h }); }
  inline void move_x(int x) { set_position({ _bounds.x + x, _bounds.y }); }
  inline void move_y(int y) { set_position({ _bounds.x, _bounds.y + y }); }

  const mts::point<int>& get_position() const noexcept;
  const mts::size<int>& get_size() const noexcept;
  const mts::rect<int>& get_bounds() const noexcept;
  inline mts::rect<float> get_fbounds() const noexcept;
  inline mts::rect<int> get_local_bounds() const noexcept { return mts::rect<int>({ 0, 0 }, _bounds.size); }
  inline mts::rect<float> get_local_fbounds() const noexcept { return get_local_bounds(); }
  inline mts::point<int> get_absolute_position() const noexcept;
  inline mts::rect<int> get_absolute_bounds() const noexcept;
  inline mts::rect<float> get_absolute_fbounds() const noexcept;
  mts::point<int> get_screen_position() const noexcept;
  mts::rect<int> get_screen_bounds() const noexcept;

  inline int get_x() const noexcept { return get_position().x; }
  inline int get_y() const noexcept { return get_position().y; }
  inline int get_width() const noexcept { return get_size().width; }
  inline int get_height() const noexcept { return get_size().height; }

  inline void set_drawing_margin(const mts::margin<float>& margin) noexcept;
  inline bool has_drawing_margin() const noexcept;
  inline const mts::margin<float>& get_drawing_margin() const noexcept { return _drawing_margin; }

  inline mts::rect<float> get_drawing_bounds() const noexcept;
  inline mts::rect<float> get_absolute_drawing_bounds() const noexcept;

  //
  // MARK: Native view.
  //

  void set_cursor(cursor_type ct);
  inline native_view* get_native_view() noexcept { return _native_view.get(); }
  inline const native_view* get_native_view() const noexcept { return _native_view.get(); }
  inline bool has_native_view() const noexcept { return _native_view.get() != nullptr; }

  //
  // MARK: Window.
  //
  void add_to_desktop();

  //
  // MARK: Chilrend.
  //

  using children_vector = std::vector<mts::managed_ptr<view>>;
  using reference = typename children_vector::reference;
  using const_reference = typename children_vector::const_reference;
  using iterator = typename children_vector::iterator;
  using const_iterator = typename children_vector::const_iterator;
  using reverse_iterator = typename children_vector::reverse_iterator;
  using const_reverse_iterator = typename children_vector::const_reverse_iterator;

  void add(mts::managed_ptr<view>&& v, bool notify_style_changed = true);

  inline void add(view* v, bool notify_style_changed = true) {
    add(mts::managed_ptr<view>(v, false), notify_style_changed);
  }

  inline void add(view& v, bool notify_style_changed = true) {
    add(mts::managed_ptr<view>(&v, false), notify_style_changed);
  }

  inline void add(std::unique_ptr<view>&& v, bool notify_style_changed = true) {
    add(mts::managed_ptr<view>(std::move(v)), notify_style_changed);
  }

  inline void add(std::shared_ptr<view> v, bool notify_style_changed = true) {
    add(mts::managed_ptr<view>(v), notify_style_changed);
  }

  inline void add(view* v, const mts::rect<int>& bounds, bool notify_style_changed = true);
  inline void add(view& v, const mts::rect<int>& bounds, bool notify_style_changed = true);
  inline void add(mts::managed_ptr<view>&& v, const mts::rect<int>& bounds, bool notify_style_changed = true);
  inline void add(std::unique_ptr<view>&& v, const mts::rect<int>& bounds, bool notify_style_changed = true);
  inline void add(std::shared_ptr<view> v, const mts::rect<int>& bounds, bool notify_style_changed = true);
  inline void add(view* v, const mts::point<int>& pos, bool notify_style_changed = true);
  inline void add(view& v, const mts::point<int>& pos, bool notify_style_changed = true);
  inline void add(mts::managed_ptr<view>&& v, const mts::point<int>& pos, bool notify_style_changed = true);
  inline void add(std::unique_ptr<view>&& v, const mts::point<int>& pos, bool notify_style_changed = true);
  inline void add(std::shared_ptr<view> v, const mts::point<int>& pos, bool notify_style_changed = true);

  template <typename T>
  inline void add(mts::managed_ptr<T>&& v, const mts::rect<int>& bounds, bool notify_style_changed = true) {
    view* raw = v.get();

    add(std::move(v), notify_style_changed);
    raw->set_bounds(bounds);
  }

  void remove(view* v, bool notify_parent_changed = true, bool notify_style_changed = false);

  inline void remove(view& v, bool notify_parent_changed = true, bool notify_style_changed = false) {
    remove(&v, notify_parent_changed, notify_style_changed);
  }

  inline std::size_t children_count() const { return _children.size(); }
  inline bool has_children() const { return _children.size(); }
  inline children_vector& get_children() { return _children; }
  inline const children_vector& get_children() const { return _children; }
  inline bool has_child(const view* v) const { return get_child_index(v) != invalid_index; }
  inline bool has_child(object::uid c_id) const { return get_child_index(c_id) != invalid_index; }
  inline std::size_t get_child_index(const view* v) const;
  inline std::size_t get_child_index(object::uid c_id) const;
  inline std::size_t get_child_index(std::string_view name) const;

  inline mts::managed_ptr<view> get_child(const view* v) const;
  inline mts::managed_ptr<view> get_child(object::uid c_id) const;
  inline mts::managed_ptr<view> get_child(std::string_view name) const;

  inline iterator begin() { return _children.begin(); }
  inline const_iterator begin() const { return _children.begin(); }
  inline iterator end() { return _children.end(); }
  inline const_iterator end() const { return _children.end(); }
  inline reverse_iterator rbegin() noexcept { return _children.rbegin(); }
  inline const_reverse_iterator rbegin() const noexcept { return _children.rbegin(); }
  inline reverse_iterator rend() noexcept { return _children.rend(); }
  inline const_reverse_iterator rend() const noexcept { return _children.rend(); }
  inline const_iterator cbegin() const noexcept { return _children.cbegin(); }
  inline const_iterator cend() const noexcept { return _children.cend(); }
  inline const_reverse_iterator crbegin() const noexcept { return _children.crbegin(); }
  inline const_reverse_iterator crend() const noexcept { return _children.crend(); }
  inline reference front() noexcept { return _children.front(); }
  inline const_reference front() const noexcept { return _children.front(); }
  inline reference back() noexcept { return _children.back(); }
  inline const_reference back() const noexcept { return _children.back(); }
  inline reference operator[](std::size_t index) noexcept { return _children[index]; }
  inline const_reference operator[](std::size_t index) const noexcept { return _children[index]; }
  inline reference at(std::size_t index) { return _children.at(index); }
  inline const_reference at(std::size_t index) const { return _children.at(index); }

  //
  // MARK: Parent.
  //

  inline mts::weak_managed_ptr<view> get_parent() { return _parent; }
  inline mts::weak_managed_ptr<const view> get_parent() const { return _parent; }
  inline mts::weak_managed_ptr<const view> get_root_parent() const;
  inline mts::weak_managed_ptr<view> get_root_parent();
  inline bool is_root() const;

  bool is_parent_of(const view* child) const noexcept;

  void to_front();

  //
  // MARK: Others.
  //
  inline void set_name(const std::string& name) { _name = name; }

  inline const std::string& get_name() const noexcept { return _name; }

  inline void set_clipping(bool clipping) noexcept { mts::set_flag(f_is_clipped, clipping, _flags); }
  inline bool is_clipped() const noexcept { return mts::has_flag(f_is_clipped, _flags); }
  inline bool is_opaque() const noexcept { return mts::has_flag(f_is_opaque, _flags) && _opacity == 1; }
  inline bool is_visible() const noexcept { return mts::has_flag(f_is_visible, _flags); }

  inline void set_wants_mouse_move(bool value) noexcept { mts::set_flag(f_wants_mouse_move, value, _flags); }
  inline bool wants_mouse_move() const noexcept { return mts::has_flag(f_wants_mouse_move, _flags); }

  inline void set_wants_keyboard_focus(bool value) noexcept { mts::set_flag(f_wants_keyboard_focus, value, _flags); }
  inline bool wants_keyboard_focus() const noexcept { return mts::has_flag(f_wants_keyboard_focus, _flags); }

  inline void set_wants_unfocused_key_events(bool active) noexcept {
    mts::set_flag(view::flags::f_unfocused_key_events, active, view::_flags);
  }

  inline bool wants_unfocused_key_events() const noexcept {
    return mts::has_flag(view::flags::f_unfocused_key_events, view::_flags);
  }

  void set_visible(bool visible);
  void set_opacity(float opacity);
  inline float get_opacity() const noexcept { return _opacity; }

  void grab_focus();
  void ungrab_focus();

  //
  // MARK: Listeners.
  //

  class listener {
  public:
    virtual ~listener() = default;

    virtual void view_bounds_changed(view& v, const bounds_status& bs) { mts::unused(v, bs); }
    virtual void view_visibility_changed(view& v) { mts::unused(v); }

    virtual void view_focus_changed(view& v, bool focused) { mts::unused(v, focused); }

    /// Called when the view is in the process of being deleted.

    /// This callback is made from inside the destructor, so be very, very cautious
    /// about what you do in here.

    /// In particular, bear in mind that it's the view base class's destructor that calls
    /// this - so if the object that's being deleted is a subclass of view, then the
    /// subclass layers of the object will already have been destructed when it gets to this
    /// point!
    virtual void view_being_deleted(view& v) { mts::unused(v); }
  };

  class listeners {
  public:
    listeners() = delete;
    listeners(const listeners&) = delete;
    listeners(listeners&&) = delete;
    ~listeners() = default;
    listeners& operator=(const listeners&) = delete;
    listeners& operator=(listeners&&) = delete;

    inline bool empty() const { return !(bool)_listeners; }

    inline void add(mts::not_null<listener*> l) { get_ref().add(l); }

    inline void remove(mts::not_null<listener*> l) {
      if (!empty()) {
        _listeners->remove(l);
      }
    }

  private:
    friend class view;
    friend class native_view;

    listeners(view& v)
        : _view(v) {}

    view& _view;
    std::unique_ptr<listener_list<listener>> _listeners;
    inline listener_list<listener>& get_ref() { return mts::get_ref(_listeners); }

    template <typename Fct, typename... Args>
    inline void notify(Fct fct, Args&&... args) {
      if (empty()) {
        return;
      }

      _listeners->call(fct, std::forward<Args>(args)...);
    }
  } listeners;

  //
  // MARK: Attributes / Style.
  //

  inline std::shared_ptr<const mts::style> get_style() const;
  inline void set_style(std::shared_ptr<const mts::style> s);
  inline void set_style(const std::string& style_name);
  inline void set_style(std::nullptr_t);

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline void set_attribute(const style::value_t<_EnumValue>& v) {
    _attributes.set<_EnumValue>(v);
  }

  template <typename _Style, std::enable_if_t<std::is_base_of_v<mts::style_descriptor_base, _Style>, bool> = true>
  inline void set_attribute(const _Style& v) {
    _attributes.set<_Style>(v);
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline void erase_attribute() {
    _attributes.erase<_EnumValue>();
  }

  template <auto _EnumValue, std::enable_if_t<is_style_enum_v<decltype(_EnumValue)>, bool> = true>
  inline const style::value_t<_EnumValue>& get_attribute() const {
    return _attributes.get<_EnumValue>();
  }

  //
  // MARK: Properties.
  //

  bool set_property(const std::string& name, const property& p);
  const property& get_property(const std::string& name) const;
  bool has_property(const std::string& name) const;
  bool remove_property(const std::string& name);

  //
  // MARK: Components.
  //

  template <typename _ComponentType>
  inline bool has_component() const;

  template <typename _ComponentType, typename... Args>
  inline std::shared_ptr<_ComponentType> add_component(Args&&... args);

  template <typename _ComponentType, typename... Args>
  inline std::shared_ptr<_ComponentType> force_get_component(Args&&... args);

  template <typename _ComponentType>
  inline std::shared_ptr<_ComponentType> get_component();

  template <typename _ComponentType>
  inline std::shared_ptr<const _ComponentType> get_component() const;

  //
  // MARK: Geometries.
  //

  inline bool has_geometries() const;

  template <typename T, typename... Args>
  inline T* add_geometry(Args&&... args);

  inline void remove_geometry(const mts::geometry* g);
  inline void remove_geometry(std::size_t index);

  inline mts::geometry* get_geometry(std::size_t index);
  inline const mts::geometry* get_geometry(std::size_t index) const;

  inline void draw_geometries(mts::context& gc);
  inline void draw_geometries(mts::context& gc, const mts::rect<float>& dirty_rect);
  inline void draw_geometries(mts::context& gc, const mts::memory_range<const mts::rect<float>>& dirty_rects);

protected:
  //
  // MARK: Parent / Children.
  //

  virtual void parent_changed() {}
  virtual void children_changed() {}
  virtual void child_added(mts::view* child) { mts::unused(child); }
  virtual void child_removed(mts::view* child) { mts::unused(child); }

  //
  // MARK: Bounds.
  //

  virtual void bounds_changed(const bounds_status& bs) { mts::unused(bs); }

  //
  // MARK: Style / Properties.
  //

  virtual void attribute_changed(std::string_view group, std::string_view name) { mts::unused(group, name); }
  virtual void style_changed() {}
  virtual void property_changed(const std::string& name) { mts::unused(name); }

  //
  // MARK: Mouse.
  //

  inline void set_no_mouse_drag(bool no_drag) { mts::set_flag(f_no_mouse_drag, no_drag, _flags); }
  inline bool get_no_mouse_drag() const { return mts::has_flag(f_no_mouse_drag, _flags); }

  virtual void mouse_left_down(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_left_up(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_right_down(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_right_up(const mouse_event& evt) { mts::unused(evt); }

  virtual void mouse_moved(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_left_drag(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_right_drag(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_wheel(const mouse_event& evt, const scroll_event& sevt);

  /// When the mouse button is pressed and held down while being moved in or out
  /// of a component, none of mouse_enter or mouse_leave are called.
  virtual void mouse_enter(const mouse_event& evt) { mts::unused(evt); }
  virtual void mouse_leave() {}

  //
  // MARK: Keyboard.
  //

  virtual bool key_pressed(const key_event& kevt) {
    mts::unused(kevt);
    return false;
  }

  //
  // MARK: Visibility / Focus.
  //

  inline void set_opaque(bool opaque) { mts::set_flag(f_is_opaque, opaque, _flags); }

  virtual void visibily_changed() {}
  virtual void focus_gained() {}
  virtual void focus_lost() {}

  //
  // MARK: Paint.
  //

  void repaint();
  void repaint(const mts::rect<float>& area);

  virtual void paint(mts::context& gc, const mts::rect<float>& dirty_rect) { draw_geometries(gc, dirty_rect); }
  virtual void paint_over(mts::context& gc, const mts::rect<float>& dirty_rect) { mts::unused(gc, dirty_rect); }

private:
  friend class native_view;
  friend class window;

  using component_map = std::unordered_map<component::type, std::shared_ptr<component>>;

  enum flags {
    f_is_clipped = mts::get_flag_value(1),
    f_is_opaque = mts::get_flag_value(2),
    f_is_visible = mts::get_flag_value(3),
    f_wants_mouse_move = mts::get_flag_value(4),
    f_wants_keyboard_focus = mts::get_flag_value(5),
    f_no_mouse_drag = mts::get_flag_value(6),
    f_unbounded_mouse_events = mts::get_flag_value(7),
    f_unfocused_key_events = mts::get_flag_value(8),
  };

  class attributes : public mts::style_map_base {
  public:
    std::shared_ptr<const mts::style> get_style() const;
    void set_style(std::shared_ptr<const mts::style> s);
    void set_style(const std::string& style_name);
    void set_style(std::nullptr_t);

  private:
    friend class view;

    virtual const style& default_style() const override;
    virtual void notify(std::string_view group, std::string_view name) override;

    inline attributes(view& v) noexcept
        : _view(v) {}

    view& _view;
    std::shared_ptr<const mts::style> _style;
    mutable std::shared_ptr<const mts::style> _cached_style;
  };

  std::shared_ptr<native_view> _native_view;
  children_vector _children;
  mts::rect<int> _bounds;
  mts::margin<float> _drawing_margin;
  mts::weak_managed_ptr<mts::view> _parent;
  std::unique_ptr<component_map> _components;
  attributes _attributes;
  std::string _name;
  float _opacity = 1;
  flags _flags = (flags)(f_is_clipped | f_wants_mouse_move | f_is_visible | f_wants_keyboard_focus);
  enum type _type = type::view;

  view(enum type __type)
      : view() {
    _type = __type;
  }

  void send_style_changed();
};

//
// MARK: Bounds.
//

struct view::bounds_status {
  enum class changed_type { bounds, position, size };
  changed_type type;

  inline bool size_and_position_changed() const { return type == changed_type::bounds; }
  inline bool position_changed() const { return mts::is_one_of(type, changed_type::bounds, changed_type::position); }
  inline bool size_changed() const { return mts::is_one_of(type, changed_type::bounds, changed_type::size); }
};

mts::rect<float> view::get_fbounds() const noexcept { return get_bounds(); }

mts::point<int> view::get_absolute_position() const noexcept {
  mts::point<int> pos = get_position();

  const mts::view* v = this;
  while (const mts::view* p = v->get_parent()) {
    pos += p->get_position();
    v = p;
  }

  return pos;
}

mts::rect<int> view::get_absolute_bounds() const noexcept {
  return mts::rect<int>(get_absolute_position(), get_size());
}

mts::rect<float> view::get_absolute_fbounds() const noexcept { return get_absolute_bounds(); }

bool view::has_drawing_margin() const noexcept {
  return mts::all_equals(
      0.0f, _drawing_margin.left, _drawing_margin.right, _drawing_margin.top, _drawing_margin.bottom);
}

void view::set_drawing_margin(const mts::margin<float>& margin) noexcept {
  if (mts::assign(_drawing_margin, margin)) {
    repaint();
  }
}

mts::rect<float> view::get_drawing_bounds() const noexcept { return get_fbounds() + _drawing_margin; }

mts::rect<float> view::get_absolute_drawing_bounds() const noexcept { return get_absolute_fbounds() + _drawing_margin; }

//
// MARK: Children.
//

void view::add(view* v, const mts::rect<int>& bounds, bool notify_style_changed) {
  view* raw = v;
  add(mts::managed_ptr<view>(v, false), notify_style_changed);
  raw->set_bounds(bounds);
}

void view::add(view& v, const mts::rect<int>& bounds, bool notify_style_changed) {
  add(mts::managed_ptr<view>(&v, false), notify_style_changed);
  v.set_bounds(bounds);
}

void view::add(mts::managed_ptr<view>&& v, const mts::rect<int>& bounds, bool notify_style_changed) {
  view* raw = (view*)v;
  add(std::move(v), notify_style_changed);
  raw->set_bounds(bounds);
}

void view::add(std::unique_ptr<view>&& v, const mts::rect<int>& bounds, bool notify_style_changed) {
  view* raw = v.get();
  add(mts::managed_ptr<view>(std::move(v)), notify_style_changed);
  raw->set_bounds(bounds);
}

void view::add(std::shared_ptr<view> v, const mts::rect<int>& bounds, bool notify_style_changed) {
  view* raw = v.get();
  add(mts::managed_ptr<view>(v), notify_style_changed);
  raw->set_bounds(bounds);
}

void view::add(view* v, const mts::point<int>& pos, bool notify_style_changed) {
  view* raw = v;
  add(mts::managed_ptr<view>(v, false), notify_style_changed);
  raw->set_position(pos);
}

void view::add(view& v, const mts::point<int>& pos, bool notify_style_changed) {
  add(mts::managed_ptr<view>(&v, false), notify_style_changed);
  v.set_position(pos);
}

void view::add(mts::managed_ptr<view>&& v, const mts::point<int>& pos, bool notify_style_changed) {
  view* raw = (view*)v;
  add(std::move(v), notify_style_changed);
  raw->set_position(pos);
}

void view::add(std::unique_ptr<view>&& v, const mts::point<int>& pos, bool notify_style_changed) {
  view* raw = v.get();
  add(mts::managed_ptr<view>(std::move(v)), notify_style_changed);
  raw->set_position(pos);
}

void view::add(std::shared_ptr<view> v, const mts::point<int>& pos, bool notify_style_changed) {
  view* raw = v.get();
  add(mts::managed_ptr<view>(v), notify_style_changed);
  raw->set_position(pos);
}

std::size_t view::get_child_index(const view* v) const {
  for (std::size_t i = 0; i < _children.size(); i++) {
    if (_children[i].get() == v) {
      return i;
    }
  }

  return invalid_index;
}

std::size_t view::get_child_index(object::uid c_id) const {
  for (std::size_t i = 0; i < _children.size(); i++) {
    if (_children[i]->get_id() == c_id) {
      return i;
    }
  }

  return invalid_index;
}

mts::managed_ptr<view> view::get_child(const view* v) const {
  for (std::size_t i = 0; i < _children.size(); i++) {
    if (_children[i].get() == v) {
      return _children[i];
    }
  }

  return {};
}

mts::managed_ptr<view> view::get_child(object::uid c_id) const {
  for (std::size_t i = 0; i < _children.size(); i++) {
    if (_children[i]->get_id() == c_id) {
      return _children[i];
    }
  }

  return {};
}

inline bool is_valid_child_string(std::string_view str) {
  if (str.empty()) {
    return false;
  }

  std::size_t start = 0;
  std::size_t end = str.find('.');

  if (end == std::string_view::npos) {
    return true;
  }

  while (end != std::string_view::npos) {
    if (str.substr(start, end - start).empty()) {
      return false;
    }

    start = end + 1;
    end = str.find('.', start);
  }

  return !str.substr(start, end).empty();
}

std::size_t view::get_child_index(std::string_view name) const {
  for (std::size_t i = 0; i < _children.size(); i++) {
    if (_children[i]->get_name() == name) {
      return i;
    }
  }
  return invalid_index;
}

mts::managed_ptr<view> view::get_child(std::string_view name) const {
  if (name.empty()) {
    return {};
  }

  std::size_t dot_pos = name.find('.');

  if (dot_pos == std::string::npos) {
    for (std::size_t i = 0; i < _children.size(); i++) {
      if (_children[i]->get_name() == name) {
        return _children[i];
      }
    }
    return {};
  }

  if (!is_valid_child_string(name)) {
    return {};
  }

  std::size_t start = 0;
  std::size_t end = dot_pos;

  const view* child = this;
  while (end != std::string::npos) {
    std::size_t s_index = child->get_child_index(name.substr(start, end - start));

    if (s_index == invalid_index) {
      return {};
    }

    child = child->get_children()[s_index].get();

    std::cout << "--------" << name.substr(start, end - start) << std::endl;
    start = end + 1;
    end = name.find('.', start);
  }

  std::string_view key = name.substr(start, end);

  return child->get_child(key);
}

//
// MARK: Parent.
//

bool view::is_root() const { return !_parent && get_native_view(); }

mts::weak_managed_ptr<mts::view> view::get_root_parent() {
  if (!_parent) {
    return nullptr;
  }

  mts::weak_managed_ptr<mts::view> p = _parent;

  while (p->get_parent()) {
    p = p->get_parent();
  }

  return p;
}

mts::weak_managed_ptr<const mts::view> view::get_root_parent() const {
  if (!_parent) {
    return nullptr;
  }

  mts::weak_managed_ptr<const mts::view> p = _parent;

  while (p->get_parent()) {
    p = p->get_parent();
  }

  return p;
}

//
// MARK: Components.
//

template <typename _ComponentType>
bool view::has_component() const {
  if (!_components || _components->empty()) {
    return false;
  }

  return _components->find(component::get_component_type<_ComponentType>()) != _components->end();
}

template <typename _ComponentType, typename... Args>
std::shared_ptr<_ComponentType> view::add_component(Args&&... args) {
  std::shared_ptr<_ComponentType> c = std::make_shared<_ComponentType>(std::forward<Args>(args)...);
  mts::get_ref(_components).emplace(component::get_component_type<_ComponentType>(), c);
  return c;
}

template <typename _ComponentType, typename... Args>
std::shared_ptr<_ComponentType> view::force_get_component(Args&&... args) {
  component_map& cm = mts::get_ref(_components);
  std::shared_ptr<component>& c = cm[component::get_component_type<_ComponentType>()];

  if (c) {
    return std::static_pointer_cast<_ComponentType>(c);
  }

  std::shared_ptr<_ComponentType> sc = std::make_shared<_ComponentType>(std::forward<Args>(args)...);
  c = sc;
  return sc;
}

template <typename _ComponentType>
std::shared_ptr<_ComponentType> view::get_component() {
  if (!_components) {
    return nullptr;
  }

  component_map& cm = *_components;
  if (auto it = cm.find(component::get_component_type<_ComponentType>()); it != cm.end()) {
    return std::static_pointer_cast<_ComponentType>(it->second);
  }

  return nullptr;
}

template <typename _ComponentType>
std::shared_ptr<const _ComponentType> view::get_component() const {
  if (!_components) {
    return nullptr;
  }

  const component_map& cm = *_components;
  if (auto it = cm.find(component::get_component_type<_ComponentType>()); it != cm.end()) {
    return std::static_pointer_cast<const _ComponentType>(it->second);
  }

  return nullptr;
}

//
// MARK: Geometries.
//

bool view::has_geometries() const { return !has_component<mts::geometry_component>(); }

template <typename T, typename... Args>
T* view::add_geometry(Args&&... args) {
  return force_get_component<mts::geometry_component>()->add<T>(std::forward<Args>(args)...);
}

void view::remove_geometry(const mts::geometry* g) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->erase(g);
  }
}

void view::remove_geometry(std::size_t index) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->erase(index);
  }
}

mts::geometry* view::get_geometry(std::size_t index) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->get(index);
  }

  return nullptr;
}

const mts::geometry* view::get_geometry(std::size_t index) const {
  if (std::shared_ptr<const mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->get(index);
  }

  return nullptr;
}

void view::draw_geometries(mts::context& gc) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->draw(gc);
  }
}

void view::draw_geometries(mts::context& gc, const mts::rect<float>& dirty_rect) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->draw(gc, dirty_rect);
  }
}

void view::draw_geometries(mts::context& gc, const mts::memory_range<const mts::rect<float>>& dirty_rects) {
  if (std::shared_ptr<mts::geometry_component> geo = get_component<mts::geometry_component>()) {
    return geo->draw(gc, dirty_rects);
  }
}

//
// MARK: Style.
//

std::shared_ptr<const mts::style> view::get_style() const { return _attributes.get_style(); }
void view::set_style(std::shared_ptr<const mts::style> s) { _attributes.set_style(s); }
void view::set_style(const std::string& style_name) { _attributes.set_style(style_name); }
void view::set_style(std::nullptr_t) { _attributes.set_style(nullptr); }

MTS_END_NAMESPACE
