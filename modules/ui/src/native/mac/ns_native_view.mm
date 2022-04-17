#include "mts/config.h"

#ifdef MTS_MACOS

#include "mts/ui/view.h"
#include "mts/ui/application.h"
#include "mts/print.h"
#include "mts/ui/native_view.h"
#include "mts/ui/event.h"
#include "mts/graphics/unordered_rect_set.h"

#include "mts/ui/desktop.h"

#include "mts/detail/mac_cf_helpers.h"
#include "mts/detail/mac_objc_helpers.h"
#include "../../../../graphics/src/native/mac/helpers.h"
#include "ns_bundle_info.h"
#include <thread>
#include <cctype>
#import <Cocoa/Cocoa.h>

MTS_BEGIN_NAMESPACE

inline CGFloat getMainScreenHeight() noexcept {
  if ([[NSScreen screens] count] == 0)
    return 0.0f;

  return [[[NSScreen screens] objectAtIndex:0] frame].size.height;
}

inline NSRect flippedScreenRect(NSRect r) noexcept {
  r.origin.y = getMainScreenHeight() - (r.origin.y + r.size.height);
  return r;
}

inline NSPoint flippedScreenPoint(NSPoint p) noexcept {
  p.y = getMainScreenHeight() - p.y;
  return p;
}

#define NV_TRACE(...) trace(MTS_PRETTY_FUNCTION __VA_OPT__(, ) __VA_ARGS__)

static NSView* create_view_instance();
static NSWindow* create_window_instance();

namespace {
std::vector<mts::display> native_get_displays();
} // namespace.

void desktop::native_desktop::init(desktop* d) { d->_displays = native_get_displays(); }

class ns_native_view final : public mts::native_view {
public:
  ns_native_view(view& __view, NSView* native, window::style_flags flags)
      : mts::native_view(__view)
      , _own_window(native == nullptr) {

    _VMTS::rect<int> bounds = native_view::get_view().get_bounds();

    NSRect rect = flippedScreenRect(to_cg_rect(bounds));

    _notification_center = [NSNotificationCenter defaultCenter];

    _ns_view = [create_view_instance() initWithFrame:rect];
    set_owner(_ns_view, this);

    if (!has_main_thread()) {
      [_ns_view performSelectorOnMainThread:ns_native_view::asyncMainThreadSelector withObject:NULL waitUntilDone:NO];
    }

    [_notification_center addObserver:_ns_view
                             selector:frameChangedSelector
                                 name:NSViewFrameDidChangeNotification
                               object:_ns_view];

    [_ns_view setPostsFrameChangedNotifications:YES];

    using sf = window::style_flags;

    if (_own_window) {
      NSWindowStyleMask wstyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
          | NSWindowStyleMaskMiniaturizable;

      if (_VMTS::has_flag(sf::has_close_button, flags)) {
        wstyle |= NSWindowStyleMaskClosable;
      }
      else {
        wstyle &= ~(NSWindowStyleMaskClosable);
      }

      if (_VMTS::has_flag(sf::has_title_bar, flags)) {
        wstyle |= NSWindowStyleMaskTitled;
      }
      else {
        wstyle &= ~(NSWindowStyleMaskTitled);
      }

      if (_VMTS::has_flag(sf::has_minimise_button, flags)) {
        wstyle |= NSWindowStyleMaskMiniaturizable;
      }
      else {
        wstyle &= ~(NSWindowStyleMaskMiniaturizable);
      }

      if (_VMTS::has_flag(sf::is_resizable, flags)) {
        wstyle |= NSWindowStyleMaskResizable;
      }
      else {
        wstyle &= ~(NSWindowStyleMaskResizable);
      }

      _ns_window = [create_window_instance() initWithContentRect:rect
                                                       styleMask:wstyle
                                                         backing:NSBackingStoreBuffered
                                                           defer:YES];

      set_owner(_ns_window, this);

      if (_VMTS::has_flag(sf::has_drop_shadow, flags)) {

        [_ns_window setHasShadow:TRUE];
      }

      [_ns_window setBackgroundColor:[NSColor clearColor]];

      [_ns_window orderOut:nil];
      [_ns_window setDelegate:(id<NSWindowDelegate>)_ns_window];

      [_ns_window setContentView:_ns_view];
      [_ns_window setAcceptsMouseMovedEvents:YES];

      // We'll both retain and also release this on closing because plugin hosts can unexpectedly
      // close the window for us, and also tend to get cause trouble if setReleasedWhenClosed is NO.
      [_ns_window setReleasedWhenClosed:YES];
      [_ns_window retain];

      [_notification_center addObserver:_ns_view
                               selector:frameChangedSelector
                                   name:NSWindowDidMoveNotification
                                 object:_ns_window];

      [_notification_center addObserver:_ns_view
                               selector:frameChangedSelector
                                   name:NSWindowDidMiniaturizeNotification
                                 object:_ns_window];

      if (window* w = get_window()) {
        set_title(w->get_title());
      }
    }
  }

  virtual ~ns_native_view() override {
//    NV_TRACE();

    [_notification_center removeObserver:_ns_view];
    set_owner(_ns_view, nullptr);

    if ([_ns_view superview] != nil) {
      view_will_move_to_window(nullptr);
      [_ns_view removeFromSuperview];
    }

    if (_own_window) {
      set_owner(_ns_window, nullptr);
      [_ns_window setContentView:nil];
      [_ns_window close];
      [_ns_window release];
    }

    [_ns_view release];
  }

  virtual void set_bounds(const _VMTS::rect<int>& bounds) override {
    NSRect r = to_cg_rect(bounds);
    _VMTS::size<int> old_view_size = from_cg_rect<mts::rect<int>>([_ns_view frame]).size;

    if (_own_window) {
      [_ns_window setFrame:[_ns_window frameRectForContentRect:flippedScreenRect(r)] display:TRUE];
    }
    else {
      [_ns_view setFrame:r];
    }

    if (old_view_size != bounds.size) {
      [_ns_view setNeedsDisplay:true];
    }
  }

  virtual _VMTS::point<int> get_screen_position() const override { return get_bounds_internal(true).position; }

  _VMTS::rect<int> get_bounds_internal(const bool global) const {
    NSRect r = [_ns_view frame];
    NSWindow* viewWindow = [_ns_view window];

    if (global && viewWindow != nil) {
      r = [[_ns_view superview] convertRect:r toView:nil];
      r = [viewWindow convertRectToScreen:r];

      r = flippedScreenRect(r);
    }

      return from_cg_rect<mts::rect<int>>(r);
  }

  virtual _VMTS::rect<int> get_bounds() const override { return get_bounds_internal(_own_window); }

  virtual _VMTS::padding<int> get_frame_padding() const override {
    if (!_own_window) {
      return _VMTS::padding<int>{};
    }

    NSRect v = [_ns_view convertRect:[_ns_view frame] toView:nil];
    NSRect w = [_ns_window frame];

    return _VMTS::padding<int>((int)v.origin.x, (int)(w.size.width - (v.origin.x + v.size.width)), (int)v.origin.y,
        (int)(w.size.height - (v.origin.y + v.size.height)));
  }

  virtual void set_title(const std::string& title) override {
    if (!_own_window) {
      return;
    }

    NSString* stitle = [NSString stringWithUTF8String:title.c_str()];
    [_ns_window setTitle:stitle];
    [stitle release];
  }

  virtual void set_type(window::type __type) override {
    if (!_own_window) {
      return;
    }

    switch (__type) {
    case window::type::normal:
      [_ns_window setLevel:NSNormalWindowLevel];
      break;

    case window::type::floating:
      [_ns_window setLevel:NSFloatingWindowLevel];
      break;

    case window::type::status:
      [_ns_window setLevel:NSStatusWindowLevel];
      break;

    case window::type::modal:
      [_ns_window setLevel:NSModalPanelWindowLevel];
      break;

    case window::type::popup:
      [_ns_window setLevel:NSPopUpMenuWindowLevel];
      break;
    }
  }

  virtual void center() override {
    if (!_own_window) {
      return;
    }

    [_ns_window center];
  }

  virtual void show() override {
    //    [_ns_window makeKeyAndOrderFront:nil];
    //    [_ns_window setLevel:NSStatusWindowLevel];

    if (_ns_window) {
      [_ns_window orderFront:nil];
    }

    //    [_ns_window orderFrontRegardless];
    //    [_ns_window orderWindow:NSWindowAbove relativeTo:2];
    //    NSWindowOrderingMode

    //    - (void)orderWindow:(NSWindowOrderingMode)place relativeTo:(NSInteger)otherWin;
  }

  bool windowShouldClose() {
//    NV_TRACE(this);

    if (!native_view::is_valid_native_view(this)) {
      return YES;
    }

    get_view().set_visible(false);
    return NO;
  }

  virtual void set_cursor(cursor_type ct) override {
    switch (ct) {
    case cursor_type::normal:
      [[NSCursor arrowCursor] set];
      break;
    case cursor_type::move:
      [[NSCursor closedHandCursor] set];
      break;
    case cursor_type::resize:
      [[NSCursor resizeLeftRightCursor] set];
      break;

    case cursor_type::text:
      [[NSCursor IBeamCursor] set];
      break;
    }
  }

  virtual void close() override {
//    NV_TRACE(this);
    [_ns_window orderOut:nil];
    //    [_ns_window close];

    if (window* win = get_window()) {
//      NV_TRACE("Remove window from modal manager");
      desktop::get_modal_manager().remove(win);
    }
  }

  virtual void hide() override {
//    NV_TRACE(this);
    [_ns_window orderOut:nil];
    //    if(window* win = get_window()) {
    //      NV_TRACE("Remove window from modal manager");
    //      desktop::get_modal_manager().remove(win);
    //    }
  }

  virtual void set_visible(bool visible) override {
//    NV_TRACE(visible);

    if (_own_window) {
      if (visible) {
        [_ns_window orderFront:nil];
      }
      else {
        [_ns_window orderOut:nil];
      }
    }
    else {
      if (visible) {
        [_ns_view setHidden:false];
      }

      else if ([_ns_window firstResponder] != _ns_view
          || ([_ns_window firstResponder] == _ns_view && [_ns_window makeFirstResponder:nil])) {
        [_ns_view setHidden:true];
      }
    }
  }

