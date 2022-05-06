#include "mts/ui/view.h"
#include "mts/ui/native_view.h"
//
MTS_BEGIN_NAMESPACE
//
view::view()
    : listeners(*this)
    , _attributes(*this) {}
view::view(const std::string& name)
    : listeners(*this)
    , _attributes(*this)
    , _name(name) {}
view::~view() {

  listeners.notify(&listener::view_being_deleted, *this);

  if (_parent) {
    _parent->remove(this);
  }
}

void view::set_cursor(cursor_type ct) {
  if (native_view* nv = get_native_view(); is_root()) {
    nv->set_cursor(ct);
    return;
  }

  if (view* p = get_root_parent()) {
    if (native_view* nv = p->get_native_view()) {
      nv->set_cursor(ct);
    }
  }
}

void view::add(mts::managed_ptr<view>&& v, bool notify_style_changed) {
  // Remove from old parent.
  if (mts::weak_managed_ptr<view> p = v->get_parent()) {
    p->remove(v.get(), false);
  }

  view* raw = v.get();

  child_added(raw);

  _children.push_back(std::move(v));
  raw->_parent = this;

  raw->parent_changed();

  if (notify_style_changed && !raw->_attributes._style && raw->_attributes._cached_style != get_style()) {
    raw->send_style_changed();
  }

  children_changed();
}

void view::remove(view* v, bool notify_parent_changed, bool notify_style_changed) {
  std::size_t index = get_child_index(v);
  if (index == invalid_index) {
    return;
  }

  v->_parent = nullptr;
  mts::managed_ptr<view> child = std::move(_children[index]);
  mts::rect child_bounds = child->get_bounds();

  _children.erase(_children.begin() + index);

  if (notify_parent_changed) {
    child->parent_changed();
  }

  if (notify_style_changed && !child->_attributes._style && child->_attributes._cached_style == get_style()) {
    child->send_style_changed();
  }

  child_removed(child.get());

  children_changed();

  repaint(child_bounds);
}

void view::to_front() {
  if (view* p = get_parent()) {
    children_vector& children = p->_children;

    std::size_t index = p->get_child_index(this);
    if (index != view::invalid_index) {
      mts::managed_ptr<view> __self = std::move(children[index]);
      children.erase(children.begin() + index);
      children.push_back(std::move(__self));
    }
  }
}

void view::set_position(const mts::point<int>& pos) {
  mts::rect<int> old_bounds = get_drawing_bounds();

  if (mts::assign(_bounds.position, pos)) {
    const bounds_status bs{ bounds_status::changed_type::position };
    bounds_changed(bs);
    listeners.notify(&listener::view_bounds_changed, *this, bs);

    repaint();
    if (view* p = get_parent()) {
      p->repaint(old_bounds.merged(get_drawing_bounds()));
    }
  }
}

void view::set_size(const mts::size<int>& size) {
  mts::rect<int> old_bounds = _bounds;
  if (mts::assign(_bounds.size, size)) {
    const bounds_status bs{ bounds_status::changed_type::size };
    bounds_changed(bs);
    listeners.notify(&listener::view_bounds_changed, *this, bs);

    repaint();
    if (view* p = get_parent()) {
      p->repaint(old_bounds.merged(_bounds));
    }

    if (has_native_view()) {
      native_view* nv = get_native_view();
      nv->set_bounds({ nv->get_bounds().position, size });
    }
  }
}

void view::set_bounds(const mts::rect<int>& bounds) {
  mts::rect<int> old_bounds = _bounds;

  bool p_changed = mts::assign(_bounds.position, bounds.position);
  bool s_changed = mts::assign(_bounds.size, bounds.size);

  if (!(p_changed || s_changed)) {
    return;
  }

  using ctype = bounds_status::changed_type;
  ctype c = (p_changed && s_changed) ? ctype::bounds : (p_changed ? ctype::position : ctype::size);
  const bounds_status bs{ c };
  bounds_changed(bs);

  listeners.notify(&listener::view_bounds_changed, *this, bs);

  if (view* p = get_parent()) {
    p->repaint(old_bounds.merged(_bounds));
  }

  repaint();
}

