#include "mts/ui/native_view.h"
#include "mts/ui/event.h"
#include "mts/ui/desktop.h"

MTS_BEGIN_NAMESPACE

bool native_view::is_valid_native_view(_VMTS::not_null<native_view*> nv) { return desktop::get_instance().has(nv); }

_VMTS::weak_managed_ptr<view> native_view::_selected_view;
_VMTS::weak_managed_ptr<view> native_view::_focused_view;
_VMTS::weak_managed_ptr<view> native_view::_view_under_mouse;

native_view::native_view(view& __view)
    : _view(__view)
    , _view_ptr(&__view) {

  desktop::get_instance().add(this);
}

native_view::~native_view() { desktop::get_instance().remove(this); }

_VMTS::size<int> native_view::get_size_constrain(const _VMTS::size<int>& frame_size) const {
  const window* w = get_window();
  return w ? w->get_size_constrain(frame_size) : frame_size;
}

void native_view::handle_bounds_changed() {
  bool is_minimised = false;

  if (_view.has_native_view() && !is_minimised) {
    _view.set_bounds(get_bounds().with_position({ 0, 0 }));

    if (window* w = get_window()) {
      w->frame_changed();
    }

    //        const WeakReference<Component> deletionChecker (&component);
    //
    //        auto newBounds = Component::ComponentHelpers::rawPeerPositionToLocal (component, getBounds());
    //        auto oldBounds = component.getBounds();
    //
    //        const bool wasMoved   = (oldBounds.getPosition() != newBounds.getPosition());
    //        const bool wasResized = (oldBounds.getWidth() != newBounds.getWidth() || oldBounds.getHeight() !=
    //        newBounds.getHeight());
    //
    //        if (wasMoved || wasResized)
    //        {
    //            component.boundsRelativeToParent = newBounds;
    //
    //            if (wasResized)
    //                component.repaint();
    //
    //            component.sendMovedResizedMessages (wasMoved, wasResized);
    //
    //            if (deletionChecker == nullptr)
    //                return;
    //        }
  }
  //
  //    if (isWindowMinimised != nowMinimised)
  //    {
  //        isWindowMinimised = nowMinimised;
  //        component.minimisationStateChanged (nowMinimised);
  //        component.sendVisibilityChangeMessage();
  //    }
  //
  //    if (! isFullScreen())
  //        lastNonFullscreenBounds = component.getBounds();
}

void native_view::set_view_under_mouse(_VMTS::weak_managed_ptr<view> v, const mouse_event& evt) {
  if (!v.is_valid()) {
    if (_view_under_mouse) {
      _view_under_mouse->mouse_leave();
      _view_under_mouse = nullptr;
    }
    return;
  }

  if (_view_under_mouse == v) {
    return;
  }

  if (_view_under_mouse) {
    _view_under_mouse->mouse_leave();
    _view_under_mouse = nullptr;
  }

  _view_under_mouse = v;
  _view_under_mouse->mouse_enter(evt);
}

void native_view::set_selected_view(_VMTS::weak_managed_ptr<view> v, const mouse_event& evt) {
  _VMTS::unused(evt);
  _selected_view = v;
}

void native_view::set_focused_view(_VMTS::weak_managed_ptr<view> v) {
  if (_focused_view) {
    if (_focused_view != v) {
      _focused_view->focus_lost();
    }
  }

  _focused_view = v;
  if (_focused_view) {
    _focused_view->focus_gained();
  }
}

void native_view::handle_mouse_move(const mouse_event::common& cevt) {
  found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);

  if (c_info) {
    set_view_under_mouse(c_info.view, mouse_event(cevt, c_info.relative_position));

    if (_view_under_mouse) {
      if (_view_under_mouse->wants_mouse_move()) {
        _view_under_mouse->mouse_moved(mouse_event(cevt, c_info.relative_position));
      }
    }

    return;
  }

  if (window* w = get_window(); w && w->get_wants_unbounded_mouse_events()) {
    w->mouse_moved(mouse_event(cevt, cevt.absolute_position - w->get_absolute_position()));
  }

  set_view_under_mouse(nullptr, mouse_event(cevt, c_info.relative_position));
}