  void on_moved_or_resized() { handle_bounds_changed(); }

  static inline bool is_forground_process() { return is_application_sandboxed() || [NSApp isActive]; }

  static inline void make_foreground_process() {
    if (!is_application_sandboxed()) {
      [NSApp activateIgnoringOtherApps:YES];
    }
  }

  static inline void activate_process() {
    if (is_application_sandboxed()) {
      return;
    }

    if (![NSApp isActive]) {
      [NSApp activateIgnoringOtherApps:YES];
    }
  }

  inline key_modifiers get_extra_mouse_modifiers(NSEvent* ev) {
    using flags = key_modifiers::flags;
    flags mods = flags::none;
    NSEventModifierFlags mflags = [ev modifierFlags];

    _VMTS::set_flag(flags::shift, _VMTS::has_flag<NSEventModifierFlags>(NSEventModifierFlagShift, mflags), mods);
    _VMTS::set_flag(flags::ctrl, _VMTS::has_flag<NSEventModifierFlags>(NSEventModifierFlagControl, mflags), mods);
    _VMTS::set_flag(flags::command, _VMTS::has_flag<NSEventModifierFlags>(NSEventModifierFlagCommand, mflags), mods);
    _VMTS::set_flag(flags::option, _VMTS::has_flag<NSEventModifierFlags>(NSEventModifierFlagOption, mflags), mods);

    return mods;
  }

  inline mouse_event::common create_common_mouse_event(NSEvent* ev) {
    key_modifiers mods = get_modifier_for_button_number([ev buttonNumber]);

    return mouse_event::common{
      mouse_event::source_type::mouse, //
      mods.with_flags(get_extra_mouse_modifiers(ev)), //
      get_mouse_position(ev, _ns_view), //
      (int)[ev clickCount] //
    };
  }

  inline void send_mouse_event(NSEventType ev_type, NSPoint pos, NSEventModifierFlags mods) {
    NSEvent* evt = [NSEvent mouseEventWithType:ev_type
                                      location:pos
                                 modifierFlags:mods
                                     timestamp:0
                                  windowNumber:[_ns_window windowNumber]
                                       context:nil
                                   eventNumber:0
                                    clickCount:1
                                      pressure:1.];
    [_ns_window sendEvent:evt];
  }

  inline bool redirect_mouse_event_to_focused_view(NSEvent* evt, SEL selector, NSEventType evt_type) {
    if (!currentlyFocusedPeer || (currentlyFocusedPeer == this)) {
      return false;
    }

    NSPoint mouse_screen_pos = [_ns_window convertPointToScreen:[evt locationInWindow]];
    NSPoint mouse_pos = [currentlyFocusedPeer->_ns_window convertPointFromScreen:mouse_screen_pos];

    NSEvent* new_evt = [NSEvent mouseEventWithType:evt_type
                                          location:mouse_pos
                                     modifierFlags:[evt modifierFlags]
                                         timestamp:[evt timestamp]
                                      windowNumber:[currentlyFocusedPeer->_ns_window windowNumber]
                                           context:nil
                                       eventNumber:[evt eventNumber]
                                        clickCount:[evt clickCount]
                                          pressure:[evt pressure]];

    const NSUniquePtr<NSEvent> r([new_evt retain]);

//    NV_TRACE();

    [currentlyFocusedPeer->_ns_view performSelector:selector withObject:new_evt];

    return true;
  }

  inline bool redirect_mouse_event_to_focused_view(NSEvent* evt, SEL selector) {
    return redirect_mouse_event_to_focused_view(evt, selector, [evt type]);
  }

  void on_mouse_down(NSEvent* ev) {
    mts::print("native on_mouse_down");
    if (redirect_mouse_event_to_focused_view(ev, @selector(mouseDown:))) {
      return;
    }

    activate_process();

    mouse_event::common cevt = create_common_mouse_event(ev);
    key_modifiers mods = cevt.modifiers;

    if (mods.is_left_button_down()) {
      handle_mouse_left_down(cevt);
    }
    else if (mods.is_middle_button_down()) {
    }
    else if (mods.is_right_button_down()) {
      handle_mouse_right_down(cevt);
    }
  }

  void on_mouse_up(NSEvent* ev) {
    mts::print("native on_mouse_up");
    if (redirect_mouse_event_to_focused_view(ev, @selector(mouseUp:))) {
      return;
    }

    //    using flags = key_modifiers::flags;

    mouse_event::common cevt = {
      mouse_event::source_type::mouse, //
      key_modifiers(), //
      get_mouse_position(ev, _ns_view), //
      (int)[ev clickCount] //
    };

    key_modifiers mods = get_modifier_for_button_number([ev buttonNumber]);

    if (mods.is_left_button_down()) {
      handle_mouse_left_up(cevt);
    }
    else if (mods.is_middle_button_down()) {
    }
    else if (mods.is_right_button_down()) {
      handle_mouse_right_up(cevt);
    }

    mts::print("native post on_mouse_up");

    // TODO: Show cursor if needed.
  }

  void on_mouse_move(NSEvent* ev) {
    if (redirect_mouse_event_to_focused_view(ev, @selector(mouseMoved:))) {
      return;
    }

    mouse_event::common cevt = create_common_mouse_event(ev);
    handle_mouse_move(cevt);

    //    _view_manager.mouse_move(cevt);

    //    ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons();
    //
    //        NSPoint windowPos = [ev locationInWindow];
    //        NSPoint screenPos = [[ev window] convertRectToScreen: NSMakeRect (windowPos.x,
    //        windowPos.y, 1.0f, 1.0f)].origin;
    //
    //        if (isWindowAtPoint ([ev window], screenPos))
    //            sendMouseEvent (ev);
    //        else
    //            // moved into another window which overlaps this one, so trigger an exit
    //            handleMouseEvent (MouseInputSource::InputSourceType::mouse, MouseInputSource::offscreenMousePos,
    //            ModifierKeys::currentModifiers,
    //                              getMousePressure (ev), MouseInputSource::invalidOrientation, getMouseTime (ev));
    //
    //        showArrowCursorIfNeeded();
  }

  void on_mouse_left_drag(NSEvent* ev) {
    if (redirect_mouse_event_to_focused_view(ev, @selector(mouseMoved:), NSEventTypeMouseMoved)) {
      currentlyFocusedPeer->set_dragged_after_creation(true);
      return;
    }

    //    if(redirect_mouse_event_to_focused_view(ev, @selector(mouseDragged:))) {
    //      return;
    //    }

    mouse_event::common cevt = create_common_mouse_event(ev);
    handle_mouse_left_drag(cevt);

    //    ModifierKeys::currentModifiers = ModifierKeys::currentModifiers.withoutMouseButtons();
    //
    //        NSPoint windowPos = [ev locationInWindow];
    //        NSPoint screenPos = [[ev window] convertRectToScreen: NSMakeRect (windowPos.x,
    //        windowPos.y, 1.0f, 1.0f)].origin;
    //
    //        if (isWindowAtPoint ([ev window], screenPos))
    //            sendMouseEvent (ev);
    //        else
    //            // moved into another window which overlaps this one, so trigger an exit
    //            handleMouseEvent (MouseInputSource::InputSourceType::mouse, MouseInputSource::offscreenMousePos,
    //            ModifierKeys::currentModifiers,
    //                              getMousePressure (ev), MouseInputSource::invalidOrientation, getMouseTime (ev));
    //
    //        showArrowCursorIfNeeded();
  }

  void on_mouse_wheel(NSEvent* ev) {
    //    mts::print("dsdsdsdsdsdsdsdsd");
    //    if(redirect_mouse_event_to_focused_view(ev, @selector(scrollWheel:))) {
    //      return;
    //    }

    //        const float scale = 10.0f / 256.0f;
    //     float dx = scale * (float) [ev deltaX];
    //    float dy = scale * (float) [ev deltaY];

    float dx = (float)[ev deltaX];
    float dy = (float)[ev deltaY];

    if (dy == 0 && dx == 0) {
      return;
    }

    //    mts::print("AAAAAAAAA");
    bool is_reversed = false;
    if ([ev respondsToSelector:@selector(isDirectionInvertedFromDevice)]) {
      is_reversed = [ev isDirectionInvertedFromDevice];
    }
    //    mts::print("BBBBBBBB");

    mouse_event::common cevt = {
      mouse_event::source_type::mouse, //
      get_modifier_for_button_number([ev buttonNumber]), //
      get_mouse_position(ev, _ns_view), //
      0 //
    };
    handle_mouse_wheel(cevt, scroll_event(_VMTS::point<float>(dx, dy), is_reversed));

    //        updateModifiers (ev);
    //
    //        MouseWheelDetails wheel;
    //        wheel.deltaX = 0;
    //        wheel.deltaY = 0;
    //        wheel.isReversed = false;
    //        wheel.isSmooth = false;
    //        wheel.isInertial = false;
    //
    //        @try
    //        {
    //            if ([ev respondsToSelector: @selector (isDirectionInvertedFromDevice)])
    //                wheel.isReversed = [ev isDirectionInvertedFromDevice];
    //
    //            wheel.isInertial = ([ev momentumPhase] != NSEventPhaseNone);
    //
    //            if ([ev respondsToSelector: @selector (hasPreciseScrollingDeltas)])
    //            {
    //                if ([ev hasPreciseScrollingDeltas])
    //                {
    //                    const float scale = 0.5f / 256.0f;
    //                    wheel.deltaX = scale * (float) [ev scrollingDeltaX];
    //                    wheel.deltaY = scale * (float) [ev scrollingDeltaY];
    //                    wheel.isSmooth = true;
    //                }
    //            }
    //            else if ([ev respondsToSelector: @selector (deviceDeltaX)])
    //            {
    //                wheel.deltaX = checkDeviceDeltaReturnValue ([ev deviceDeltaX]);
    //                wheel.deltaY = checkDeviceDeltaReturnValue ([ev deviceDeltaY]);
    //            }
    //        }
    //        @catch (...)
    //        {}
    //
    //        if (wheel.deltaX == 0.0f && wheel.deltaY == 0.0f)
    //        {
    //            const float scale = 10.0f / 256.0f;
    //            wheel.deltaX = scale * (float) [ev deltaX];
    //            wheel.deltaY = scale * (float) [ev deltaY];
    //        }
    //
    //        handleMouseWheel (MouseInputSource::InputSourceType::mouse, getMousePos (ev, view), getMouseTime (ev),
    //        wheel);
  }

