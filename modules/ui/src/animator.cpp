//#include "dragon-fruit/gui/animator.h"
//
// MTS_BEGIN_NAMESPACE
// fade_animator::fade_animator(_VDNFT::view& __view)
//    : _timer(*this)
//    , _view(__view) {}
//
// void fade_animator::fade_in() {
//
//  if (_view.is_visible()) {
//    if (_view.get_opacity() == 1) {
//      _target = 1;
//      _value = 1;
//      _timer.stop();
//      return;
//    }
//
//    if (_timer.is_running() && _target == 1) {
//      return;
//    }
//
//    _value = _view.get_opacity();
//    _target = 1;
//    _time = get_new_time();
//    _timer.start(_delta_time);
//    return;
//  }
//
//  _view.set_visible(true);
//  _value = 0;
//  _target = 1;
//  _view.set_opacity(_value);
//  _time = std::chrono::milliseconds(0);
//  _timer.start(_delta_time);
//}
//
// void fade_animator::fade_out() {
//  if (_view.is_visible()) {
//    if (_view.get_opacity() == 0) {
//      _target = 0;
//      _value = 0;
//      _timer.stop();
//      return;
//    }
//
//    if (_timer.is_running() && _target == 0) {
//      return;
//    }
//
//    _value = _view.get_opacity();
//    _target = 0;
//    _time = get_new_time();
//    _timer.start(_delta_time);
//    return;
//  }
//
//  _view.set_visible(false);
//  _value = 0;
//  _target = 0;
//  _view.set_opacity(0);
//  _timer.stop();
//}
//
// void fade_animator::timer_callback() {
//  _time += _delta_time;
//
//  bool is_fade_in = _value < _target;
//
//  if (_time >= (is_fade_in ? _fade_in_time : _fade_out_time)) {
//    if (_target) {
//      call_async([this]() { _view.set_opacity(_target); });
//      _value = _target;
//      _timer.stop();
//    }
//    else {
//      call_async([this]() {
//        _view.set_visible(false);
//        _view.set_opacity(_target);
//      });
//
//      _value = _target;
//      _timer.stop();
//    }
//  }
//  else {
//    float alpha = _VDNFT::lerp(_value, _target, is_fade_in ? get_in_time_ratio() : get_out_time_ratio());
//    call_async([this, alpha]() { _view.set_opacity(alpha); });
//  }
//}
//
// differed_fade_animator::differed_fade_animator(_VDNFT::view& __view)
//    : _fade(__view)
//    , _timer(*this) {}
//
// void differed_fade_animator::fade_in() {
//  _fade.fade_in();
//  _timer.stop();
//}
//
// void differed_fade_animator::fade_out() {
//  _hover_time = std::chrono::milliseconds(0);
//  _timer.start(_delta_time);
//}
//
// void differed_fade_animator::stop_fade_out() { _timer.stop(); }
//
// void differed_fade_animator::on_timer() {
//  _hover_time += _delta_time;
//  if (_hover_time >= _delay_time) {
//    _timer.stop();
//    _VDNFT::event_manager::post<_VDNFT::generic_event>([this]() { _fade.fade_out(); });
//  }
//}
//
// MTS_END_NAMESPACE
