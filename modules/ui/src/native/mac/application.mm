#include "mts/config.h"

#ifdef MTS_MACOS

#include "mts/ui/application.h"
#import <Cocoa/Cocoa.h>
#import "app_delegate.h"
#include "mts/print.h"
#include "mts/ui/event.h"

MTS_BEGIN_NAMESPACE

application::application_data application::app_data;

int application::create_application_impl(std::shared_ptr<application>&& app, int argc, const char* argv[]) {
  //  @autoreleasepool {
  //    // Setup code that might create autoreleased objects goes here.
  //  }

  //    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

  event_manager_proxy::set_main_thread();

  DragonFruitAppDelegate* ns_app = [[DragonFruitAppDelegate alloc] initWithApp:std::move(app)];
  [[NSApplication sharedApplication] setDelegate:ns_app];
  return NSApplicationMain(argc, argv);
}

application::~application() { mts::print("APP destructor"); }

MTS_END_NAMESPACE

#endif // MTS_MACOS
