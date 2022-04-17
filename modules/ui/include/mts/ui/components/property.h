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
#include "mts/component.h"
#include "mts/ui/property.h"
#include <unordered_map>

MTS_BEGIN_NAMESPACE
class property_component : public component::base<property_component> {
public:
  inline bool empty() const { return _properties.empty(); }

  inline bool contains(const std::string& name) const { return _properties.contains(name); }

  //    inline void add(const std::string& name, const property& p) { _properties.emplace(name, p); }

  inline void set(const std::string& name, const property& p) { _properties[name] = p; }

  inline bool erase(const std::string& name) {
    auto it = _properties.find(name);
    if (it == _properties.end()) {
      return false;
    }

    _properties.erase(it);
    return true;
  }

  inline const property& get(const std::string& name) const {
    static property __p;
    if (auto it = _properties.find(name); it != _properties.end()) {
      return it->second;
    }

    return __p;
  }

  inline property& operator[](const std::string& name) { return _properties[name]; }

private:
  std::unordered_map<std::string, property> _properties;
};
MTS_END_NAMESPACE

//  //
//  // MARK: Properties.
//  //
//
//  class properties {
//  public:
//    properties() = delete;
//    properties(const properties&) = delete;
//    properties(properties&&) = delete;
//    ~properties() = default;
//    properties& operator=(const properties&) = delete;
//    properties& operator=(properties&&) = delete;
//
//    inline bool empty() const { return !(bool)_properties; }
//    inline void add(const std::string& name, const property& p) { _view.add_property(name, p); }
//    inline void set(const std::string& name, const property& p) { _view.set_property(name, p); }
//    inline property get(const std::string& name) const { return _view.get_property(name); }
//
//  private:
//    friend class view;
//    properties(view& v)
//        : _view(v) {}
//
//    using property_map = std::unordered_map<std::string, property>;
//
//    view& _view;
//    std::unique_ptr<property_map> _properties;
//    inline property_map& get_ref() { return _VMTS::get_ref(_properties); }
//  } properties;
