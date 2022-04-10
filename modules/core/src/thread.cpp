#include "mts/thread.h"
#include "mts/util.h"

#undef __MTS_THREAD_USE_POSIX

#if __MTS_UNISTD__ && !__MTS_WINDOWS__
  #include <unistd.h>

  #ifdef _POSIX_THREADS
    #if _POSIX_THREADS > 0
      #define __MTS_THREAD_USE_POSIX 1
    #endif // _POSIX_THREADS > 0
  #endif // _POSIX_THREADS
#endif // __MTS_UNISTD__ && !__MTS_WINDOWS__

#ifndef __MTS_THREAD_USE_POSIX
  #define __MTS_THREAD_USE_POSIX 0
#endif

#if __MTS_WINDOWS__
  #include <windows.h>
#endif

#if __MTS_THREAD_USE_POSIX
  #include <pthread.h>

  #if __MTS_LINUX__
    #include <sys/prctl.h>
  #endif
#endif

#if !__MTS_WINDOWS__ && !__MTS_THREAD_USE_POSIX
  #warning No thread support
#endif

MTS_BEGIN_NAMESPACE

void set_thread_name(std::thread::native_handle_type handle, const char* name) {
#if __MTS_WINDOWS__
  #ifdef _DEBUG
  const DWORD MS_VC_EXCEPTION = 0x406D1388;

  //    #pragma pack(push, 8)
  MTS_PACKED_N_START(8)
  typedef struct tagTHREADNAME_INFO {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;
  MTS_PACKED_END
  //    #pragma pack(pop)

  DWORD dwThreadID = ::GetThreadId(static_cast<HANDLE>(handle));

  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = name.c_str();
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;

  __try {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
  } __except (GetExceptionCode() == EXCEPTION_NONCONTINUABLE_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER
                                                                       : EXCEPTION_CONTINUE_EXECUTION) {
  }
  #endif // _DEBUG.

#elif __MTS_THREAD_USE_POSIX && __MTS_LINUX__
  _VMTS::unused(handle);
  prctl(PR_SET_NAME, name, 0, 0, 0);

#elif defined(__APPLE__)
  _VMTS::unused(handle);
  pthread_setname_np(name);

#elif __MTS_THREAD_USE_POSIX
  pthread_setname_np(handle, name));
#else
  #warning No thread set name support
#endif
}

void kill_thread(std::thread::native_handle_type handle) {
#if __MTS_WINDOWS__
  TerminateThread(static_cast<HANDLE>(handle), 0);
#elif __MTS_THREAD_USE_POSIX
  pthread_cancel((pthread_t)handle);
#else
  #warning No thread kill support.
#endif
}

struct thread::impl : std::enable_shared_from_this<impl> {
  using mutex_type = std::recursive_mutex;

  handle _handle = nullptr;
  std::thread::id _id = std::thread::id();
  std::atomic<bool> _is_running = true;
  std::atomic<bool> _should_stop = false;
  _VMTS::waitable<> _waitable;
  _VMTS::waitable<> _start_wait;
  mutex_type _lock;

  inline void signal_stop() {
    _should_stop.exchange(true, std::memory_order_acquire);
    _waitable.notify();
  }

  inline void notify() { _waitable.notify(); }

  inline bool running() const noexcept { return _is_running.load(); }

  inline bool should_stop() const noexcept { return _should_stop.load(); }

  inline std::thread::id id() const noexcept { return _id; }

  inline handle native_handle() const noexcept { return _handle; }

  inline void start(std::unique_ptr<callback> cb) {
    std::thread t([d = shared_from_this(), fct = std::move(cb)]() { d->run(d, fct.get()); });

    _handle = t.native_handle();
    _id = t.get_id();
    t.detach();
    _start_wait.notify();
  }

  //    template <typename Fct>
  inline void run(std::shared_ptr<impl> __self, callback* cb) {
    if (_start_wait.wait()) {

      if (_should_stop.load()) {
        _is_running.store(false);
        return;
      }

      try {
        cb->run(proxy(*__self));
      } catch (...) {
        mts_error("");
      }
    }

    _is_running.store(false);
    _should_stop.store(true);
  }

  void join();