bool native_view::handle_mouse_left_down(const mouse_event::common& cevt) {
  found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);

  if (c_info) {

    set_view_under_mouse(c_info.view, mouse_event(cevt, c_info.relative_position));

    if (_view_under_mouse) {
      if (_view_under_mouse->get_no_mouse_drag()) {
        set_selected_view(nullptr, mouse_event(cevt, c_info.relative_position));
      }
      else {
        set_selected_view(_view_under_mouse, mouse_event(cevt, c_info.relative_position));
      }

      bool wants_key_focus = _view_under_mouse->wants_keyboard_focus();
      _view_under_mouse->mouse_left_down(mouse_event(cevt, c_info.relative_position));

      if (!_view_under_mouse && wants_key_focus) {
        set_focused_view(nullptr);
        return true;
      }

      if (_view_under_mouse && wants_key_focus) {
        set_focused_view(_view_under_mouse);
      }
    }

    return true;
  }

  if (window* w = get_window(); w && w->get_wants_unbounded_mouse_events()) {
    w->mouse_left_down(mouse_event(cevt, cevt.absolute_position - w->get_absolute_position()));
  }

  set_selected_view(nullptr, mouse_event(cevt, c_info.relative_position));
  return false;
}

// bool native_view::handle_mouse_left_up(const mouse_event::common& cevt) {
//   found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);
//
//   if(_selected_view) {
//     _selected_view->mouse_left_up(
//           mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
//
//     _selected_view = nullptr;
//   }
//
//   if(c_info) {
//     set_view_under_mouse(c_info.view, mouse_event(cevt, c_info.relative_position));
//     return true;
//   }
//
//   set_view_under_mouse(nullptr, mouse_event(cevt, c_info.relative_position));
//
////  if (_selected_view) {
////
////    bool same = c_info.view == _selected_view;
////
////    auto watcher = _selected_view->get_watcher();
////
////    _selected_view->mouse_left_up(
////        mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
////
////    if (watcher.expired()) {
////      _selected_view = nullptr;
////
////      if (same) {
////        return true;
////      }
////
////      set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
////
////      return true;
////    }
////
////    if (view* root = _selected_view->get_root_parent()) {
////      if (root->get_native_view() == this && !has_focus()) {
////        mts::print("handle_mouse_left_up::same_native_view");
////        _selected_view->mouse_leave();
////      }
////    }
////
////    if (_selected_view != c_info.view) {
////      set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
////    }
////
////    return true;
////  }
////
////  if (c_info) {
////    set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
////    return true;
////  }
////
////  set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//  return false;
//}

// get_no_mouse_drag

bool native_view::handle_mouse_left_up(const mouse_event::common& cevt) {
  found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);

  bool has_selected_view = _selected_view.is_valid();

  if (has_selected_view) {
    _selected_view->mouse_left_up(mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));

    _selected_view = nullptr;
    mts::print("_selected_view MOUSE UP");
  }

  if (c_info) {
    set_view_under_mouse(c_info.view, mouse_event(cevt, c_info.relative_position));

    if (!has_selected_view && _view_under_mouse && _view_under_mouse->get_no_mouse_drag()) {
      _view_under_mouse->mouse_left_up(
          mouse_event(cevt, cevt.absolute_position - _view_under_mouse->get_absolute_position()));
    }

    //    mts::print("_view_under_mouse MOUSE UP", _view_under_mouse->get_root_parent()->get_native_view());
    return true;
  }

  if (window* w = get_window(); w && w->get_wants_unbounded_mouse_events()) {
    mts::print("WINDOW MOUSE UP");
    w->mouse_left_up(mouse_event(cevt, cevt.absolute_position - w->get_absolute_position()));
  }

  mts::print("ddd MOUSE UP");
  set_view_under_mouse(nullptr, mouse_event(cevt, c_info.relative_position));
  return false;
}