const mts::point<int>& view::get_position() const noexcept { return _bounds.position; }

const mts::size<int>& view::get_size() const noexcept { return _bounds.size; }

const mts::rect<int>& view::get_bounds() const noexcept { return _bounds; }

mts::point<int> view::get_screen_position() const noexcept {
  if (const native_view* nv = get_native_view(); is_root()) {
    return get_position() + nv->get_screen_position();
  }

  mts::point abs_pos = get_absolute_position();

  if (const view* p = get_root_parent()) {
    if (const native_view* nv = p->get_native_view()) {
      abs_pos += nv->get_screen_position();
    }
  }

  return abs_pos;
}

mts::rect<int> view::get_screen_bounds() const noexcept { return mts::rect<int>(get_screen_position(), get_size()); }

void view::set_opacity(float opacity) {
  if (mts::assign(_opacity, std::clamp(opacity, 0.0f, 1.0f))) {
    repaint();
  }
}
void view::set_visible(bool visible) {
  if (is_visible() == visible) {
    return;
  }

  if (native_view* nv = get_native_view()) {
    nv->set_visible(visible);
  }

  mts::set_flag(view::flags::f_is_visible, visible, _flags);
  repaint();
  if (view* p = get_parent()) {
    p->repaint(get_bounds());
  }

  visibily_changed();

  listeners.notify(&listener::view_visibility_changed, *this);
}

void view::grab_focus() {
  if (native_view* nv = get_native_view(); is_root()) {
    nv->set_focused_view(mts::weak_managed_ptr<mts::view>(this));
    return;
  }

  if (view* p = get_root_parent()) {
    if (native_view* nv = p->get_native_view()) {
      nv->set_focused_view(mts::weak_managed_ptr<mts::view>(this));
    }
  }
}

void view::ungrab_focus() {
  if (native_view* nv = get_native_view(); is_root()) {
    if (nv->get_focused_view() == this) {
      nv->set_focused_view(nullptr);
    }

    return;
  }

  if (view* p = get_root_parent()) {
    if (native_view* nv = p->get_native_view()) {
      if (nv->get_focused_view() == this) {
        nv->set_focused_view(nullptr);
      }
    }
  }
}

bool view::is_parent_of(const view* child) const noexcept {
  while (child) {
    child = child->get_parent();

    if (child == this) {
      return true;
    }
  }

  return false;
}

const style& view::attributes::default_style() const { return *get_style(); }

std::shared_ptr<const mts::style> view::attributes::get_style() const {
  static std::shared_ptr<style> __style = std::make_shared<style>();

  if (_style) {
    _cached_style = _style;
    return _style;
  }

  if (_cached_style) {
    return _cached_style;
  }

  for (auto* c = &_view; c != nullptr; c = c->get_parent()) {
    if (auto lf = c->_attributes._style) {
      _cached_style = lf;
      return _cached_style;
    }
  }

  if (std::shared_ptr<const mts::style> s = get_assets()->styles.get("default")) {
    _cached_style = s;
  }
  else {
    _cached_style = __style;
  }

  return _cached_style;
}

void view::attributes::set_style(std::shared_ptr<const mts::style> s) {
  if (mts::assign(_style, s)) {
    _view.send_style_changed();
  }
}

void view::attributes::set_style(const std::string& style_name) {
  if (auto s = get_assets()->styles.get(style_name)) {
    set_style(s);
  }
}

void view::attributes::set_style(std::nullptr_t) {
  if (_style) {
    _style.reset();
    _view.send_style_changed();
  }
}

void view::send_style_changed() {
  _attributes._cached_style.reset();
  style_changed();
  repaint();

  for (auto& c : _children) {
    if (!c->_attributes._style) {
      c->send_style_changed();
    }
  }
}