  static int getKeyCodeFromEvent(NSEvent* ev) {
    // Unfortunately, charactersIgnoringModifiers does not ignore the shift key.
    // Using [ev keyCode] is not a solution either as this will,
    // for example, return VK_KEY_Y if the key is pressed which
    // is typically located at the Y key position on a QWERTY
    // keyboard. However, on international keyboards this might not
    // be the key labeled Y (for example, on German keyboards this key
    // has a Z label). Therefore, we need to query the current keyboard
    // layout to figure out what character the key would have produced
    // if the shift key was not pressed
    std::string unmodified = [[ev charactersIgnoringModifiers] UTF8String];
    std::u16string u16_unmod = _VMTS::utf8::utf8to16(unmodified);

    int keyCode = (int)u16_unmod[0];
    //    mts::print("KKKKKK", u16_unmod[0]);
    switch (keyCode) {
    case 0x19:
      // backwards-tab.
      keyCode = '\t';
      break;

    case 0x03:
      // enter.
      keyCode = '\r';
      break;

    default:
      keyCode = std::toupper(keyCode);
    }

    if (([ev modifierFlags] & NSEventModifierFlagNumericPad) != 0) {
      // clang-format off
      const int numPadConversions[] = {
        '0', key_event::numberPad0,
        '1', key_event::numberPad1,
        '2', key_event::numberPad2,
        '3', key_event::numberPad3,
        '4', key_event::numberPad4,
        '5', key_event::numberPad5,
        '6', key_event::numberPad6,
        '7', key_event::numberPad7,
        '8', key_event::numberPad8,
        '9', key_event::numberPad9,
        '+', key_event::numberPadAdd,
        '-', key_event::numberPadSubtract,
        '*', key_event::numberPadMultiply,
        '/', key_event::numberPadDivide,
        '.', key_event::numberPadDecimalPoint,
        ',', key_event::numberPadDecimalPoint, // (to deal with non-english kbds)
        '=', key_event::numberPadEquals
      };
      // clang-format on

      for (std::size_t i = 0; i < _VMTS::array_size(numPadConversions); i += 2) {
        if (keyCode == numPadConversions[i]) {
          return numPadConversions[i + 1];
        }
      }
    }

    return keyCode;
  }

  static key_modifiers getKeyEventModifiers(const NSUInteger key_flags) {
    using flags = key_modifiers::flags;
    flags mods = flags::none;

    if ((key_flags & NSEventModifierFlagShift) != 0) {
      _VMTS::set_flag(flags::shift, true, mods);
    }

    if ((key_flags & NSEventModifierFlagControl) != 0) {
      _VMTS::set_flag(flags::ctrl, true, mods);
    }

    if ((key_flags & NSEventModifierFlagOption) != 0) {
      _VMTS::set_flag(flags::option, true, mods);
    }

    if ((key_flags & NSEventModifierFlagCommand) != 0) {
      _VMTS::set_flag(flags::command, true, mods);
    }

    return mods;
  }

  bool on_key_down(NSEvent* ev) {

    //
    int key_code = getKeyCodeFromEvent(ev);

    char32_t textCharacter = _VMTS::utf8::convert_first_utf8_char_to_32([[ev characters] UTF8String]);
    //
    switch (key_code) {
    case NSLeftArrowFunctionKey:
    case NSRightArrowFunctionKey:
    case NSUpArrowFunctionKey:
    case NSDownArrowFunctionKey:
    case NSPageUpFunctionKey:
    case NSPageDownFunctionKey:
    case NSEndFunctionKey:
    case NSHomeFunctionKey:
    case NSDeleteFunctionKey:
      textCharacter = 0;
      break; // (these all seem to generate unwanted garbage unicode strings)

    default:
      if (([ev modifierFlags] & NSEventModifierFlagCommand) != 0
          || (key_code >= NSF1FunctionKey && key_code <= NSF35FunctionKey))
        textCharacter = 0;
      break;
    }

    return handle_key_down(key_event(key_code, getKeyEventModifiers([ev modifierFlags]), textCharacter));
    //    return handle_key_down(key_event(getKeyCodeFromEvent(ev), getKeyEventModifiers([ev modifierFlags]),
    //        convert_first_utf8_char_to_32([[ev characters] UTF8String])));

    //    return true;
  }

  bool on_key_up(NSEvent* ev) {
    handle_key_up(key_event(getKeyCodeFromEvent(ev), getKeyEventModifiers([ev modifierFlags]),
        _VMTS::utf8::convert_first_utf8_char_to_32([[ev characters] UTF8String])));
    return true;
  }

  void draw_rect(NSRect rect) {
    std::vector<_VMTS::rect<float>> frects;
    //    fst::mts::view_manager::rect_vector frects;

    if (_dirty_rects.empty()) {
      frects.push_back(from_cg_rect<mts::rect<float>>(rect));
    }
    else {
      frects.insert(frects.begin(), _dirty_rects.begin(), _dirty_rects.end());
      _dirty_rects.clear();
    }

    [[NSColor clearColor] set];
    NSRectFill(rect);

    //    [[NSColor clearColor] set];
    //        NSRectFillUsingOperation(rect, NSCompositeSourceOver);

    //    [[NSColor greenColor] setFill];
    //    NSRectFill(rect);

    NSGraphicsContext* context = [NSGraphicsContext currentContext];
      mts::context gc(context.CGContext);
    //    handle_paint(gc);

    handle_paint(gc, frects);
  }

  void on_main_thread() { handle_set_main_thread(); }

  bool canBecomeKeyWindow() {
    return get_view().is_visible()
        && get_view().wants_keyboard_focus(); // component.isVisible();// && (getStyleFlags() &
                                              // juce::ComponentPeer::windowIgnoresKeyPresses) == 0;
  }
  //  bool canBecomeKeyWindow() {
  //    return get_view().is_visible(); // component.isVisible();// && (getStyleFlags() &
  //                                    // juce::ComponentPeer::windowIgnoresKeyPresses) == 0;
  //  }

  bool canBecomeMainWindow() {
    return get_view().is_visible(); // component.isVisible() && dynamic_cast<ResizableWindow*> (&component) != nullptr;
  }

  //  bool worksWhenModal() const
  //  {
  //      // In plugins, the host could put our plugin window inside a modal window, so this
  //      // allows us to successfully open other popups. Feels like there could be edge-case
  //      // problems caused by this, so let us know if you spot any issues..
  //      return ! JUCEApplication::isStandaloneApp();
  //  }

  void becomeKeyWindow() {
    //      handleBroughtToFront();
    grab_focus();
  }

  void resignKeyWindow() { viewFocusLoss(); }

  void viewFocusGain() {

    if (currentlyFocusedPeer != this) {
      if (currentlyFocusedPeer && is_valid_native_view(currentlyFocusedPeer)) {
        currentlyFocusedPeer->handle_focus_lost();
      }

      currentlyFocusedPeer = this;
      handle_focus_gained();
    }
  }

  void viewFocusLoss() {
    mts::print("native_view::viewFocusLoss");
    if (currentlyFocusedPeer == this) {
      currentlyFocusedPeer = nullptr;
      handle_focus_lost();
    }
  }

  void become_key() {
    //    repaint();
  }

  void view_will_move_to_window(NSWindow* new_window) {
      _VMTS::unused(new_window);
      
    if (NSWindow* win = [_ns_view window]) {
      [_notification_center removeObserver:_ns_view name:NSWindowDidMoveNotification object:win];

      [_notification_center removeObserver:_ns_view name:NSWindowWillMiniaturizeNotification object:win];
    }

    //    if (isSharedWindow && [view window] == window && newWindow == nullptr)
    //    {
    //        if (auto* comp = safeComponent.get())
    //            comp->setVisible (false);
    //    }
  }

  void view_did_move_to_window() {

    if (NSWindow* win = [_ns_view window]) {
      [_notification_center addObserver:_ns_view
                               selector:dismissModalsSelector
                                   name:NSWindowDidMoveNotification
                                 object:win];

      [_notification_center addObserver:_ns_view
                               selector:dismissModalsSelector
                                   name:NSWindowWillMiniaturizeNotification
                                 object:win];
    }
  }

  bool isFocused() const {
    return (!_own_window || !application::is_standalone()) ? (this == currentlyFocusedPeer)
                                                                   : [_ns_window isKeyWindow];
    //        return (isSharedWindow || ! JUCEApplication::isStandaloneApp())
    //                    ? this == currentlyFocusedPeer
    //                    : [window isKeyWindow];
  }

  virtual void grab_focus() override {
    if (_ns_window != nil && [_ns_window canBecomeKeyWindow]) {
      [_ns_window makeKeyWindow];
      [_ns_window makeFirstResponder:_ns_view];

      viewFocusGain();

      NSPoint p2 = [_ns_window convertPointFromScreen:[NSEvent mouseLocation]];
      //      NSPoint p = [NSEvent mouseLocation];
      //      mts::print("DLKJDA", from_ns_point(p), "---",from_ns_point(p2), "---", from_ns_rect([_ns_window frame]));
      send_mouse_event(NSEventTypeMouseMoved, p2, 0);
    }
  }

  virtual bool has_focus() const override {
    if (_own_window) {
      return currentlyFocusedPeer == this;
    }

    return [_ns_window isKeyWindow];
    //    return (isSharedWindow || ! JUCEApplication::isStandaloneApp())
    //                ? this == currentlyFocusedPeer
    //                : [window isKeyWindow];

    //    return false;
  }

