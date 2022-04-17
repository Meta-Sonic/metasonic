#include "main_view.h"

main_view::main_view()
:_button_1("a")
, _view_1("v1")
,_view_2("v2")
{
  set_wants_unfocused_key_events(true);
    
    
    set_name("M");
    
    _img = mts::get_assets()->images.get("logo");
    
    
//    mts::button_style btn_atts;
//    btn_atts.set<mts::style::button::border_color>(0xFF0000FF);
    
//    mts::style::button_style btn_atts;
//    btn_atts.background_color = 0x00AAAAFF;
//    btn_atts.hover_background_color = btn_atts.background_color.brighter(0.4f);
//    btn_atts.pressed_background_color = btn_atts.background_color.darker(0.4f);
//    btn_atts.pressed_text_color = 0xFF0000FF;
//    btn_atts.border_width = 1.0f;
//    _button_1.set_custom_style(btn_atts);

//    mts::style ss;
//    _button_1.attributes.set_style(&ss);
    
//    _button_1.set_style("button");
//    _button_1.attributes.set(btn_atts);
    _menu.add_item("A");
    _menu.add_item("B");
    
    _box.add_item("Small");
    _box.add_item("Medium");
    _box.add_item("Large");
    
    add(_box, {300, 200, 100, 25});
    
    set_style("button");
    _view_1.set_bounds({ 50, 50, 200, 200 });
    _view_1.set_text("BINGO");
    _view_1.set_attribute<mts::style::styled_view::text_alignment>(mts::alignment::justification::left_bottom);
    _view_1.set_attribute<mts::style::styled_view::text_offset>({10, -10});
    
    add(_view_1);
    _view_1.add(_view_2, {50, 100, 50, 50});
    
    _view_2.add_geometry<mts::shape::filled_rounded_rectangle_with_contour>(mts::rect<float>{2, 2, 46, 46}, 0xFF0000FF, 0x000000FF, 2, 5);
    
    _button_1.set_bounds({ 10, 10, 100, 20 });
    _button_1.set_text("1");
    _view_1.add(_button_1);
    _button_1.set_attribute<mts::style::button::background_color>(0x0000FFFF);
    _button_1.on_click = [this]() {
        mts::print("CLIECJ");
        _menu.show_menu(_button_1, _button_1.get_screen_bounds().next_down(3));
//        _button_1.set_visible(false);
    };
    
//    set_property("v1.a.b", 32);
//
//    mts::print("AAA", _button_1.get_property("b"));
//
////    _view_1.set_clipping(true);
////    _view_1.set_bounds({});
//
//    mts::managed_ptr<mts::object> pp(new mts::object(), true);
//
//    mts::print("LLLLLLL", pp.is_shared());
//    set_property("v1.v2.gg", 34);
//    mts::print("AAA", get_property("v1.v2.gg"));
//    mts::optional_call(get_child("v1.a"), [](mts::managed_ptr<mts::view> it){
//        it->set_attribute<mts::style::button::background_color>(0xf000FFFF);
//    });
////    if(auto it = get_child("v1.a")) {
////        it->set_attribute<mts::style::button::background_color>(0xf000FFFF);
////    }
//
//    mts::optional_call(get_child("v1.a").get(), &mts::view::set_attribute<mts::style::button::background_color>, mts::color(0xFF0000FF));
}


main_view::~main_view() { mts::print("main_view::~main_view"); }


void main_view::bounds_changed(const mts::view::bounds_status& bs) {
  //  mts::print("main_window::bounds_changed", get_bounds());
  
}

void main_view::property_changed(const std::string& name) {
    mts::print("property_changed", name, get_property(name));
}

//void main_view::visibily_changed() { mts::print("main_view::visibily_changed"); }
void main_view::view_being_deleted(mts::view& v) {
  mts::print("main_view::view_being_deleted", v.get_id(), get_children().size());
}

void main_view::mouse_left_down(const mts::mouse_event& evt) {

  mts::unused(evt);
  //    _button.set_style("default");
  //  set_style("default");

  //  _is_down = true;
  //  _current_color = get_color_attribute<mts::style::button::colors::background_color>().darker(0.3f);
  //  _current_text_color = get_style()->button.selected_text_color;
  //  repaint({ 0, 0, 10, 10 });
//  _button_3.move_y(5);
}
//
//void main_view::mouse_wheel(const mts::mouse_event& evt, const mts::scroll_event& sevt) {
//  _VDNFT::unused(evt);
//  //  evt.
//  //  mts::mouse_event ss(evt.get_source_type(), evt.get_absolute_position(),
//  //      evt.get_position() - _scroll_bar.get_position(), evt.get_modifiers(), evt.click_count());
//  //  //      modifier mods, int click_count = 1)
//  //  _scroll_bar.send_mouse_wheel(ss, sevt);
//
//  _scroll_bar.scroll(sevt.is_reversed() ? sevt.get_delta().y : -sevt.get_delta().y);
//}
//
//bool main_view::key_pressed(const mts::key_event& kevt) {
//  mts::print("main_view::key_pressed", kevt.get_string());
//  return true;
//}

// void main_view::paint(mts::graphics::context& gc) {
//   mts::print("main_view::paint");
//   gc.set_fill_color(0xFFFFFFFF);
//   gc.fill_rect(get_local_bounds());
//
//   geometries.draw(gc);
//
//   //  gc.draw_image(_img, mts::frect{ 0, 0, 30, 30 });
// }

//void main_view::observe_value_for_key_path(
//    const mts::object* sender, const std::string& key_path, mts::property msg) {
//  mts::unused(sender);
//  //    if (context == mts::object::global_context) {
//  if (key_path == "bounds") {
//    mts::print("main_view::bounds", msg);
////    if (sender == &_button_3) {
////      mts::print("main_view::bounds button_3");
////    }
////    else if (sender == &_button_4) {
////      mts::print("main_view::bounds button_4");
////    }
//  }
//  else if (key_path == "title") {
//    mts::print("main_view::title", msg);
//  }
//  else if (key_path == "bingo") {
//    mts::print("main_view::bingo", msg);
//  }
//  //    }
//}

void main_view::paint(mts::context& gc, const mts::rect<float>& dirty_rect) {
    
//    if(auto att = nss.get("view", "bg"); att.is_color()) {
//        gc.set_fill_color(att.get_color());
//    }
//    else {
//        gc.set_fill_color(0xEAEBEBFF);
//    }
    
//  gc.set_fill_color(0xEAEBEBFF);
    
    gc.set_fill_color(get_attribute<mts::style::global::background_color>());
  gc.fill_rect(dirty_rect);

  draw_geometries(gc, dirty_rect);
    
    gc.draw_image(_img, {300, 0});

//  gc.set_fill_color(0x000000FF);
//  gc.fill_path(_path_test2, { 10, 10, 10, 10 });
//
//  gc.set_fill_color(0x00FF00FF);
//  _VDNFT::fpoint tl = { 10.0f, 80.0f };
//  gc.fill_quad(_VDNFT::fquad(tl, { tl.x + 90, tl.y }, { tl.x + 60, tl.y + 50 }, { tl.x + 30, tl.y + 50 }));
}


