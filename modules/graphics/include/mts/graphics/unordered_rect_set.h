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
#include "mts/graphics/rect.h"
#include <unordered_set>

MTS_BEGIN_NAMESPACE
namespace detail {
template <typename T>
void hash_combine(std::size_t& seed, const T& val) {
  seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T, typename... Types>
void hash_combine(std::size_t& seed, const T& val, const Types&... args) {
  hash_combine(seed, val);
  hash_combine(seed, args...);
}

template <typename... Types>
std::size_t hash_val(const Types&... args) {
  std::size_t seed = 0;
  hash_combine(seed, args...);
  return seed;
}

template <typename T>
struct rect_hash {
  std::size_t operator()(const rect<T>& r) const { return hash_val(r.x, r.y, r.width, r.height); }
};
} // namespace detail.

template <typename _Tp>
class unordered_rect_set : public std::unordered_set<rect<_Tp>, detail::rect_hash<_Tp>> {
public:
  using rect_value_type = _Tp;
  using rect_type = rect<rect_value_type>;
  using hash_type = detail::rect_hash<rect_value_type>;
  using set_type = std::unordered_set<rect_type, hash_type>;
};
MTS_END_NAMESPACE
