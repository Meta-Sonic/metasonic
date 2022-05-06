#include "mts/config.h"

#ifdef MTS_MACOS

  #include "mts/ui/event.h"
  #include "mts/print.h"
  #include "mts/util.h"

  //#import <Cocoa/Cocoa.h>
  #include "mts/detail/mac_cf_helpers.h"

MTS_BEGIN_NAMESPACE
class event_manager::pimpl {
public:
  pimpl(event_manager* em)
      : _evt_mamanger(em) {
    _run_loop = CFRunLoopGetMain();

    CFRunLoopSourceContext sourceContext;
    memset((void*)&sourceContext, 0, sizeof(sourceContext));
    sourceContext.info = this;
    sourceContext.perform = runLoopSourceCallback;
    _run_loop_source.reset(CFRunLoopSourceCreate(kCFAllocatorDefault, 1, &sourceContext));
    CFRunLoopAddSource(_run_loop, _run_loop_source.get(), kCFRunLoopCommonModes);
  }

  ~pimpl() {
    CFRunLoopRemoveSource(_run_loop, _run_loop_source.get(), kCFRunLoopCommonModes);
    CFRunLoopSourceInvalidate(_run_loop_source.get());
  }

  inline void wake_up() {
    CFRunLoopSourceSignal(_run_loop_source.get());
    CFRunLoopWakeUp(_run_loop);
  }

private:
  CFRunLoopRef _run_loop;
  cf_unique_ptr<CFRunLoopSourceRef> _run_loop_source;
  event_manager* _evt_mamanger;

  void run_loop_callback() noexcept {
    std::vector<std::shared_ptr<event>>& events = _evt_mamanger->_events;

    if (events.empty()) {
      return;
    }

    std::size_t size = mts::minimum(events.size(), 4);

  #if __MTS_HAS_EXCEPTIONS__
    try {
      for (std::size_t i = 0; i < size; i++) {
        events[i]->call();
      }
    } catch (const std::exception& e) {
      mts::errprint(e.what());
    } catch (...) {
      std::terminate();
      //      mts_error("Event error");
    }
  #else
    for (std::size_t i = 0; i < size; i++) {
      events[i]->call();
    }
  #endif // __MTS_HAS_EXCEPTIONS__.

    _evt_mamanger->_mutex.lock();
    events.erase(events.begin(), events.begin() + size);
    _evt_mamanger->_mutex.unlock();

    if (events.size()) {
      wake_up();
    }
  }

  static void runLoopSourceCallback(void* info) noexcept { static_cast<pimpl*>(info)->run_loop_callback(); }
};
MTS_END_NAMESPACE

//
//
//
MTS_BEGIN_NAMESPACE
event_manager::event_manager() { _pimpl = std::make_unique<pimpl>(this); }

event_manager::~event_manager() {}

event_manager& event_manager::get_instance() {
  static event_manager evt_manager;
  return evt_manager;
}

void event_manager::post(std::shared_ptr<event> evt) {
  event_manager& em = get_instance();
  em._mutex.lock();
  em._events.push_back(evt);
  em._mutex.unlock();
  em._pimpl->wake_up();
}

void event_manager::set_main_thread() {
  event_manager& em = get_instance();

  std::thread::id tid = std::this_thread::get_id();
  mts_assert(!(em._main_thread_id != std::thread::id() && em._main_thread_id != tid),
      "event_manager already has main thread id.");

  if (em._main_thread_id != tid) {
    em._main_thread_id = tid;

    //    if(em._events.size()) {
    //      em._pimpl->wake_up();
    //    }
  }
}

bool event_manager::has_main_thread() { return get_instance()._main_thread_id != std::thread::id(); }

bool event_manager::is_main_thread() { return get_instance()._main_thread_id == std::this_thread::get_id(); }

MTS_END_NAMESPACE

#endif // MTS_MACOS