bool native_view::handle_mouse_left_drag(const mouse_event::common& cevt) {
  //  mts::print("native_view::handle_mouse_left_drag");
  if (_selected_view) {
    _selected_view->mouse_left_drag(
        mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
    return true;
  }

  found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);
  if (c_info && c_info.view->get_no_mouse_drag()) {
    set_view_under_mouse(c_info.view, mouse_event(cevt, c_info.relative_position));

    if (_view_under_mouse) {
      _view_under_mouse->mouse_moved(
          mouse_event(cevt, cevt.absolute_position - _view_under_mouse->get_absolute_position()));
      return true;
    }
  }

  if (window* w = get_window(); w && w->get_wants_unbounded_mouse_events()) {
    w->mouse_moved(mouse_event(cevt, cevt.absolute_position - w->get_absolute_position()));
  }

  return false;
}

// bool native_view::handle_mouse_left_drag(const mouse_event::common& cevt) {
//   if (!_selected_view) {
//     return false;
//   }
//
//   _selected_view->mouse_left_drag(
//       mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
//   return true;
// }

bool native_view::handle_mouse_wheel(const mouse_event::common& cevt, const scroll_event& sevt) {
  if (_selected_view) {
    _selected_view->mouse_wheel(
        mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()), sevt);
    return true;
  }

  if (found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position)) {
    c_info.view->mouse_wheel(mouse_event(cevt, c_info.relative_position), sevt);
    return true;
  }

  return false;
}

// void native_view::handle_mouse_move(const mouse_event::common& cevt) {
//
//   found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);
//
//
//
//
//   if (c_info) {
//     set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//
//     if (_selected_view) {
//       if (_selected_view->wants_mouse_move()) {
//         _selected_view->mouse_moved(mouse_event(cevt, c_info.relative_position));
//       }
//     }
//   }
//   else {
//     set_selected_view(nullptr, mouse_event(cevt, c_info.relative_position));
//   }
// }

bool native_view::handle_mouse_right_down(const mouse_event::common& cevt) {
  if (found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position)) {
    c_info.view->mouse_right_down(mouse_event(cevt, c_info.relative_position));
    return true;
  }

  return false;
}

// bool native_view::handle_mouse_left_up(const mouse_event::common& cevt) {
//   found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);
//
//   if (_selected_view) {
//
//     bool same = c_info.view == _selected_view;
//
//     auto watcher = _selected_view->get_watcher();
//
//     _selected_view->mouse_left_up(
//         mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
//
//     if (watcher.expired()) {
//       _selected_view = nullptr;
//
//       if (same) {
//         return true;
//       }
//
//       set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//
//       return true;
//     }
//
//     if (view* root = _selected_view->get_root_parent()) {
//       if (root->get_native_view() == this && !has_focus()) {
//         mts::print("handle_mouse_left_up::same_native_view");
//         _selected_view->mouse_leave();
//       }
//     }
//
//     if (_selected_view != c_info.view) {
//       set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//     }
//
//     return true;
//   }
//
//   if (c_info) {
//     set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//     return true;
//   }
//
//   set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
//   return false;
// }

//
bool native_view::handle_mouse_right_up(const mouse_event::common& cevt) {
  found_view_info c_info = find_view_under_position(&_view, cevt.absolute_position);

  if (_selected_view) {
    _selected_view->mouse_right_up(mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));

    if (_selected_view != c_info.view) {
      set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
    }

    return true;
  }

  if (c_info) {
    set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
    return true;
  }

  set_selected_view(c_info.view, mouse_event(cevt, c_info.relative_position));
  return false;
}

// bool native_view::handle_mouse_left_drag(const mouse_event::common& cevt) {
//   if (!_selected_view) {
//     return false;
//   }
//
//   _selected_view->mouse_left_drag(
//       mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
//   return true;
// }

