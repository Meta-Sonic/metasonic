#include "mts/ui/controls/popup_menu.h"
#include "mts/ui/native_view.h"
#include <unordered_set>

MTS_BEGIN_NAMESPACE

//
// MARK: Popup menu content.
//

class popup_menu_content : public _VMTS::view {
public:
  popup_menu_content() { _check_mark_path = _VMTS::path::create_check_mark(); }

  virtual ~popup_menu_content() override = default;

  inline const _VMTS::path& get_checkmark() const { return _check_mark_path; }

  virtual void bounds_changed(const _VMTS::view::bounds_status& bs) override {
    if (bs.size_changed()) {
      _bg_path = _VMTS::path();
      float border_radius = 5;
      float bw = 1;

      _bg_path.add_rounded_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }), border_radius);

      repaint();
    }
  }

  virtual void paint(_VMTS::context& gc, const _VMTS::rect<float>& dirty_rect) override {
    _VMTS::unused(dirty_rect);

    gc.set_fill_color(0xDBDBDBFF);
    //    gc.fill_rect(dirty_rect);
    gc.fill_path(_bg_path);

    gc.set_stroke_color(0xBBBBBBFF);
    gc.set_line_width(0.8f);
    gc.stroke_path(_bg_path);
  }

  void on_click(const popup_menu::item& item) {
    if (item.action) {
      item.action();
      close_window();
      return;
    }

    if (item.id == _VMTS::popup_menu::item::invalid_id) {
      close_window();
      return;
    }

    if (_callback) {
      _callback(item.id);
    }

    //    mts::print("PRE CLOSE WINDOW");
    close_window();
    //    mts::print("POST CLOSE WINDOW");
  }

  inline void set_window(_VMTS::weak_managed_ptr<_VMTS::popup_window> w) { _window = w; }

  inline void set_callback(std::function<void(_VMTS::popup_menu::item::uid)> callback) { _callback = callback; }

private:
  _VMTS::weak_managed_ptr<popup_window> _window;
  _VMTS::path _bg_path;

  inline void close_window() {
    if (_window.is_valid()) {
      _window->close();
    }
  }

  _VMTS::path _check_mark_path;
  std::function<void(_VMTS::popup_menu::item::uid)> _callback;
};

//
// MARK: Popup menu item.
//

class popup_menu_item : public _VMTS::view {
public:
  popup_menu_item(popup_menu_content& content, const popup_menu::item& item)
      : _content(content)
      , _item(item) {
    _font = _VMTS::get_assets()->fonts.get("roboto-regular-12");

    set_no_mouse_drag(true);
  }

  virtual ~popup_menu_item() override = default;

  virtual void mouse_left_down(const _VMTS::mouse_event& evt) override {
    _VMTS::unused(evt);
    _did_mouse_down = true;
  }

  virtual void mouse_left_up(const _VMTS::mouse_event& evt) override {
    if (!_item.enabled) {
      return;
    }
    _VMTS::unused(evt);

    _VMTS::view* root_parent = get_root_parent();
    mts_assert(root_parent, "popup_menu_item doesn't have any root parent.");

    _VMTS::native_view* nv = root_parent->get_native_view();
    mts_assert(nv, "popup_menu_item doesn't have any native view.");

    if (!_did_mouse_down && !nv->dragged_after_creation()) {
      return;
    }

    _content.on_click(_item);
    _did_mouse_down = false;
    mts::print("ON POST CONTENT CLICK");
  }

  virtual void mouse_enter(const _VMTS::mouse_event& evt) override {
    _VMTS::unused(evt);

    if (_VMTS::assign(_is_highlighted, true)) {
      repaint();
    }
  }

  virtual void mouse_leave() override {
    if (_VMTS::assign(_is_highlighted, false)) {
      repaint();
    }
  }

  virtual bool key_pressed(const _VMTS::key_event& kevt) override {
    if (kevt.is_return()) {
      _content.on_click(_item);
      return true;
    }
    return false;
  }

  virtual void bounds_changed(const _VMTS::view::bounds_status& bs) override {
    if (bs.size_changed()) {
      _bg_path = _VMTS::path();
      float border_radius = 5;
      float bw = 0;

      _bg_path.add_rounded_rect(get_local_fbounds().reduced({ bw * 0.5f, bw * 0.5f }), border_radius);

      repaint();
    }
  }