  virtual bool set_always_on_top(bool value) override {
    _VMTS::unused(value);
//    MTS_WARNING("TODO");

    if (!_own_window) {
      return false;
    }

    //    return false;

    //    if (! isSharedWindow)
    //        {
    //            [window setLevel: alwaysOnTop ? ((getStyleFlags() & windowIsTemporary) != 0 ? NSPopUpMenuWindowLevel
    //                                                                                        : NSFloatingWindowLevel)
    //                                          : NSNormalWindowLevel];
    //
    //            isAlwaysOnTop = alwaysOnTop;
    //        }
    //
    return true;
  }

  /// Brings the window to the top, optionally also giving it keyboard focus.
  virtual void to_front(bool with_focus) override {
    //    if(_own_window) {
    //      NSView* superview = [_ns_view superview];
    //    }
    //    if (isSharedWindow)
    //        {
    //            NSView* superview = [view superview];
    //            NSMutableArray* subviews = [NSMutableArray arrayWithArray: [superview subviews]];
    //
    //            const auto isFrontmost = [[subviews lastObject] isEqual: view];
    //
    //            if (! isFrontmost)
    //            {
    //                [view retain];
    //                [subviews removeObject: view];
    //                [subviews addObject: view];
    //
    //                [superview setSubviews: subviews];
    //                [view release];
    //            }
    //        }
    //

    if (_ns_window != nil && get_view().is_visible()) {
      if (with_focus) {
        [_ns_window makeKeyAndOrderFront:nil];
      }
      else {
        [_ns_window orderFront:nil];
      }
      //            ++insideToFrontCall;

      //            if (makeActiveWindow)
      //                [window makeKeyAndOrderFront: nil];
      //            else
      //                [window orderFront: nil];
      //
      //            if (insideToFrontCall <= 1)
      //            {
      //                Desktop::getInstance().getMainMouseSource().forceMouseCursorUpdate();
      //                handleBroughtToFront();
      //            }
      //
      //            --insideToFrontCall;
    }
  }

  /// Moves the window to be just behind another one.
  virtual void to_behind(native_view* other) override {
    _VMTS::unused(other);
    MTS_WARNING("TODO");
  }

  inline void dismiss_modals() { mts::print("native_view::dismiss_modals"); }

  // static void (*SuperSetNeedsDisplayInRect) (id, SEL, NSRect) = (void (*) (id, SEL, NSRect))objc_msgSendSuper;

  inline void update(const _VMTS::rect<float>& rect) {
    _dirty_rects.insert(rect);
    //    [_ns_view setNeedsDisplayInRect:to_cg_rect(rect)];

    //
    //#define __OBJC_SUPER(x) objc_super __os; __os.receiver = x; __os.super_class = class_getSuperclass ([x class]);
    //#define SUPER	static_cast<id> (&__os)
    //
    //    __OBJC_SUPER(_ns_view)
    //    SuperSetNeedsDisplayInRect (SUPER, _cmd, to_cg_rect(rect));
    //
    ////    objc_msgSendSuper
    ////    SEL theSelector = @selector(setNeedsDisplayInRect:);
    ////    IMP theImplementation = [[[_ns_view class] superclass] methodForSelector:theSelector];
    ////    theImplementation([[_ns_view class] superclass], theSelector, to_cg_rect(rect));
    //
    ////    [[[_ns_view class] superclass] methodForSelector:
    ///@selector(setNeedsDisplayInRect:)to_cg_rect(rect)];
  }

  virtual void repaint(const _VMTS::rect<float>& area) override {
    [_ns_view setNeedsDisplayInRect: to_cg_rect(area)];
  }

  NSRect constrain_rect(const NSRect r) {

    mts::rect<int> bounds = from_cg_rect<mts::rect<int>>(r);
    bounds.size = get_size_constrain(bounds.size);
    return to_cg_rect(bounds);

    //      return r;

    //      if(_VMTS::window* w =dynamic_cast<_VMTS::window*>(&get_view())) {
    //        mts::print("constrain_rect--", get_bounds());
    //        mts::rect bounds = mts::from_ns_rect(r);
    //        w->get_size_constrain(bounds.size);
    //
    ////        virtual _VMTS::size get_size_constrain(const _VMTS::size& frame_size) const
    //
    ////      w->get_size_constrain(get_bounds());
    //    }

    //        if (constrainer == nullptr || isKioskMode())
    //            return r;
    //
    //        const auto scale = getComponent().getDesktopScaleFactor();
    //
    //        auto pos            = ScalingHelpers::unscaledScreenPosToScaled (scale, convertToRectInt
    //        (flippedScreenRect (r))); const auto original = ScalingHelpers::unscaledScreenPosToScaled (scale,
    //        convertToRectInt (flippedScreenRect ([window frame])));
    //
    //        const auto screenBounds = Desktop::getInstance().getDisplays().getTotalBounds (true);
    //
    //        const bool inLiveResize = [window inLiveResize];
    //
    //        if (! inLiveResize || isFirstLiveResize)
    //        {
    //            isFirstLiveResize = false;
    //
    //            isStretchingTop    = (pos.getY() != original.getY() && pos.getBottom() == original.getBottom());
    //            isStretchingLeft   = (pos.getX() != original.getX() && pos.getRight()  == original.getRight());
    //            isStretchingBottom = (pos.getY() == original.getY() && pos.getBottom() != original.getBottom());
    //            isStretchingRight  = (pos.getX() == original.getX() && pos.getRight()  != original.getRight());
    //        }
    //
    //        constrainer->checkBounds (pos, original, screenBounds,
    //                                  isStretchingTop, isStretchingLeft, isStretchingBottom, isStretchingRight);
    //
    //        return flippedScreenRect (makeNSRect (ScalingHelpers::scaledScreenPosToUnscaled (scale, pos)));
  }

  static const SEL frameChangedSelector;
  static const SEL asyncMouseDownSelector;
  static const SEL asyncMouseUpSelector;
  static const SEL asyncMainThreadSelector;
  static const SEL becomeKeySelector;
  static const SEL dismissModalsSelector;

  static inline _VMTS::point<float> get_mouse_position(NSEvent* e, NSView* view) {
    NSPoint p = [view convertPoint:[e locationInWindow] fromView:nil];
    return { (float)p.x, (float)p.y };
  }

  static int getMouseTime(NSEvent* e) noexcept {
    return (std::int64_t)([e timestamp] * 1000.0);
    //    return (mts::currentTimeMillis() - mts::time::get_global_start_time_ms()) + ((mts::int64)([e timestamp] *
    //    1000.0)); return  ((mts::int64)([e timestamp] * 1000.0)) ;//- mts::time::get_global_start_time_ms();
    //        return (Time::currentTimeMillis() - Time::getMillisecondCounter())
    //                 + (int64) ([e timestamp] * 1000.0);
  }

  static inline key_modifiers get_modifier_for_button_number(NSInteger num) {
    switch (num) {
    case 0:
      return key_modifiers::left_mouse_button;
    case 1:
      return key_modifiers::right_mouse_button;
    case 2:
      return key_modifiers::middle_mouse_button;
    }
    return key_modifiers::none;
  }

  virtual bool dragged_after_creation() const override { return _dragged_after_creation; }

  void set_dragged_after_creation(bool v) { _dragged_after_creation = v; }

private:
  NSView* _ns_view = nullptr;
  NSWindow* _ns_window = nullptr;
  bool _own_window = false;
  NSNotificationCenter* _notification_center = nullptr;
  bool _dragged_after_creation = false;

    unordered_rect_set<float> _dirty_rects;

  static ns_native_view* currentlyFocusedPeer;

  static void set_owner(id viewOrWindow, ns_native_view* newOwner) {
    object_setInstanceVariable(viewOrWindow, "owner", newOwner);
  }
};

ns_native_view* ns_native_view::currentlyFocusedPeer = nullptr;

const SEL ns_native_view::frameChangedSelector = @selector(frameChanged:);
const SEL ns_native_view::asyncMouseDownSelector = @selector(asyncMouseDown:);
const SEL ns_native_view::asyncMouseUpSelector = @selector(asyncMouseUp:);
const SEL ns_native_view::asyncMainThreadSelector = @selector(asyncMainThread:);
const SEL ns_native_view::becomeKeySelector = @selector(becomeKey:);
const SEL ns_native_view::dismissModalsSelector = @selector(dismissModals);

template <typename Base>
struct NSViewNativeViewWrapper : public Base {
  explicit NSViewNativeViewWrapper(const char* baseName)
      : Base(baseName) {
    Base::template addIvar<ns_native_view*>("owner");
  }

  static ns_native_view* get_owner(id self) { return getIvar<ns_native_view*>(self, "owner"); }
};