bool native_view::handle_mouse_right_drag(const mouse_event::common& cevt) {
  if (!_selected_view) {
    return false;
  }

  _selected_view->mouse_right_drag(mouse_event(cevt, cevt.absolute_position - _selected_view->get_absolute_position()));
  return true;
}

void native_view::handle_focus_gained() {
  get_view().listeners.notify(&view::listener::view_focus_changed, get_view(), true);
}

void native_view::handle_focus_lost() {
  //
  //  bool same_native_view = true;
  //  if(_selected_view) {
  //    if(view* root = _selected_view->get_root_parent()) {
  //      same_native_view = (root->get_native_view() == this);
  //    }
  //    mts::print("handle_focus_lost::same_native_view", same_native_view);
  //  }
  //
  //  mouse_event::common cmd{};
  //  set_selected_view(nullptr, mouse_event(mouse_event::common{}, {0, 0}));

  get_view().listeners.notify(&view::listener::view_focus_changed, get_view(), false);

  //  set_view_under_mouse(nullptr, mouse_event(mouse_event::common{}, {0, 0}));
  set_selected_view(nullptr, mouse_event(mouse_event::common{}, { 0, 0 }));
  set_focused_view(nullptr);
  //  set_view_under_mouse(nullptr, mouse_event(mouse_event::common{}, {0, 0}))

  //    if(_selected_view) {
  //      _selected_view->mouse_leave();
  //    }
}

bool native_view::handle_key_down(const key_event& kevt) {
  if (_focused_view) {
    return _focused_view->key_pressed(kevt);
  }

  if (!_view_under_mouse) {
    if (window* w = get_window(); w && w->wants_unfocused_key_events()) {
      return w->key_pressed(kevt);
    }
    return false;
  }

  if (_view_under_mouse->key_pressed(kevt)) {
    return true;
  }

  if (!_view_under_mouse) {
    return true;
  }

  _VMTS::weak_managed_ptr<view> target = _view_under_mouse->get_parent();

  while (target) {
    if (target->wants_unfocused_key_events() && target->key_pressed(kevt)) {
      return true;
    }

    if (!target) {
      return true;
    }

    target = target->get_parent();
  }

  if (window* w = get_window(); w && w->wants_unfocused_key_events()) {
    return w->key_pressed(kevt);
  }

  return false;
}

// bool native_view::handle_key_down(const key_event& kevt) {
//   if (_focused_view) {
//     return _focused_view->key_pressed(kevt);
//   }
//
//   if (_view_under_mouse) {
//     if(_view_under_mouse->key_pressed(kevt)) {
//       return true;
//     }
//
//     if (!_view_under_mouse) {
//       return true;
//     }
//
//     view* vum = _view_under_mouse.get();
//
////    weak_managed_ptr<view> target = vum->get_parent() ? vum->get_parent() : nullptr;
//
////    _VMTS::weak_managed_ptr<view> target = _view_under_mouse;
//
//
//
//    while (target) {
//      if(target->wants_unfocused_key_events()) {
//        view* vv = target.get();
//        mts::print("DS:LKFKJLKFD", vv);
//      }
//
//      if (target->wants_unfocused_key_events() && target->key_pressed(kevt)) {
//
//        return true;
//      }
//
//      if (!target) {
//        return true;
//      }
//
//      if (view* parent = target->get_parent()) {
//        target = parent;
//      }
//      else {
//        break;
//      }
//    }
//  }
//
//  if (_VMTS::window* w = get_window(); w && w->wants_unfocused_key_events()) {
//    return w->key_pressed(kevt);
//  }
//
//  return false;
//}

