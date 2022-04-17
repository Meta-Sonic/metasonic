///
/// Copyright (C) 2021, Meta-Sonic
/// All rights reserved.
///
/// Proprietary and confidential.
/// Any unauthorized copying, alteration, distribution, transmission,
/// performance, display or other use of this material is strictly prohibited.
///
/// Written by Alexandre Arsenault <alx.arsenault@gmail.com>
///

#pragma once
#include "mts/ui/view.h"
#include "mts/ui/popup_window.h"
#include <functional>
#include <limits>
#include <vector>

MTS_BEGIN_NAMESPACE

class popup_menu {
public:
  struct item {
    using uid = std::size_t;
    static constexpr uid invalid_id = std::numeric_limits<uid>::max();

    std::string text;
    std::function<void()> action;
    uid id = invalid_id;
    bool checked = false;
    bool enabled = true;
  };

  popup_menu();
  ~popup_menu();

  item::uid add_item(const std::string& name, std::function<void()> action, bool checked = false, bool enabled = true);
  item::uid add_item(const std::string& name);

  void set_selected_item_id(item::uid item_id);
  void set_selected_item_index(std::size_t item_index);
  void unselect_all();

  inline void set_callback(std::function<void(item::uid)> callback) { _callback = callback; }

  inline const item* get_item(item::uid item_id) const {
    if (item_id == item::invalid_id) {
      return nullptr;
    }

    for (const auto& it : _items) {
      if (item_id == it.id) {
        return &it;
      }
    }
    return nullptr;
  }

  inline const item* get_item_from_index(std::size_t item_index) const {
    if (item_index >= _items.size()) {
      return nullptr;
    }
    return &_items[item_index];
  }

  //  inline std::vector<item>& get_items() { return _items; }
  inline const std::vector<item>& get_items() const { return _items; }

  void show_menu(_VMTS::view& parent, const _VMTS::point<int>& position);

private:
  std::vector<item> _items;
  std::function<void(item::uid)> _callback;
  _VMTS::weak_managed_ptr<popup_window> _window;
  item::uid _it_counter = 0;
};

MTS_END_NAMESPACE
