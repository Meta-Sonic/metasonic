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
#include "mts/graphics/context.h"
#include "mts/graphics/geometry.h"

MTS_BEGIN_NAMESPACE
class geometry_component : public component::base<geometry_component> {
public:
  template <typename T, typename... Args>
  inline T* add(Args&&... args) {
    return _geometries.add<T>(std::forward<Args>(args)...);
  }

  inline void erase(const geometry* obj) { _geometries.erase(obj); }
  inline void erase(std::size_t index) { _geometries.erase(index); }

  inline geometry* get(std::size_t index) { return _geometries[index]; }
  inline const geometry* get(std::size_t index) const { return _geometries[index]; }

  inline void draw(context& gc) { _geometries.draw(gc); }
  inline void draw(context& gc, const rect<float>& dirty_rect) { _geometries.draw(gc, dirty_rect); }
  inline void draw(context& gc, const memory_range<const rect<float>>& dirty_rects) {
    _geometries.draw(gc, dirty_rects);
  }

private:
  geometry_buffer _geometries;
};
MTS_END_NAMESPACE