// bool native_view::handle_key_down(const _VMTS::key_event& kevt) {
//   if (_focused_view) {
//     return _focused_view->key_pressed(kevt);
//   }
//
//   if (_view_under_mouse) {
//     if(_view_under_mouse->key_pressed(kevt)) {
//       return true;
//     }
//
//     if (!_view_under_mouse) {
//       return true;
//     }
//
//
//     _VMTS::weak_managed_ptr<view> target = _view_under_mouse;
//
//
//
//     while (target) {
//       if(target->wants_unfocused_key_events()) {
//         view* vv = target.get();
//         mts::print("DS:LKFKJLKFD", vv);
//       }
//
//       if (target->wants_unfocused_key_events() && target->key_pressed(kevt)) {
//
//         return true;
//       }
//
//       if (!target) {
//         return true;
//       }
//
//       if (view* parent = target->get_parent()) {
//         target = parent;
//       }
//       else {
//         break;
//       }
//     }
//   }
//
//   if (_VMTS::window* w = get_window(); w && w->wants_unfocused_key_events()) {
//     return w->key_pressed(kevt);
//   }
//
//   return false;
// }

bool native_view::handle_key_up(const key_event& kevt) {
  _VMTS::unused(kevt);
  return false;
}
//
//
//
//
//
//

enum class draw_heuristic { all, combined, separated };

inline draw_heuristic get_draw_heuristic(const view* v, const mts::memory_range<const _VMTS::rect<float>>& rects,
    _VMTS::rect<float>& combined_rect, float area_threshold = 0.5f, float combined_area_factor = 0.9f) {

  const _VMTS::rect<float> v_bounds = (_VMTS::rect<float>)v->get_bounds();

  if (rects.size() == 1) {
    if (rects[0] == v_bounds) {
      combined_rect = rects[0];
      return draw_heuristic::all;
    }

    combined_rect = rects[0];
    return draw_heuristic::combined;
  }

  combined_rect = rects[0];
  for (std::size_t i = 1; i < rects.size(); i++) {
    combined_rect = combined_rect.get_union(rects[i]);
  }

  // Sum area of all dirty rects.
  float area = 0;
  for (const auto& r : rects) {
    area += r.get_area();
  }

  const float b_area = v_bounds.get_area();
  const float combined_area_ratio = combined_rect.get_area() / b_area;
  const float separated_area_ratio = area / b_area;

  // If combined and separated are essentially the same, then we go with combined.
  if (combined_area_ratio * combined_area_factor <= separated_area_ratio) {
    return draw_heuristic::combined;
  }

  // If separated area ratio is small enough (smaller than area_threshold) we go for separated.
  return separated_area_ratio < area_threshold ? draw_heuristic::separated : draw_heuristic::combined;
}

void native_view::handle_paint(mts::context& g, const mts::memory_range<const _VMTS::rect<float>>& dirty_rects) {
  mts_assert(dirty_rects.size(), "dirty_rects should never be empty.");

  _VMTS::rect<float> combined_rect;
  draw_heuristic dh = get_draw_heuristic(&_view, dirty_rects, combined_rect);

  switch (dh) {
  case draw_heuristic::all:
    //    handle_paint_all(g, &_view, { 0, 0 });
    handle_paint(g, &_view, combined_rect, { 0, 0 });
    break;

  case draw_heuristic::combined:
    g.save_state();
    g.clip_to_rect(combined_rect);
    handle_paint(g, &_view, combined_rect, { 0, 0 });
    g.restore_state();
    break;

  case draw_heuristic::separated:
    for (const _VMTS::rect<float>& r : dirty_rects) {
      g.save_state();
      g.clip_to_rect(r);
      handle_paint(g, &_view, r, { 0, 0 });
      g.restore_state();
    }
    break;
  }
}

inline bool native_view::has_child_in_rect(const view* v, const _VMTS::rect<int>& bounds) {
  _VMTS::rect<float> fbounds = bounds;

  for (const auto& c : *v) {
    _VMTS::rect<float> c_bounds = c->get_drawing_bounds();
    auto clipping = v->get_component<mts::clipping_component>();
    bool has_clipping_path = clipping && clipping->has_clipping_path();

    if (!has_clipping_path && c->is_visible() && c->is_opaque()
        && c_bounds.get_union(c_bounds.get_intersection(fbounds)) == fbounds) {
      return true;
    }
  }
  return false;
}