struct DnftNSViewClass : public NSViewNativeViewWrapper<ObjCClass<NSView>> {
  DnftNSViewClass()
      : NSViewNativeViewWrapper("MTSView_") {

    //        addMethod(@selector(dealloc), dealloc, "v@:");

    addMethod(@selector(isOpaque), isOpaque, "c@:");
    addMethod(@selector(drawRect:), drawRect, "v@:", @encode(NSRect));
    addMethod(@selector(setNeedsDisplayInRect:), setNeedsDisplayInRect, "v@:", @encode(NSRect));

    addMethod(@selector(mouseDown:), mouseDown, "v@:@");
    addMethod(@selector(mouseUp:), mouseUp, "v@:@");
    addMethod(@selector(mouseDragged:), mouseDragged, "v@:@");
    addMethod(@selector(mouseMoved:), mouseMoved, "v@:@");

    //        addMethod (@selector (mouseEntered:),                 mouseEntered,                      "v@:@");
    //        addMethod (@selector (mouseExited:),                  mouseExited,                       "v@:@");
    addMethod(@selector(rightMouseDown:), mouseDown, "v@:@");
    //        addMethod (@selector (rightMouseDragged:),            mouseDragged,                      "v@:@");
    addMethod(@selector(rightMouseUp:), mouseUp, "v@:@");
    addMethod(@selector(otherMouseDown:), mouseDown, "v@:@");
    //        addMethod (@selector (otherMouseDragged:),            mouseDragged,                      "v@:@");
    addMethod(@selector(otherMouseUp:), mouseUp, "v@:@");
    addMethod(@selector(scrollWheel:), scrollWheel, "v@:@");
    //        addMethod (@selector (magnifyWithEvent:),             magnify,                           "v@:@");
    addMethod(@selector(acceptsFirstMouse:), acceptsFirstMouse, "c@:@");
    //        addMethod (@selector (windowWillMiniaturize:),        windowWillMiniaturize,             "v@:@");
    //        addMethod (@selector (windowDidDeminiaturize:),       windowDidDeminiaturize,            "v@:@");
    addMethod(@selector(wantsDefaultClipping), wantsDefaultClipping, "c@:");

    //        addMethod(@selector(hitTest:), hitTest, "@@:", @encode(NSPoint));

    //        - (nullable NSView *)hitTest:(NSPoint)point;

    //        addMethod (@selector (worksWhenModal),                worksWhenModal,                    "c@:");

    addMethod(@selector(viewWillMoveToWindow:), willMoveToWindow, "v@:@");
    addMethod(@selector(viewDidMoveToWindow), viewDidMoveToWindow, "v@:");

    addMethod(@selector(keyDown:), keyDown, "v@:@");
    addMethod(@selector(keyUp:), keyUp, "v@:@");
    //        addMethod (@selector (viewWillDraw),                  viewWillDraw,                      "v@:");
    //        addMethod (@selector (keyDown:),                      keyDown,                           "v@:@");
    //        addMethod (@selector (keyUp:),                        keyUp,                             "v@:@");
    //        addMethod (@selector (insertText:),                   insertText,                        "v@:@");
    //        addMethod (@selector (doCommandBySelector:),          doCommandBySelector,               "v@::");
    //        addMethod (@selector (setMarkedText:selectedRange:),  setMarkedText,                     "v@:@", @encode
    //        (NSRange)); addMethod (@selector (unmarkText),                    unmarkText, "v@:"); addMethod (@selector
    //        (hasMarkedText),                 hasMarkedText,                     "c@:"); addMethod (@selector
    //        (conversationIdentifier),        conversationIdentifier,            "l@:"); addMethod (@selector
    //        (attributedSubstringFromRange:), attributedSubstringFromRange,      "@@:", @encode (NSRange)); addMethod
    //        (@selector (markedRange),                   markedRange,                       @encode (NSRange), "@:");
    //        addMethod (@selector (selectedRange),                 selectedRange,                     @encode
    //        (NSRange), "@:"); addMethod (@selector (firstRectForCharacterRange:),   firstRectForCharacterRange,
    //        @encode (NSRect), "@:", @encode (NSRange)); addMethod (@selector (characterIndexForPoint:),
    //        characterIndexForPoint,            "L@:", @encode (NSPoint)); addMethod (@selector
    //        (validAttributesForMarkedText),  validAttributesForMarkedText,      "@@:"); addMethod (@selector
    //        (flagsChanged:),                 flagsChanged,                      "v@:@");
    //
    addMethod(@selector(becomeFirstResponder), becomeFirstResponder, "c@:");
    addMethod(@selector(resignFirstResponder), resignFirstResponder, "c@:");
    addMethod(@selector(acceptsFirstResponder), acceptsFirstResponder, "c@:");
    //
    //        addMethod (@selector (draggingEntered:),              draggingEntered,                   @encode
    //        (NSDragOperation), "@:@"); addMethod (@selector (draggingUpdated:),              draggingUpdated, @encode
    //        (NSDragOperation), "@:@"); addMethod (@selector (draggingEnded:),                draggingEnded, "v@:@");
    //        addMethod (@selector (draggingExited:),               draggingExited,                    "v@:@");
    //        addMethod (@selector (prepareForDragOperation:),      prepareForDragOperation,           "c@:@");
    //        addMethod (@selector (performDragOperation:),         performDragOperation,              "c@:@");
    //        addMethod (@selector (concludeDragOperation:),        concludeDragOperation,             "v@:@");

    addMethod(@selector(isFlipped), isFlipped, "c@:");
    //
    addMethod(ns_native_view::dismissModalsSelector, dismissModals, "v@:");
    addMethod(ns_native_view::asyncMouseDownSelector, asyncMouseDown, "v@:@");
    addMethod(ns_native_view::asyncMouseUpSelector, asyncMouseUp, "v@:@");
    addMethod(ns_native_view::frameChangedSelector, frameChanged, "v@:@");
    addMethod(ns_native_view::becomeKeySelector, becomeKey, "v@:@");
    //
    //        addMethod (@selector (performKeyEquivalent:),           performKeyEquivalent,     "c@:@");

    addMethod(ns_native_view::asyncMainThreadSelector, asyncMainThread, "v@:");

    //
    //            addProtocol (@protocol (NSTextInput));
    //
    registerClass();
  }

  ~DnftNSViewClass() { mts::print("~DnftNSViewClass"); }

private:
  //  static void dealloc(id self, SEL) {
  //    mts::print("DEALLOC");
  //    if (auto* p = get_owner(self)) {
  //      mts::print("DEALLOC INSIDE");
  //      sendSuperclassMessage<void>(self, @selector(dealloc));
  //    }
  //  }

  //  - (void)setNeedsDisplayInRect:(NSRect)invalidRect {
  //  _dirty_rects.insert(from_cg_rect<float>(invalidRect));
  //  [super setNeedsDisplayInRect:invalidRect];
  //}
  //
  //- (void)setNeedsDisplay:(BOOL)value {
  //  [super setNeedsDisplay:value];
  //}

  //  static NSView* hitTest(id self, SEL, NSPoint pos) {
  //    NSView* view = self;
  //    NSPoint p = [view convertPoint:pos fromView:nil];
  //    mts::print("HIT TEST", from_ns_point(pos), from_ns_point(p));
  //    return self;
  //  }

  static void drawRect(id self, SEL, NSRect r) {
    if (auto* p = get_owner(self)) {
      p->draw_rect(r);
    }
  }

  static void setNeedsDisplayInRect(id self, SEL, NSRect invalidRect) {
    if (auto* p = get_owner(self)) {
      p->update(from_cg_rect<mts::rect<float>>(invalidRect));
      //      sendSuperclassMessage<void>(self, @selector(setNeedsDisplayInRect), invalidRect);

      sendSuperclassMessage<void>(self, @selector(setNeedsDisplayInRect:), invalidRect);

      //            frameRect = sendSuperclassMessage<NSRect, NSRect, NSScreen*> (self, @selector
      //            (constrainFrameRect:toScreen:),
      //                                                                          frameRect, screen);
    }
  }

  static void willMoveToWindow(id self, SEL, NSWindow* w) {
    if (auto* p = get_owner(self)) {
      p->view_will_move_to_window(w);
    }
  }

  //  static void mouseDown(id self, SEL s, NSEvent* ev) {
  //    asyncMouseDown(self, s, ev);
  //  }
  //
  //  static void mouseUp(id self, SEL s, NSEvent* ev) {
  //    asyncMouseUp(self, s, ev);
  //  }

  static void mouseDown(id self, SEL s, NSEvent* ev) {
    if (mts::application::is_standalone()) {
      asyncMouseDown(self, s, ev);
      return;
    }

    // In some host situations, the host will stop modal loops from working
    // correctly if they're called from a mouse event, so we'll trigger
    // the event asynchronously.
    [self performSelectorOnMainThread:ns_native_view::asyncMouseDownSelector withObject:ev waitUntilDone:NO];
  }

  static void mouseUp(id self, SEL s, NSEvent* ev) {
    if (mts::application::is_standalone()) {
      asyncMouseUp(self, s, ev);
      return;
    }

    // In some host situations, the host will stop modal loops from working
    // correctly if they're called from a mouse event, so we'll trigger
    // the event asynchronously.
    [self performSelectorOnMainThread:ns_native_view::asyncMouseUpSelector withObject:ev waitUntilDone:NO];
  }

