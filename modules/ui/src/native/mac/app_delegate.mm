#include "mts/config.h"

#ifdef MTS_MACOS

#include "app_delegate.h"
#include <iostream>
#include "mts/print.h"

MTS_BEGIN_NAMESPACE
class application_interface {
public:
  //  application_interface(std::shared_ptr<_VMTS::application> app) {
  //    _VMTS::application::app_data.app = app;
  //  }

  static void init(std::weak_ptr<mts::application> app) { mts::application::app_data.app = app; }
};
MTS_END_NAMESPACE

@implementation DragonFruitAppDelegate

- (void)dealloc {
  mts::print("DragonFruitAppDelegate dealloc");
  [super dealloc];
}

- (id)initWithApp:(std::shared_ptr<mts::application>&&)app_ptr {
  if (self = [super init]) {
    app = std::move(app_ptr);

      mts::application_interface::init(app);
  }

  return self;
}

//- (id)initWithRect:(NSRect)contentSize {
//
//  if ( self = [super init] ) {
//    std::cout << "DragonFruitAppDelegate::initWithRect" << std::endl;
//    window = [[NSWindow alloc]
//      initWithContentRect:contentSize
//			styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
//      backing:NSBackingStoreBuffered
//      defer:YES];
//
//		[window setTitle:@"Dragon Fruit"];
//
////    main_component* m_view = new main_component();
////    m_view->set_bounds(nu::rect(0, 0, contentSize.size.width, contentSize.size.height));
////    _view = std::make_unique<nu::view>(m_view, true);
//  }
//
//  return self;
//}

- (void)applicationWillFinishLaunching:(NSNotification*)notification {
  if (app) {
    app->init();
  }
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
  std::cout << "DragonFruitAppDelegate::applicationDidFinishLaunching" << std::endl;
  //  [window makeKeyAndOrderFront:self];
  //  [window center];
}

- (void)applicationWillTerminate:(NSNotification*)notification {
  std::cout << "DragonFruitAppDelegate::applicationWillTerminate" << std::endl;
  app.reset();
}

// Close application when clicking on the top left red "x" button.
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)app {
  return TRUE;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender {
  std::cout << "DragonFruitAppDelegate::applicationShouldTerminate" << std::endl;
  return NSTerminateNow;
}

@end

#endif // MTS_MACOS