// inline bool has_child_in_rect(const view* v, const _VMTS::rect<int>& bounds) {
//   _VMTS::rect<float> fbounds = bounds;
//
//   for (const auto& c : *v) {
//     _VMTS::rect<float> c_bounds = c->get_drawing_bounds();
//
//     if (c->is_visible() && c->is_opaque() && c_bounds.get_union(c_bounds.get_intersection(fbounds)) == fbounds) {
//       return true;
//     }
//   }
//   return false;
// }

// void native_view::handle_paint(
//     _VMTS::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta) {
//
//   const _VMTS::rect<int>& bounds = v->get_bounds();
//   _VMTS::rect<float> drawing_bounds = v->get_drawing_bounds();
//
//
//   if (!(drawing_bounds + delta).intersect(rect)) {
//     return;
//   }
//
//   if (!v->is_visible()) {
//     return;
//   }
//
//   g.save_state();
//
//   g.translate(bounds.position);
//
//   float opacity = v->get_opacity();
//   if (opacity != 1) {
//     g.begin_transparent_layer(opacity);
//   }
//
//   _VMTS::rect<float> c_rect = g.get_clipping_rect();
//   _VMTS::rect local_bounds = bounds.with_position({ 0, 0 }).get_intersection(c_rect);
//
//   if (v->is_clipped()) {
//     g.clip_to_rect(local_bounds);
//   }
//
//   //  _VMTS::rect<float> intersection = bounds.get_intersection(rect - delta) - bounds.position;
//   _VMTS::rect<float> intersection = (bounds.get_intersection(rect - delta) -
//   bounds.position).get_intersection(local_bounds); if (!v->children_count() || !has_child_in_rect(v, intersection)) {
//     v->paint(g, intersection);
//   }
//
//   for (auto& c : v->_children) {
//     handle_paint(g, c.get(), rect, delta + bounds.position);
//   }
//
//   if (opacity != 1) {
//     g.end_transparent_layer();
//   }
//   g.restore_state();
// }

void native_view::handle_paint(
    mts::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta) {

  const _VMTS::rect<float> bounds = v->get_fbounds();
  const _VMTS::rect<float> drawing_bounds = v->get_drawing_bounds();

  if (!(drawing_bounds + delta).intersect(rect)) {
    return;
  }

  if (!v->is_visible()) {
    return;
  }

  g.save_state();

  g.translate(bounds.position);

  float opacity = v->get_opacity();
  if (opacity != 1) {
    g.begin_transparent_layer(opacity);
  }

  _VMTS::rect<float> c_rect = g.get_clipping_rect();
  _VMTS::rect<float> local_bounds = bounds.with_position({ 0, 0 }).get_intersection(c_rect);

  _VMTS::rect<float> local_drawing_bounds
      = drawing_bounds.with_position({ -v->get_drawing_margin().left, -v->get_drawing_margin().top })
            .get_intersection(c_rect);

  if (v->is_clipped()) {
    g.clip_to_rect(local_drawing_bounds);
  }

  //  _VMTS::rect<float> intersection = bounds.get_intersection(rect - delta) - bounds.position;
  _VMTS::rect<float> intersection
      = (bounds.get_intersection(rect - delta) - bounds.position).get_intersection(local_bounds);
  _VMTS::rect<float> drawing_intersection = (drawing_bounds.get_intersection(rect - delta) - drawing_bounds.position)
                                                .get_intersection(local_drawing_bounds);

  if (auto clipping = v->get_component<mts::clipping_component>(); clipping && clipping->has_clipping_path()) {
    g.clip_to_path(clipping->get_clipping_path());
  }
  //  if (const _VMTS::path* cp = v->get_clipping_path()) {
  //    g.clip_to_path(*cp);
  //  }

  if (!v->children_count() || !has_child_in_rect(v, drawing_intersection)) {
    g.save_state();
    v->paint(g, intersection);
    g.restore_state();
  }

  for (auto& c : v->_children) {
    handle_paint(g, c.get(), rect, delta + bounds.position);
  }

  //  v->paint_over(g, intersection);

  if (opacity != 1) {
    g.end_transparent_layer();
  }
  g.restore_state();

  g.save_state();
  g.translate(bounds.position);
  v->paint_over(g, intersection);
  g.restore_state();
}