  static void asyncMouseDown(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      p->on_mouse_down(ev);
    }
  }
  static void asyncMouseUp(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      p->on_mouse_up(ev);
    }
  }

  static void mouseMoved(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      p->on_mouse_move(ev);
    }
  }

  static void mouseDragged(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      p->on_mouse_left_drag(ev);
    }
  }

  static void asyncMainThread(id self, SEL) {
    if (auto* p = get_owner(self)) {
      p->on_main_thread();
    }
  }

  static void scrollWheel(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      p->on_mouse_wheel(ev);
    }
  }

  static void keyDown(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      if (!p->on_key_down(ev)) {
        sendSuperclassMessage<void>(self, @selector(keyDown:), ev);
      }
    }

    //      if (auto* owner = getOwner (self))
    //      {
    //          auto* target = owner->findCurrentTextInputTarget();
    //          owner->textWasInserted = false;
    //
    //          if (target != nullptr)
    //              [(NSView*) self interpretKeyEvents: [NSArray arrayWithObject: ev]];
    //          else
    //              owner->stringBeingComposed.clear();
    //
    //          if (! (owner->textWasInserted || owner->redirectKeyDown (ev)))
    //              sendSuperclassMessage<void> (self, @selector (keyDown:), ev);
    //      }
  }

  static void keyUp(id self, SEL, NSEvent* ev) {
    if (auto* p = get_owner(self)) {
      if (!p->on_key_up(ev)) {
        sendSuperclassMessage<void>(self, @selector(keyUp:), ev);
      }
    }

    //      auto* owner = getOwner (self);
    //
    //      if (! owner->redirectKeyUp (ev))
    //          sendSuperclassMessage<void> (self, @selector (keyUp:), ev);
  }

  //  static BOOL acceptsFirstMouse(id, SEL, NSEvent*) { return YES; }

  //  static void mouseMoved       (id self, SEL, NSEvent* ev)   { if (auto* p = getOwner (self))
  //    p->redirectMouseMove  (ev); } static void mouseEntered     (id self, SEL, NSEvent* ev)   { if (auto* p =
  //    getOwner (self)) p->redirectMouseEnter (ev); } static void mouseExited      (id self, SEL, NSEvent* ev)   { if
  //    (auto* p = getOwner (self)) p->redirectMouseExit  (ev); } static void scrollWheel      (id self, SEL, NSEvent*
  //    ev)   { if (auto* p = getOwner (self)) p->redirectMouseWheel (ev); } static void magnify          (id self, SEL,
  //    NSEvent* ev)   { if (auto* p = getOwner (self)) p->redirectMagnify    (ev); } static void copy             (id
  //    self, SEL, NSObject* s)   { if (auto* p = getOwner (self)) p->redirectCopy       (s);  } static void paste (id
  //    self, SEL, NSObject* s)   { if (auto* p = getOwner (self)) p->redirectPaste      (s);  } static void cut (id
  //    self, SEL, NSObject* s)   { if (auto* p = getOwner (self)) p->redirectCut        (s);  } static void selectAll
  //    (id self, SEL, NSObject* s)   { if (auto* p = getOwner (self)) p->redirectSelectAll  (s);  } static void
  //    willMoveToWindow (id self, SEL, NSWindow* w)   { if (auto* p = getOwner (self)) p->redirectWillMoveToWindow (w);
  //    }
  //
  static BOOL acceptsFirstMouse(id, SEL, NSEvent*) { return YES; }
  static BOOL wantsDefaultClipping(id, SEL) { return YES; } // (this is the default, but may want
  //    to customise it in future) static BOOL worksWhenModal (id self, SEL)                  { if (auto* p = getOwner
  //    (self)) return p->worksWhenModal(); return NO; }
  //

  static void frameChanged(id self, SEL, NSNotification*) {
    if (auto* p = get_owner(self)) {
      p->on_moved_or_resized();
    }
  }
  static void viewDidMoveToWindow(id self, SEL) {
    if (auto* p = get_owner(self)) {
      p->view_did_move_to_window();
    }
  }

  static void becomeKey(id self, SEL) {
    if (auto* p = get_owner(self)) {
      p->become_key();
    }
  }
  static BOOL isFlipped(id, SEL) { return true; }

  static void dismissModals(id self, SEL) {
    if (auto* p = get_owner(self)) {
      p->dismiss_modals();
    }
  }
  //
  //    static void viewWillDraw (id self, SEL)
  //    {
  //        // Without setting contentsFormat macOS Big Sur will always set the invalid area
  //        // to be the entire frame.
  //       #if defined (MAC_OS_X_VERSION_10_12) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
  //        if (NSFoundationVersionNumber > (double) NSFoundationVersionNumber10_11_Max)
  //        {
  //            CALayer* layer = ((NSView*) self).layer;
  //            layer.contentsFormat = kCAContentsFormatRGBA8Uint;
  //        }
  //       #endif
  //
  //        sendSuperclassMessage<void> (self, @selector (viewWillDraw));
  //    }
  //
  //    static void windowWillMiniaturize (id self, SEL, NSNotification*)
  //    {
  //        if (auto* p = getOwner (self))
  //        {
  //            if (p->isAlwaysOnTop)
  //            {
  //                // there is a bug when restoring minimised always on top windows so we need
  //                // to remove this behaviour before minimising and restore it afterwards
  //                p->setAlwaysOnTop (false);
  //                p->wasAlwaysOnTop = true;
  //            }
  //        }
  //    }
  //
  //    static void windowDidDeminiaturize (id self, SEL, NSNotification*)
  //    {
  //        if (auto* p = getOwner (self))
  //        {
  //            if (p->wasAlwaysOnTop)
  //                p->setAlwaysOnTop (true);
  //
  //            p->redirectMovedOrResized();
  //        }
  //    }
  //
  static BOOL isOpaque(id self, SEL) {
    auto* owner = get_owner(self);
    return owner == nullptr || owner->get_view().is_opaque();
  }
  //
  //    //==============================================================================
  //    static void keyDown (id self, SEL, NSEvent* ev)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            auto* target = owner->findCurrentTextInputTarget();
  //            owner->textWasInserted = false;
  //
  //            if (target != nullptr)
  //                [(NSView*) self interpretKeyEvents: [NSArray arrayWithObject: ev]];
  //            else
  //                owner->stringBeingComposed.clear();
  //
  //            if (! (owner->textWasInserted || owner->redirectKeyDown (ev)))
  //                sendSuperclassMessage<void> (self, @selector (keyDown:), ev);
  //        }
  //    }
  //
  //    static void keyUp (id self, SEL, NSEvent* ev)
  //    {
  //        auto* owner = getOwner (self);
  //
  //        if (! owner->redirectKeyUp (ev))
  //            sendSuperclassMessage<void> (self, @selector (keyUp:), ev);
  //    }
  //
  //    //==============================================================================
  //    static void insertText (id self, SEL, id aString)
  //    {
  //        // This commits multi-byte text when return is pressed, or after every keypress for western keyboards
  //        if (auto* owner = getOwner (self))
  //        {
  //            NSString* newText = [aString isKindOfClass: [NSAttributedString class]] ? [aString string] : aString;
  //
  //            if ([newText length] > 0)
  //            {
  //                if (auto* target = owner->findCurrentTextInputTarget())
  //                {
  //                    target->insertTextAtCaret (nsStringToJuce (newText));
  //                    owner->textWasInserted = true;
  //                }
  //            }
  //
  //            owner->stringBeingComposed.clear();
  //        }
  //    }
  //
  //    static void doCommandBySelector (id, SEL, SEL) {}
  //
  //    static void setMarkedText (id self, SEL, id aString, NSRange)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            owner->stringBeingComposed = nsStringToJuce ([aString isKindOfClass: [NSAttributedString class]]
  //                                                           ? [aString string] : aString);
  //
  //            if (auto* target = owner->findCurrentTextInputTarget())
  //            {
  //                auto currentHighlight = target->getHighlightedRegion();
  //                target->insertTextAtCaret (owner->stringBeingComposed);
  //                target->setHighlightedRegion (currentHighlight.withLength (owner->stringBeingComposed.length()));
  //                owner->textWasInserted = true;
  //            }
  //        }
  //    }
  //
  //    static void unmarkText (id self, SEL)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            if (owner->stringBeingComposed.isNotEmpty())
  //            {
  //                if (auto* target = owner->findCurrentTextInputTarget())
  //                {
  //                    target->insertTextAtCaret (owner->stringBeingComposed);
  //                    owner->textWasInserted = true;
  //                }
  //
  //                owner->stringBeingComposed.clear();
  //            }
  //        }
  //    }
  //
  //    static BOOL hasMarkedText (id self, SEL)
  //    {
  //        auto* owner = getOwner (self);
  //        return owner != nullptr && owner->stringBeingComposed.isNotEmpty();
  //    }
  //
  //    static long conversationIdentifier (id self, SEL)
  //    {
  //        return (long) (pointer_sized_int) self;
  //    }
  //
  //    static NSAttributedString* attributedSubstringFromRange (id self, SEL, NSRange theRange)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            if (auto* target = owner->findCurrentTextInputTarget())
  //            {
  //                Range<int> r ((int) theRange.location,
  //                              (int) (theRange.location + theRange.length));
  //
  //                return [[[NSAttributedString alloc] initWithString: juceStringToNS (target->getTextInRange (r))]
  //                autorelease];
  //            }
  //        }
  //
  //        return nil;
  //    }
  //
  //    static NSRange markedRange (id self, SEL)
  //    {
  //        if (auto* owner = getOwner (self))
  //            if (owner->stringBeingComposed.isNotEmpty())
  //                return NSMakeRange (0, (NSUInteger) owner->stringBeingComposed.length());
  //
  //        return NSMakeRange (NSNotFound, 0);
  //    }
  //
  //    static NSRange selectedRange (id self, SEL)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            if (auto* target = owner->findCurrentTextInputTarget())
  //            {
  //                auto highlight = target->getHighlightedRegion();
  //
  //                if (! highlight.isEmpty())
  //                    return NSMakeRange ((NSUInteger) highlight.getStart(),
  //                                        (NSUInteger) highlight.getLength());
  //            }
  //        }
  //
  //        return NSMakeRange (NSNotFound, 0);
  //    }
  //
  //    static NSRect firstRectForCharacterRange (id self, SEL, NSRange)
  //    {
  //        if (auto* owner = getOwner (self))
  //            if (auto* comp = dynamic_cast<Component*> (owner->findCurrentTextInputTarget()))
  //                return flippedScreenRect (makeNSRect (comp->getScreenBounds()));
  //
  //        return NSZeroRect;
  //    }
  //
  //    static NSUInteger characterIndexForPoint (id, SEL, NSPoint)     { return NSNotFound; }
  //    static NSArray* validAttributesForMarkedText (id, SEL)          { return [NSArray array]; }
  //
  //    //==============================================================================
  //    static void flagsChanged (id self, SEL, NSEvent* ev)
  //    {
  //        if (auto* owner = getOwner (self))
  //            owner->redirectModKeyChange (ev);
  //    }
  //
  static BOOL becomeFirstResponder(id self, SEL) {
    if (auto* owner = get_owner(self)) {
      owner->viewFocusGain();
    }

    return YES;
  }
  //
  static BOOL resignFirstResponder(id self, SEL) {
    if (auto* owner = get_owner(self)) {
      owner->viewFocusLoss();
    }

    return YES;
  }

  static BOOL acceptsFirstResponder(id self, SEL) {
    auto* owner = get_owner(self);
    return owner != nullptr && owner->canBecomeKeyWindow();
  }
  //
  //    //==============================================================================
  //    static NSDragOperation draggingEntered (id self, SEL s, id<NSDraggingInfo> sender)
  //    {
  //        return draggingUpdated (self, s, sender);
  //    }
  //
  //    static NSDragOperation draggingUpdated (id self, SEL, id<NSDraggingInfo> sender)
  //    {
  //        if (auto* owner = getOwner (self))
  //            if (owner->sendDragCallback (0, sender))
  //                return NSDragOperationGeneric;
  //
  //        return NSDragOperationNone;
  //    }
  //
  //    static void draggingEnded (id self, SEL s, id<NSDraggingInfo> sender)
  //    {
  //        draggingExited (self, s, sender);
  //    }
  //
  //    static void draggingExited (id self, SEL, id<NSDraggingInfo> sender)
  //    {
  //        if (auto* owner = getOwner (self))
  //            owner->sendDragCallback (1, sender);
  //    }
  //
  //    static BOOL prepareForDragOperation (id, SEL, id<NSDraggingInfo>)
  //    {
  //        return YES;
  //    }
  //
  //    static BOOL performDragOperation (id self, SEL, id<NSDraggingInfo> sender)
  //    {
  //        auto* owner = getOwner (self);
  //        return owner != nullptr && owner->sendDragCallback (2, sender);
  //    }
  //
  //    static void concludeDragOperation (id, SEL, id<NSDraggingInfo>) {}
  //
  //    //==============================================================================
  //    static BOOL getIsAccessibilityElement (id, SEL)
  //    {
  //        return NO;
  //    }
  //
  //    static NSArray* getAccessibilityChildren (id self, SEL)
  //    {
  //        return NSAccessibilityUnignoredChildrenForOnlyChild (getAccessibleChild (self));
  //    }
  //
  //    static id accessibilityHitTest (id self, SEL, NSPoint point)
  //    {
  //        return [getAccessibleChild (self) accessibilityHitTest: point];
  //    }
  //
  //    static id getAccessibilityFocusedUIElement (id self, SEL)
  //    {
  //        return [getAccessibleChild (self) accessibilityFocusedUIElement];
  //    }
  //
  //    static BOOL getAccessibilityIsIgnored (id, SEL)
  //    {
  //        return YES;
  //    }
  //
  //    static id getAccessibilityAttributeValue (id self, SEL, NSString* attribute)
  //    {
  //        if ([attribute isEqualToString: NSAccessibilityChildrenAttribute])
  //            return getAccessibilityChildren (self, {});
  //
  //        return sendSuperclassMessage<id> (self, @selector (accessibilityAttributeValue:), attribute);
  //    }
  //
  //    static bool tryPassingKeyEventToPeer (NSEvent* e)
  //    {
  //        if ([e type] != NSEventTypeKeyDown && [e type] != NSEventTypeKeyUp)
  //            return false;
  //
  //        if (auto* focused = Component::getCurrentlyFocusedComponent())
  //        {
  //            if (auto* peer = dynamic_cast<NSViewComponentPeer*> (focused->getPeer()))
  //            {
  //                return [e type] == NSEventTypeKeyDown ? peer->redirectKeyDown (e)
  //                                                      : peer->redirectKeyUp (e);
  //            }
  //        }
  //
  //        return false;
  //    }
  //
  //    static BOOL performKeyEquivalent (id self, SEL s, NSEvent* event)
  //    {
  //        // We try passing shortcut keys to the currently focused component first.
  //        // If the component doesn't want the event, we'll fall back to the superclass
  //        // implementation, which will pass the event to the main menu.
  //        if (tryPassingKeyEventToPeer (event))
  //            return YES;
  //
  //        return sendSuperclassMessage<BOOL> (self, s, event);
  //    }
};