void view::attributes::notify(std::string_view group, std::string_view name) { _view.attribute_changed(group, name); }

void view::repaint() {
  if (has_native_view()) {
    if (native_view* nv = get_native_view(); nv && native_view::is_valid_native_view(nv)) {
      nv->repaint(get_absolute_drawing_bounds());
    }
    return;
  }

  mts::weak_managed_ptr<view> root = get_root_parent();
  if (!root) {
    return;
  }

  if (root->has_native_view()) {
    if (native_view* nv = root->get_native_view(); nv && native_view::is_valid_native_view(nv)) {
      nv->repaint(get_absolute_drawing_bounds());
    }
  }
}

void view::mouse_wheel(const mouse_event& evt, const scroll_event& sevt) {
  if (view* p = get_parent()) {
    mouse_event ss(evt.get_source_type(), evt.get_absolute_position(), evt.get_position() + get_position(),
        evt.get_modifiers(), evt.get_click_count());
    p->mouse_wheel(ss, sevt);
  }
}

void view::repaint(const mts::rect<float>& area) {
  mts::rect<float> dirty_rect = area;
  dirty_rect += get_absolute_position();

  if (has_native_view()) {
    if (native_view* nv = get_native_view()) {
      nv->repaint(dirty_rect);
    }
    return;
  }

  if (view* v = get_root_parent()) {
    if (v->has_native_view()) {
      if (native_view* nv = v->get_native_view()) {
        nv->repaint(dirty_rect);
      }
      return;
    }
  }
}

bool view::has_property(const std::string& name) const {
  if (std::shared_ptr<const mts::property_component> p = get_component<mts::property_component>()) {
    return p->contains(name);
  }
  return false;
}

bool view::remove_property(const std::string& name) {
  std::shared_ptr<mts::property_component> p = get_component<mts::property_component>();

  if (p && p->erase(name)) {
    property_changed(name);
    return true;
  }

  return false;
}

bool view::set_property(const std::string& name, const property& p) {
  if (name.empty()) {
    return false;
  }

  std::size_t dot_pos = name.find('.');

  if (dot_pos == std::string::npos) {
    force_get_component<mts::property_component>()->set(name, p);
    property_changed(name);
    return true;
  }

  if (!is_valid_child_string(name)) {
    return false;
  }

  std::string_view vname = name;

  std::size_t start = 0;
  std::size_t end = dot_pos;

  view* child = this;
  while (end != std::string::npos) {
    std::size_t s_index = child->get_child_index(vname.substr(start, end - start));

    if (s_index == invalid_index) {
      return false;
    }

    child = child->get_children()[s_index].get();

    start = end + 1;
    end = vname.find('.', start);
  }

  return child->set_property(name.substr(start, end), p);
}

const property& view::get_property(const std::string& name) const {
  static property __p;

  std::string_view vname = name;

  if (vname.empty()) {
    return __p;
  }

  std::size_t dot_pos = vname.find('.');

  if (dot_pos == std::string::npos) {
    if (std::shared_ptr<const mts::property_component> p = get_component<mts::property_component>()) {
      return p->get(name);
    }
    return __p;
  }

  if (!is_valid_child_string(vname)) {
    return __p;
  }

  std::size_t start = 0;
  std::size_t end = dot_pos;

  const view* child = this;
  while (end != std::string::npos) {
    std::size_t s_index = child->get_child_index(vname.substr(start, end - start));

    if (s_index == invalid_index) {
      return __p;
    }

    child = child->get_children()[s_index].get();

    start = end + 1;
    end = vname.find('.', start);
  }

  if (std::shared_ptr<const mts::property_component> p = child->get_component<mts::property_component>()) {
    return p->get(std::string(vname.substr(start, end)));
  }

  return __p;
}

void view::add_to_desktop() {
  native_view::add_to_desktop(this, window::type::popup, window::style_flags::none, nullptr);
}
MTS_END_NAMESPACE