  bool stop(const std::chrono::nanoseconds& wait_ns);

  inline void wait_for(const std::chrono::nanoseconds& wait_time) const { _waitable.wait_for(wait_time); }

  inline void wait_until(
      const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& timeout) const {
    _waitable.wait_until(timeout);
  }
};

void thread::impl::join() {
  std::scoped_lock<mutex_type> lock(_lock);

  if (!_is_running.load()) {
    _handle = nullptr;
    _id = std::thread::id();
    return;
  }

  if (!_handle) {
    _id = std::thread::id();
    return;
  }

  while (_is_running.load()) {
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::microseconds(5));
  }

  _handle = nullptr;
  _id = std::thread::id();
}

bool thread::impl::stop(const std::chrono::nanoseconds& wait_ns) {
  std::scoped_lock<mutex_type> lock(_lock);

  if (!_is_running.load()) {
    _handle = nullptr;
    _id = std::thread::id();
    return true;
  }

  if (!_handle) {
    return true;
  }

  signal_stop();

  if (wait_ns == std::chrono::nanoseconds::zero()) {
    if (_is_running.load()) {
      return false;
    }

    _handle = nullptr;
    _id = std::thread::id();
    return true;
  }

  auto timeout = std::chrono::system_clock::now() + wait_ns;

  while (_is_running.load()) {
    if (std::chrono::system_clock::now() >= timeout) {
      return false;
    }

    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  _handle = nullptr;
  _id = std::thread::id();
  return true;
}

thread::thread(std::unique_ptr<callback> cb)
    : _impl(std::make_shared<impl>()) {
  _impl->start(std::move(cb));
}

thread::~thread() {

  if (!joinable()) {
    return;
  }

  if (!_impl->stop(std::chrono::milliseconds(500))) {

    // kill.
    kill_thread(_impl->_handle);

    _impl->_handle = nullptr;
    _impl->_id = std::thread::id();
  }
}

thread& thread::operator=(thread&& t) {
  mts_assert(!joinable(), "Can't call operator= when joinable()");
  if (joinable()) {

    thread tmp(std::move(*this));
    _impl = std::move(t._impl);
    return *this;
  }

  _impl = std::move(t._impl);
  return *this;
}

void thread::notify() {
  mts_assert(joinable(), "Can't call notify when not joinable()");
  if (!joinable()) {
    return;
  }

  _impl->notify();
}

void thread::signal_stop() {
  mts_assert(joinable(), "Can't call notify when not joinable()");
  if (!joinable()) {
    return;
  }

  _impl->signal_stop();
}

void thread::join() {
  mts_assert(joinable(), "Can't call join when not joinable()");
  if (!joinable()) {
    return;
  }

  _impl->join();
  _impl.reset();
}

bool thread::stop(const std::chrono::nanoseconds& wait_ns) {
  mts_assert(joinable(), "Can't call stop when not joinable()");
  if (!joinable()) {
    return true;
  }

  if (_impl->stop(wait_ns)) {
    _impl.reset();
    return true;
  }

  return false;
}

bool thread::running() const noexcept {
  return joinable() ? _impl->running() : false;
}

bool thread::should_stop() const noexcept {
  mts_assert(joinable(), "Can't call should_stop() when not joinable()");
  return joinable() ? _impl->should_stop() : true;
}

std::thread::id thread::id() const noexcept { return joinable() ? _impl->id() : std::thread::id(); }

thread::handle thread::native_handle() const noexcept { return joinable() ? _impl->native_handle() : nullptr; }

//
//
//
//
//
bool thread::proxy::running() const noexcept { return _impl.running(); }

bool thread::proxy::should_stop() const noexcept { return _impl.should_stop(); }

std::thread::id thread::proxy::id() const noexcept { return _impl.id(); }

thread::handle thread::proxy::native_handle() const noexcept { return _impl.native_handle(); }

void thread::proxy::yield() const { std::this_thread::yield(); }

void thread::proxy::wait_for(const std::chrono::nanoseconds& wait_time) const { _impl.wait_for(wait_time); }

void thread::proxy::wait_until(
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& timeout) const {
        _impl.wait_until(timeout);
}

MTS_END_NAMESPACE