struct DnftNSWindowClass : public NSViewNativeViewWrapper<ObjCClass<NSWindow>> {
  DnftNSWindowClass()
      : NSViewNativeViewWrapper("MTSWindow_") {

    //      addMethod (@selector (mouseMoved:),                   mouseMoved,                        "v@:@");
    addMethod(@selector(isOpaque), isOpaque, "c@:");

    addMethod(@selector(canBecomeKeyWindow), canBecomeKeyWindow, "c@:");
    addMethod(@selector(canBecomeMainWindow), canBecomeMainWindow, "c@:");
    addMethod(@selector(becomeKeyWindow), becomeKeyWindow, "v@:");
    addMethod(@selector(resignKeyWindow), resignKeyWindow, "v@:");
    addMethod(@selector(windowShouldClose:), windowShouldClose, "c@:@");
    addMethod(@selector(constrainFrameRect:toScreen:), constrainFrameRect, @encode(NSRect), "@:", @encode(NSRect), "@");
    addMethod(@selector(windowWillStartLiveResize:), windowWillStartLiveResize, "v@:@");
    addMethod(@selector(windowDidEndLiveResize:), windowDidEndLiveResize, "v@:@");

    addMethod(@selector(windowWillResize:toSize:), windowWillResize, @encode(NSSize), "@:@", @encode(NSSize));

    //        - (void)windowDidMove:(NSNotification *)notification;
    //    addMethod (@selector (windowWillResize:toSize:), windowWillResize, @encode
    //        (NSSize), "@:@", @encode (NSSize)); addMethod (@selector (windowDidExitFullScreen:),
    //        windowDidExitFullScreen,   "v@:@"); addMethod (@selector (windowWillEnterFullScreen:),
    //        windowWillEnterFullScreen, "v@:@"); addMethod (@selector (zoom:),                               zoom,
    //        "v@:@"); addMethod (@selector (windowWillStartLiveResize:),          windowWillStartLiveResize, "v@:@");
    //        addMethod (@selector (windowDidEndLiveResize:),             windowDidEndLiveResize,    "v@:@");
    //        addMethod (@selector (window:shouldPopUpDocumentPathMenu:), shouldPopUpPathMenu, "c@:@", @encode
    //        (NSMenu*));
    addMethod(@selector(isFlipped), isFlipped, "c@:");
    //
    //        addMethod (@selector (accessibilityTitle),                  getAccessibilityTitle,     "@@:");
    //        addMethod (@selector (accessibilityLabel),                  getAccessibilityLabel,     "@@:");
    //        addMethod (@selector (accessibilityTopLevelUIElement),      getAccessibilityWindow,    "@@:");
    //        addMethod (@selector (accessibilityWindow),                 getAccessibilityWindow,    "@@:");
    //        addMethod (@selector (accessibilityRole),                   getAccessibilityRole,      "@@:");
    //        addMethod (@selector (accessibilitySubrole),                getAccessibilitySubrole,   "@@:");
    //
    //        addMethod (@selector (window:shouldDragDocumentWithEvent:from:withPasteboard:),
    //                   shouldAllowIconDrag, "c@:@", @encode (NSEvent*), @encode (NSPoint), @encode (NSPasteboard*));
    //
    addProtocol(@protocol(NSWindowDelegate));

    registerClass();
  }

private:
  //  static void mouseMoved(id self, SEL, NSEvent* ev) {
  //    mts::print("MMMMMMMMMM");
  ////    if (auto* p = getOwner(self)) {
  ////      p->on_mouse_move(ev);
  ////    }
  //  }

  //==============================================================================
  static BOOL isOpaque(id self, SEL) {
    auto* owner = get_owner(self);
    return owner == nullptr || owner->get_view().is_opaque();
  }

  static BOOL isFlipped(id, SEL) { return true; }

  static BOOL canBecomeKeyWindow(id self, SEL) {
    auto* owner = get_owner(self);

    return owner != nullptr && owner->canBecomeKeyWindow();
    //                  && ! owner->isBlockedByModalComponent();
  }
  //
  static BOOL canBecomeMainWindow(id self, SEL) {
    auto* owner = get_owner(self);

    return owner != nullptr && owner->canBecomeMainWindow();
    //                  && ! owner->isBlockedByModalComponent();
  }

  static void becomeKeyWindow(id self, SEL) {
    sendSuperclassMessage<void>(self, @selector(becomeKeyWindow));

    if (auto* owner = get_owner(self)) {
      if (owner->canBecomeKeyWindow()) {
        owner->becomeKeyWindow();
        return;
      }

      // this fixes a bug causing hidden windows to sometimes become visible when the app regains focus
      if (!owner->get_view().is_visible())
        [(NSWindow*)self orderOut:nil];
    }
  }

  static void resignKeyWindow(id self, SEL) {
    sendSuperclassMessage<void>(self, @selector(resignKeyWindow));

    if (auto* owner = get_owner(self)) {
      owner->resignKeyWindow();
    }
  }

  static BOOL windowShouldClose(id self, SEL, id /*window*/) {
    auto* owner = get_owner(self);
    return owner == nullptr || owner->windowShouldClose();
  }

  static NSRect constrainFrameRect(id self, SEL, NSRect frameRect, NSScreen* screen) {
    _VMTS::unused(screen);
    MTS_WARNING("TODO");

    //    print("constrainFrameRect", mts::from_ns_rect(frameRect));
    if (auto* owner = get_owner(self)) {
      //      print("ownwer", owner);
      //            sendSuperclassMessage<NSRect, NSRect, NSScreen*>(self, @selector(constrainFrameRect:toScreen:),
      //            frameRect, screen);

      //              frameRect = sendSuperclassMessage<NSRect, NSRect, NSScreen*> (self, @selector
      //              (constrainFrameRect:toScreen:),
      //                                                                            frameRect, screen);
      //
      frameRect = owner->constrain_rect(frameRect);
    }

    return frameRect;
  }

  static NSSize windowWillResize(id self, SEL, NSWindow*, NSSize proposedFrameSize) {
    auto* owner = get_owner(self);

    if (!owner) {
      return proposedFrameSize;
    }

    mts::print("WINDOW WILL RESIZE");

    //          if (owner == nullptr || owner->isZooming)
    //              return proposedFrameSize;

    NSRect frameRect = flippedScreenRect([(NSWindow*)self frame]);
    frameRect.size = proposedFrameSize;

    frameRect = owner->constrain_rect(flippedScreenRect(frameRect));

    //          owner->dismissModals();

    return frameRect.size;
  }