// void native_view::handle_paint(
//     _VMTS::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta) {
//
//   const _VMTS::rect<float> bounds = v->get_fbounds();
//   const _VMTS::rect<float> drawing_bounds = v->get_drawing_bounds();
//
//   if (!(drawing_bounds + delta).intersect(rect)) {
//     return;
//   }
//
//   if (!v->is_visible()) {
//     return;
//   }
//
//   g.save_state();
//
//   g.translate(bounds.position);
//
//   float opacity = v->get_opacity();
//   if (opacity != 1) {
//     g.begin_transparent_layer(opacity);
//   }
//
//   _VMTS::rect<float> c_rect = g.get_clipping_rect();
//   _VMTS::rect<float> local_bounds = bounds.with_position({ 0, 0 }).get_intersection(c_rect);
//
//   _VMTS::rect<float> local_drawing_bounds
//       = drawing_bounds.with_position({ -v->get_margin().left, -v->get_margin().top }).get_intersection(c_rect);
//
//   if (v->is_clipped()) {
//     g.clip_to_rect(local_drawing_bounds);
//   }
//
//   //  _VMTS::rect<float> intersection = bounds.get_intersection(rect - delta) - bounds.position;
//   _VMTS::rect<float> intersection = (bounds.get_intersection(rect - delta) -
//   bounds.position).get_intersection(local_bounds); _VMTS::rect<float> drawing_intersection =
//   (drawing_bounds.get_intersection(rect - delta) - drawing_bounds.position)
//                                            .get_intersection(local_drawing_bounds);
//
//   if (const _VMTS::path* cp = v->get_clipping_path()) {
//     g.clip_to_path(*cp);
//   }
//
//   if (!v->children_count() || !has_child_in_rect(v, drawing_intersection)) {
//     g.save_state();
//     v->paint(g, intersection);
//     g.restore_state();
//   }
//
//   for (auto& c : v->_children) {
//     handle_paint(g, c.get(), rect, delta + bounds.position);
//   }
//
//   if (opacity != 1) {
//     g.end_transparent_layer();
//   }
//   g.restore_state();
// }

// void native_view::handle_paint(
//     _VMTS::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta) {
//
//   const _VMTS::rect<float> bounds = v->get_fbounds();
//   const _VMTS::rect<float> drawing_bounds = v->get_drawing_bounds();
//
//   if (!(drawing_bounds + delta).intersect(rect)) {
//     return;
//   }
//
//   if (!v->is_visible()) {
//     return;
//   }
//
//   g.save_state();
//
//   g.translate(bounds.position);
//
//   float opacity = v->get_opacity();
//   if (opacity != 1) {
//     g.begin_transparent_layer(opacity);
//   }
//
//   _VMTS::rect<float> c_rect = g.get_clipping_rect();
//   _VMTS::rect<float> local_bounds = bounds.with_position({ 0, 0 }).get_intersection(c_rect);
//
//   _VMTS::rect<float> local_drawing_bounds
//       = drawing_bounds.with_position({ -v->get_margin().left, -v->get_margin().top }).get_intersection(c_rect);
//
//   if (v->is_clipped()) {
//     g.clip_to_rect(local_drawing_bounds);
//   }
//
//   //  _VMTS::rect<float> intersection = bounds.get_intersection(rect - delta) - bounds.position;
//   _VMTS::rect<float> intersection = (bounds.get_intersection(rect - delta) -
//   bounds.position).get_intersection(local_bounds); _VMTS::rect<float> drawing_intersection =
//   (drawing_bounds.get_intersection(rect - delta) - drawing_bounds.position)
//                                            .get_intersection(local_drawing_bounds);
//
//   if (!v->children_count() || !has_child_in_rect(v, drawing_intersection)) {
//     g.save_state();
//     v->paint(g, intersection);
//     g.restore_state();
//   }
//
//   for (auto& c : v->_children) {
//     handle_paint(g, c.get(), rect, delta + bounds.position);
//   }
//
//   if (opacity != 1) {
//     g.end_transparent_layer();
//   }
//   g.restore_state();
// }

