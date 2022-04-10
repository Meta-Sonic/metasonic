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
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

MTS_BEGIN_NAMESPACE
/// @class waitable.
/// @brief Uses a std::mutex and std::condition_variable to allow a thread to
///        wait/block until woken up by another thread using the notify() method.
template <typename MutexType = std::mutex>
class waitable {
public:
  using mutex_type = MutexType;

  inline constexpr waitable() = default;
  waitable(const waitable&) = delete;
  waitable(waitable&&) = delete;

  inline ~waitable() { notify(); }

  waitable& operator=(const waitable&) = delete;
  waitable& operator=(waitable&&) = delete;

  /// @brief Blocks the calling thread until notify() is called or until
  ///        wait_time expires.
  /// @param wait_time An object of type std::chrono::duration representing the
  ///        maximum time to spend waiting.
  ///        Note that wait_time must be small enough not to overflow
  ///        when added to std::chrono::steady_clock::now().
  /// @returns true if notify() was called or false if waitTime expired.
  template <class Rep, class Period>
  inline bool wait_for(const std::chrono::duration<Rep, Period>& wait_time) const {
    {
      unique_lock lock(_mutex);

      if (_notified) {
        _notified = false;
        return true;
      }

      if (_condition.wait_for(lock, wait_time, [&] { return (bool)_notified; })) {
        _notified = false;
        return true;
      }

      return false;
    }
  }

  /// @brief Blocks the calling thread until notify() is called or until timeout
  ///        time is reached.
  /// @param timeout An object of type std::chrono::time_point representing the
  ///        time when to stop waiting.
  /// @returns true if notify() was called or false if timeout time is reached.
  template <class ClockType, class DurationType>
  inline bool wait_until(const std::chrono::time_point<ClockType, DurationType>& timeout) const {
    {
      unique_lock lock(_mutex);

      if (_notified) {
        _notified = false;
        return true;
      }

      if (_condition.wait_until(lock, timeout, [&] { return (bool)_notified; })) {
        _notified = false;
        return true;
      }

      return false;
    }
  }

  /// @brief Blocks the calling thread until notify() is called.
  inline bool wait() const {
    {
      unique_lock lock(_mutex);

      if (_notified) {
        _notified = false;
        return true;
      }

      _condition.wait(lock, [&] { return (bool)_notified; });
      _notified = false;
      return true;
    }
  }

  /// @brief Wakes up any threads that are currently waiting on this object.
  /// @warning If called before any wait_for(...), wait_until(...) or wait(), the
  ///          next call to any waiting method
  ///          will return immediately.
  inline void notify() {
    {
      scoped_lock lock(_mutex);
      _notified = true;
      _condition.notify_all();
    }
  }

  inline void reset() {
    _mutex.lock();
    _notified = false;
    _mutex.unlock();
  }

private:
  using unique_lock = std::unique_lock<mutex_type>;
  using scoped_lock = std::scoped_lock<mutex_type>;

  mutable mutex_type _mutex;
  mutable std::condition_variable _condition;
  mutable std::atomic<bool> _notified = false;
};

MTS_END_NAMESPACE
