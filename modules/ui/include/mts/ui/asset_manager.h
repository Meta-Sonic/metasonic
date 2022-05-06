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
#include "mts/print.h"
#include "mts/shared_singleton.h"
#include "mts/filesystem.h"
#include "mts/graphics/color.h"
#include "mts/graphics/padding.h"
#include "mts/graphics/rect.h"
#include "mts/graphics/font.h"
#include "mts/graphics/image.h"

#include "mts/ui/style.h"
#include "mts/ui/embedded_assets.h"

#include <map>
#include <initializer_list>
#include <string>

MTS_BEGIN_NAMESPACE

#define MTS_REPORT_ASSETS(...) mts::print(__VA_ARGS__)

/// Font map.
class font_map {
public:
  inline font_map() { add("default_font", _VMTS::font(embedded_assets::fonts::roboto_medium, 12.0f)); }

  inline _VMTS::font get(const std::string& name) const {
    auto it = _map.find(name);
    if (it != _map.end()) {
      return it->second;
    }

    _VMTS::warnprint<_VMTS::empty_string>("font_map - Could not find font : ", name);
    return _VMTS::font();
  }

  inline bool has(const std::string& name) const { return _map.find(name) != _map.end(); }

  inline _VMTS::font operator[](const std::string& name) const { return get(name); }

  inline void add(const std::string& name, const _VMTS::font& font) {
    MTS_REPORT_ASSETS("add_font", name, font.get_height(), font.get_font_size());
    _map.emplace(name, font);
  }

  inline void add(const std::string& name, const _VMTS::filesystem::path& font_path, float height) {
    MTS_REPORT_ASSETS("add_font", name, font_path);
    _map.emplace(name, _VMTS::font(font_path, height));
  }

  inline void add(
      const _VMTS::filesystem::path& font_path, std::initializer_list<std::pair<std::string, float>> font_info) {
    _VMTS::font f(font_path, 12.0f);

    for (const auto& n : font_info) {
      MTS_REPORT_ASSETS("add_font", n.first, n.second);
      _map.emplace(n.first, f.make_copy(n.second));
    }
  }

private:
  std::map<std::string, _VMTS::font> _map;
};

/// Image map.
class image_map {
public:
  inline _VMTS::image get(const std::string& name) const {
    auto it = _map.find(name);
    if (it != _map.end()) {
      return it->second;
    }

    _VMTS::warnprint<_VMTS::empty_string>("image_map - Could not find image : ", name);
    return _VMTS::image();
  }

  inline bool has(const std::string& name) const { return _map.find(name) != _map.end(); }

  inline _VMTS::image operator[](const std::string& name) const { return get(name); }

  inline void add(const std::string& name, const _VMTS::image& img) {
    MTS_REPORT_ASSETS("add_image", name);
    _map.emplace(name, img);
  }

  inline void add(const std::string& name, const _VMTS::filesystem::path& path, float scale = 1.0f) {
    MTS_REPORT_ASSETS("add_image", name, path);
    _map.emplace(name, _VMTS::image(path, scale));
  }

private:
  std::map<std::string, _VMTS::image> _map;
};

/// Style map.
class style_map {
public:
  style_map() { add("default", std::make_shared<const mts::style>()); }

  /// TODO: Should be const ?
  inline std::shared_ptr<const mts::style> get(const std::string& name) {
    auto it = _map.find(name);
    if (it != _map.end()) {
      return it->second;
    }

    _VMTS::warnprint<_VMTS::empty_string>("style_map - Could not find style : ", name);
    return nullptr;
  }

  inline std::shared_ptr<const mts::style> operator[](const std::string& name) { return get(name); }

  inline void add(const std::string& name, std::shared_ptr<const mts::style> style) {
    MTS_REPORT_ASSETS("add_style", name);
    _map.emplace(name, style);
  }

private:
  std::map<std::string, std::shared_ptr<const mts::style>> _map;
};

class asset_manager {
public:
  font_map fonts;
  image_map images;
  style_map styles;

  ~asset_manager() { mts::print("ASSET MANAGER DESTRUCTOR"); }
};

using assets = _VMTS::shared_singleton<asset_manager>;
inline std::shared_ptr<asset_manager> get_assets() { return assets::get(); }

MTS_END_NAMESPACE