  virtual void paint(_VMTS::context& gc, const _VMTS::rect<float>& dirty_rect) override {
    _VMTS::unused(dirty_rect);
    _VMTS::color bg_color = _is_highlighted ? 0x4C98FDFF : 0xDBDBDBFF;
    _VMTS::color txt_color = _is_highlighted ? 0xFFFFFFFF : 0x212222FF;

    if (!_item.enabled) {
      bg_color = 0xDBDBDBFF;
      txt_color = 0x21222255;
    }
    //    if (_is_highlighted) {
    //      gc.set_fill_color(0x4C98FDFF);
    //      //      gc.fill_rect(dirty_rect);
    //      gc.fill_path(_bg_path);
    //      gc.set_fill_color(0xFFFFFFFF);
    //    }
    //    else {
    //      gc.set_fill_color(0xDBDBDBFF);
    //      //      gc.fill_rect(dirty_rect);
    //      gc.fill_path(_bg_path);
    //      gc.set_fill_color(0x212222FF);
    //    }

    gc.set_fill_color(bg_color);
    gc.fill_path(_bg_path);
    gc.set_fill_color(txt_color);

    if (_item.checked) {
      const _VMTS::path& checkmark = _content.get_checkmark();
      _VMTS::point<float> middle = get_local_fbounds().middle();
      gc.fill_path(checkmark, { 5.0f, middle.y - 4.0f, 8.0f, 8.0f });
    }

    gc.draw_text_left_aligned(_font, _item.text, get_local_bounds().reduced({ 20, 4 }));
    //    gc.draw_text_centered(_font, _item.text, get_local_bounds() - _VMTS::point{ 0, 1 });

    // Border.
    //    gc.set_line_width(get_style_value<bstyle::border_width>());
    //    gc.set_stroke_color(get_style_color<bstyle::border_color>());

    //    gc.stroke_path(_bg_path);
  }

private:
  popup_menu_content& _content;
  popup_menu::item _item;
  _VMTS::font _font;
  bool _is_highlighted = false;
  bool _did_mouse_down = false;
  _VMTS::path _bg_path;
};

//
// MARK: Popup menu.
//

popup_menu::popup_menu() {}

popup_menu::~popup_menu() {}

popup_menu::item::uid popup_menu::add_item(
    const std::string& name, std::function<void()> action, bool checked, bool enabled) {
  popup_menu::item::uid id = _it_counter++;
  item it;
  it.text = name;
  it.action = action;
  it.checked = checked;
  it.enabled = enabled;
  it.id = id;
  _items.push_back(std::move(it));
  return id;
}

popup_menu::item::uid popup_menu::add_item(const std::string& name) {
  popup_menu::item::uid id = _it_counter++;
  item it;
  it.text = name;
  it.id = id;
  _items.push_back(std::move(it));
  return id;
}

void popup_menu::set_selected_item_id(item::uid item_id) {
  for (auto& item : _items) {
    item.checked = (item_id == item.id);
  }
}

void popup_menu::set_selected_item_index(std::size_t item_index) {
  if (item_index >= _items.size()) {
    return;
  }

  unselect_all();

  _items[item_index].checked = true;
}

void popup_menu::unselect_all() {
  for (auto& item : _items) {
    item.checked = false;
  }
}

void popup_menu::show_menu(_VMTS::view& parent, const _VMTS::point<int>& position, const _VMTS::size<int>& item_size) {
  int padding = 4;
  _VMTS::managed_ptr<popup_menu_content> content = _VMTS::make_managed<popup_menu_content>();

  popup_menu_content* cv = content.get();

  for (std::size_t i = 0; i < _items.size(); i++) {
    content->add(_VMTS::make_managed<popup_menu_item>(*content, _items[i]),
        _VMTS::rect({ padding, padding + (int)(i * item_size.height) }, item_size));
  }

  content->set_size(_VMTS::size<int>(2 * padding + item_size.width, 2 * padding + item_size.height * _items.size()));

  int delta_item = 0;
  _VMTS::point pos = position - _VMTS::point(0, delta_item * item_size.height);

  _VMTS::managed_ptr<_VMTS::popup_window> win = _VMTS::popup_window::create(parent, std::move(content), pos);
  cv->set_window(win);
  cv->set_callback(_callback);
  win->set_close_on_left_up_outside(true);
  win->set_close_on_escape_key(true);
  win->set_close_on_return_key(true);
}

MTS_END_NAMESPACE