  //    static void windowDidExitFullScreen (id self, SEL, NSNotification*)
  //    {
  //        if (auto* owner = getOwner (self))
  //            owner->resetWindowPresentation();
  //    }
  //
  //    static void windowWillEnterFullScreen (id self, SEL, NSNotification*)
  //    {
  //        if (SystemStats::getOperatingSystemType() <= SystemStats::MacOSX_10_9)
  //            return;
  //
  //        if (auto* owner = getOwner (self))
  //            if (owner->hasNativeTitleBar() && (owner->getStyleFlags() & ComponentPeer::windowIsResizable) == 0)
  //                [owner->window setStyleMask: NSWindowStyleMaskBorderless];
  //    }
  //
  //    static void zoom (id self, SEL, id sender)
  //    {
  //        if (auto* owner = getOwner (self))
  //        {
  //            {
  //                const ScopedValueSetter<bool> svs (owner->isZooming, true);
  //                sendSuperclassMessage<void> (self, @selector (zoom:), sender);
  //            }
  //
  //            owner->redirectMovedOrResized();
  //        }
  //    }
  //
  static void windowWillStartLiveResize(id self, SEL, NSNotification*) {
    if (auto* owner = get_owner(self)) {
    }
    //              owner->liveResizingStart();
  }

  static void windowDidEndLiveResize(id self, SEL, NSNotification*) {
    if (auto* owner = get_owner(self)) {
    }
    //              owner->liveResizingEnd();
  }

  //
  //    static bool shouldPopUpPathMenu (id self, SEL, id /*window*/, NSMenu*)
  //    {
  //        if (auto* owner = getOwner (self))
  //            return owner->windowRepresentsFile;
  //
  //        return false;
  //    }
  //
  //    static bool shouldAllowIconDrag (id self, SEL, id /*window*/, NSEvent*, NSPoint, NSPasteboard*)
  //    {
  //        if (auto* owner = getOwner (self))
  //            return owner->windowRepresentsFile;
  //
  //        return false;
  //    }
  //
  //    static NSString* getAccessibilityTitle (id self, SEL)
  //    {
  //        return [self title];
  //    }
  //
  //    static NSString* getAccessibilityLabel (id self, SEL)
  //    {
  //        return [getAccessibleChild (self) accessibilityLabel];
  //    }
  //
  //    static id getAccessibilityWindow (id self, SEL)
  //    {
  //        return self;
  //    }
  //
  //    static NSAccessibilityRole getAccessibilityRole (id, SEL)
  //    {
  //        return NSAccessibilityWindowRole;
  //    }
  //
  //    static NSAccessibilityRole getAccessibilitySubrole (id self, SEL)
  //    {
  //        if (@available (macOS 10.10, *))
  //            return [getAccessibleChild (self) accessibilitySubrole];
  //
  //        return nil;
  //    }
};

NSView* create_view_instance() {
  static DnftNSViewClass cls;
  return cls.createInstance();
}

NSWindow* create_window_instance() {
  static DnftNSWindowClass cls;
  return cls.createInstance();
}

void native_view::add_to_desktop(view* v, window::type type, window::style_flags flags, void* nativeWindowToAttachTo) {
  if (!v->_native_view) {
    v->_native_view = std::make_shared<ns_native_view>(*v, (NSView*)nativeWindowToAttachTo, flags);
  }

  v->_native_view->set_type(type);
  v->_native_view->show();
  v->_native_view->grab_focus();
}

// void view::add_to_desktop() {
//   using sf = window::style_flags;
//   _native_view = std::make_shared<ns_native_view>(*this, nullptr, sf::all);
//   _native_view->show();
// }

// void window::add_to_desktop(window::style_flags flags) {
//   _native_view = std::make_shared<ns_native_view>(*this, nullptr, flags);
//   _native_view->show();
// }
//
// void window::add_to_desktop_centered(window::style_flags flags) {
//   _native_view = std::make_shared<ns_native_view>(*this, nullptr, flags);
//
////  [_ns_window orderFront:nil];
//  _native_view->center();
//  _native_view->show();
//  _native_view->grab_focus();
//}

namespace {
std::vector<display> native_get_displays() {
  NSScreen* main_screen = [NSScreen mainScreen];

  std::vector<display> displays;

  for (NSScreen* s in [NSScreen screens]) {
    NSDictionary* description = [s deviceDescription];
    BOOL is_screen = [[description objectForKey:NSDeviceIsScreen] boolValue];

    if (!is_screen) {
      continue;
    }

    display dis;
    dis.is_main = (s == main_screen);

    NSSize screen_dpi = [[description objectForKey:NSDeviceResolution] sizeValue];
    dis.dpi = (screen_dpi.width + screen_dpi.height) * 0.5;

    if ([s respondsToSelector:@selector(backingScaleFactor)]) {
      dis.scale = s.backingScaleFactor;
    }
    else {
      dis.scale = 1;
    }

    dis.frame = from_cg_rect<mts::rect<int>>([s frame]);
    dis.visible_frame = from_cg_rect<mts::rect<int>>([s visibleFrame]);
    displays.push_back(std::move(dis));
  }

  return displays;
}
} // namespace.

const key_event::key_code key_event::spaceKey = ' ';
const key_event::key_code key_event::returnKey = 0x0d;
const key_event::key_code key_event::escapeKey = 0x1b;
const key_event::key_code key_event::backspaceKey = 0x7f;
const key_event::key_code key_event::leftKey = NSLeftArrowFunctionKey;
const key_event::key_code key_event::rightKey = NSRightArrowFunctionKey;
const key_event::key_code key_event::upKey = NSUpArrowFunctionKey;
const key_event::key_code key_event::downKey = NSDownArrowFunctionKey;
const key_event::key_code key_event::pageUpKey = NSPageUpFunctionKey;
const key_event::key_code key_event::pageDownKey = NSPageDownFunctionKey;
const key_event::key_code key_event::endKey = NSEndFunctionKey;
const key_event::key_code key_event::homeKey = NSHomeFunctionKey;
const key_event::key_code key_event::deleteKey = NSDeleteFunctionKey;
const key_event::key_code key_event::insertKey = -1;
const key_event::key_code key_event::tabKey = 9;
const key_event::key_code key_event::F1Key = NSF1FunctionKey;
const key_event::key_code key_event::F2Key = NSF2FunctionKey;
const key_event::key_code key_event::F3Key = NSF3FunctionKey;
const key_event::key_code key_event::F4Key = NSF4FunctionKey;
const key_event::key_code key_event::F5Key = NSF5FunctionKey;
const key_event::key_code key_event::F6Key = NSF6FunctionKey;
const key_event::key_code key_event::F7Key = NSF7FunctionKey;
const key_event::key_code key_event::F8Key = NSF8FunctionKey;
const key_event::key_code key_event::F9Key = NSF9FunctionKey;
const key_event::key_code key_event::F10Key = NSF10FunctionKey;
const key_event::key_code key_event::F11Key = NSF11FunctionKey;
const key_event::key_code key_event::F12Key = NSF12FunctionKey;
const key_event::key_code key_event::F13Key = NSF13FunctionKey;
const key_event::key_code key_event::F14Key = NSF14FunctionKey;
const key_event::key_code key_event::F15Key = NSF15FunctionKey;
const key_event::key_code key_event::F16Key = NSF16FunctionKey;
const key_event::key_code key_event::F17Key = NSF17FunctionKey;
const key_event::key_code key_event::F18Key = NSF18FunctionKey;
const key_event::key_code key_event::F19Key = NSF19FunctionKey;
const key_event::key_code key_event::F20Key = NSF20FunctionKey;
const key_event::key_code key_event::F21Key = NSF21FunctionKey;
const key_event::key_code key_event::F22Key = NSF22FunctionKey;
const key_event::key_code key_event::F23Key = NSF23FunctionKey;
const key_event::key_code key_event::F24Key = NSF24FunctionKey;
const key_event::key_code key_event::F25Key = NSF25FunctionKey;
const key_event::key_code key_event::F26Key = NSF26FunctionKey;
const key_event::key_code key_event::F27Key = NSF27FunctionKey;
const key_event::key_code key_event::F28Key = NSF28FunctionKey;
const key_event::key_code key_event::F29Key = NSF29FunctionKey;
const key_event::key_code key_event::F30Key = NSF30FunctionKey;
const key_event::key_code key_event::F31Key = NSF31FunctionKey;
const key_event::key_code key_event::F32Key = NSF32FunctionKey;
const key_event::key_code key_event::F33Key = NSF33FunctionKey;
const key_event::key_code key_event::F34Key = NSF34FunctionKey;
const key_event::key_code key_event::F35Key = NSF35FunctionKey;

const key_event::key_code key_event::numberPad0 = 0x30020;
const key_event::key_code key_event::numberPad1 = 0x30021;
const key_event::key_code key_event::numberPad2 = 0x30022;
const key_event::key_code key_event::numberPad3 = 0x30023;
const key_event::key_code key_event::numberPad4 = 0x30024;
const key_event::key_code key_event::numberPad5 = 0x30025;
const key_event::key_code key_event::numberPad6 = 0x30026;
const key_event::key_code key_event::numberPad7 = 0x30027;
const key_event::key_code key_event::numberPad8 = 0x30028;
const key_event::key_code key_event::numberPad9 = 0x30029;
const key_event::key_code key_event::numberPadAdd = 0x3002a;
const key_event::key_code key_event::numberPadSubtract = 0x3002b;
const key_event::key_code key_event::numberPadMultiply = 0x3002c;
const key_event::key_code key_event::numberPadDivide = 0x3002d;
const key_event::key_code key_event::numberPadSeparator = 0x3002e;
const key_event::key_code key_event::numberPadDecimalPoint = 0x3002f;
const key_event::key_code key_event::numberPadEquals = 0x30030;
const key_event::key_code key_event::numberPadDelete = 0x30031;
const key_event::key_code key_event::playKey = 0x30000;
const key_event::key_code key_event::stopKey = 0x30001;
const key_event::key_code key_event::fastForwardKey = 0x30002;
const key_event::key_code key_event::rewindKey = 0x30003;
MTS_END_NAMESPACE

#endif // MTS_MACOS
