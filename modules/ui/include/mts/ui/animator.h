/////
///// Copyright (C) 2021, Meta-Sonic
///// All rights reserved.
/////
///// Proprietary and confidential.
///// Any unauthorized copying, alteration, distribution, transmission,
///// performance, display or other use of this material is strictly prohibited.
/////
///// Written by Alexandre Arsenault <alx.arsenault@gmail.com>
/////
//
//#pragma once
//#include "dragon-fruit/gui/view.h"
//#include "dragon-fruit/core/timer.h"
//
// MTS_BEGIN_NAMESPACE
// class fade_animator {
// public:
//  fade_animator(_VDNFT::view& __view);
//
//  void fade_in();
//  void fade_out();
//
// private:
//  void timer_callback();
//
//  _VDNFT::timer<fade_animator, &fade_animator::timer_callback> _timer;
//  _VDNFT::view& _view;
//  float _target = 1;
//  float _value = 1;
//  std::chrono::milliseconds _time = std::chrono::milliseconds(0);
//  std::chrono::milliseconds _delta_time = std::chrono::milliseconds(10);
//  std::chrono::milliseconds _fade_in_time = std::chrono::milliseconds(50);
//  std::chrono::milliseconds _fade_out_time = std::chrono::milliseconds(500);
//
//  inline float get_in_time_ratio() const { return _time.count() / (float)_fade_in_time.count(); }
//  inline float get_out_time_ratio() const { return _time.count() / (float)_fade_out_time.count(); }
//
//  inline std::chrono::milliseconds get_new_time() const {
//    return std::chrono::milliseconds((int)((_value) / ((_target - _value))));
//  }
//
//  template <typename _Fct>
//  inline void call_async(_Fct&& fct) {
//    _VDNFT::event_manager::post<_VDNFT::generic_event>(std::move(fct));
//  }
//};
//
// class differed_fade_animator {
// public:
//  differed_fade_animator(_VDNFT::view& __view);
//
//  void fade_in();
//  void fade_out();
//  void stop_fade_out();
//
// private:
//  void on_timer();
//
//  fade_animator _fade;
//  _VDNFT::timer<differed_fade_animator, &differed_fade_animator::on_timer> _timer;
//  std::chrono::milliseconds _hover_time = std::chrono::milliseconds(0);
//  std::chrono::milliseconds _delta_time = std::chrono::milliseconds(10);
//  std::chrono::milliseconds _delay_time = std::chrono::milliseconds(500);
//};
//
// MTS_END_NAMESPACE