// void native_view::handle_paint(
//     _VMTS::context& g, view* v, const _VMTS::rect<float>& rect, const _VMTS::point<float>& delta) {
//
//   const _VMTS::rect<int>& bounds = v->get_bounds();
//
//   if (!(bounds + delta).intersect(rect.cast<int>())) {
//     return;
//   }
//
//   if (!v->is_visible()) {
//     return;
//   }
//
//   g.save_state();
//
//   g.translate(bounds.position);
//
//   float opacity = v->get_opacity();
//   if (opacity != 1) {
//     g.begin_transparent_layer(opacity);
//   }
//
//   _VMTS::rect<float> c_rect = g.get_clipping_rect();
//   _VMTS::rect local_bounds = bounds.with_position({ 0, 0 }).get_intersection(c_rect);
//
//   if (v->is_clipped()) {
//     g.clip_to_rect(local_bounds);
//   }
//
//   //  _VMTS::rect<float> intersection = bounds.get_intersection(rect - delta) - bounds.position;
//   _VMTS::rect<float> intersection = (bounds.get_intersection(rect - delta) -
//   bounds.position).get_intersection(local_bounds); if (!v->children_count() || !has_child_in_rect(v, intersection)) {
//     v->paint(g, intersection);
//   }
//
//   for (auto& c : v->_children) {
//     handle_paint(g, c.get(), rect, delta + bounds.position);
//   }
//
//   if (opacity != 1) {
//     g.end_transparent_layer();
//   }
//   g.restore_state();
// }

bool native_view::has_main_thread() { return event_manager_proxy::has_main_thread(); }

void native_view::handle_set_main_thread() { event_manager_proxy::set_main_thread(); }

native_view::found_view_info native_view::find_view_under_position(view* v, const _VMTS::point<float>& p) {
  if (!v->get_bounds().contains(p) || !v->is_visible()) {
    return { nullptr, _VMTS::point<float>(0, 0) };
  }

  _VMTS::point<float> c_pos = p - v->get_position();

  // Inverse children loop.
  for (auto it = v->get_children().rbegin(); it != v->get_children().rend(); ++it) {
    if (found_view_info c_info = find_view_under_position(it->get(), c_pos)) {
      return c_info;
    }
  }

  return { _VMTS::weak_managed_ptr<view>(v), c_pos };
}

// native_view::found_view_info native_view::find_view_under_position(view* v, const _VMTS::point<float>& p) {
//   if (!v->get_bounds().contains(p)) {
//     return { nullptr, _VMTS::point<float>(0, 0) };
//   }
//
//   _VMTS::point<float> c_pos = p - v->get_position();
//
//   // Look for child.
//   //  for (const auto& c : v->get_children()) {
//   //    if (found_view_info c_info = find_view_under_position(c.get(), c_pos)) {
//   //      return c_info;
//   //    }
//   //  }
//
//   // Inverse children loop.
//   for (auto it = v->get_children().rbegin(); it != v->get_children().rend(); ++it) {
//     if (found_view_info c_info = find_view_under_position(it->get(), c_pos)) {
//       return c_info;
//     }
//   }
//
//   return { v, c_pos };
// }

MTS_END_NAMESPACE
