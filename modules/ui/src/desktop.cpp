#include "mts/ui/desktop.h"
#include <algorithm>

MTS_BEGIN_NAMESPACE

void modal_manager::add(const _VMTS::managed_ptr<view>& v) {
  //
  const view* vv = v.get();
  if (std::find_if(
          _modal_views.begin(), _modal_views.end(), [vv](const _VMTS::managed_ptr<view>& a) { return a.get() == vv; })
      != _modal_views.end()) {
    return;
  }
  _modal_views.push_back(v);
}

void modal_manager::add(_VMTS::managed_ptr<view>&& v) {
  //
  view* vv = v.get();
  if (std::find_if(
          _modal_views.begin(), _modal_views.end(), [vv](const _VMTS::managed_ptr<view>& a) { return a.get() == vv; })
      != _modal_views.end()) {
    return;
  }
  _modal_views.push_back(std::move(v));
}

void modal_manager::remove(_VMTS::not_null<view*> v) {
  auto it = std::find_if(_modal_views.begin(), _modal_views.end(), [v](const auto& a) { return a.get() == v.get(); });
  if (it == _modal_views.end()) {
    return;
  }

  _modal_views.erase(it);
}

bool modal_manager::has(_VMTS::not_null<view*> v) {
  return std::find_if(_modal_views.begin(), _modal_views.end(), [v](const auto& a) { return a.get() == v.get(); })
      != _modal_views.end();
}

desktop& desktop::get_instance() {
  static desktop _desktop;
  return _desktop;
}

const std::vector<display>& desktop::get_displays() { return get_instance()._displays; }
const display* desktop::get_main_display() {
  for (const display& dis : get_displays()) {
    if (dis.is_main) {
      return &dis;
    }
  }

  return nullptr;
}

modal_manager& desktop::get_modal_manager() { return get_instance()._modal_manager; }

desktop::desktop() { native_desktop::init(this); }

desktop::~desktop() {}

void desktop::add(_VMTS::not_null<view*> v) {
  if (std::find(_views.begin(), _views.end(), v.get()) != _views.end()) {
    return;
  }
  _views.push_back(v);
}

void desktop::remove(_VMTS::not_null<view*> v) {
  auto it = std::find(_views.begin(), _views.end(), v.get());
  if (it == _views.end()) {
    return;
  }

  _views.erase(it);
}

void desktop::add(_VMTS::not_null<native_view*> nv) {
  if (std::find(_native_views.begin(), _native_views.end(), nv.get()) != _native_views.end()) {
    return;
  }
  _native_views.push_back(nv);
}

void desktop::remove(_VMTS::not_null<native_view*> nv) {
  auto it = std::find(_native_views.begin(), _native_views.end(), nv.get());
  if (it == _native_views.end()) {
    return;
  }

  _native_views.erase(it);
}

bool desktop::has(_VMTS::not_null<native_view*> nv) {
  return std::find(_native_views.begin(), _native_views.end(), nv.get()) != _native_views.end();
}

MTS_END_NAMESPACE
