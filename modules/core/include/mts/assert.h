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
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

#if __MTS_RELEASE_BUILD__
  #define mts_assert(Expr, Msg) ;
  #define mts_noexcept_assert(Expr, Msg) ;
  #define mts_error(Msg) ;
  #define mts_cexpr_assert(CHECK) void(0)

#else
  #define mts_assert(Expr, Msg) _VMTS::assert_detail::custom_assert(#Expr, Expr, __FILE__, __LINE__, Msg)
  #define mts_noexcept_assert(Expr, Msg) _VMTS::assert_detail::custom_assert(#Expr, Expr, __FILE__, __LINE__, Msg)
  #define mts_error(Msg) _VMTS::assert_detail::custom_error(__FILE__, __LINE__, Msg)
  #define mts_cexpr_assert(CHECK) (MTS_LIKELY(CHECK) ? void(0) : [] { assert(!#CHECK); }())
#endif // __MTS_RELEASE_BUILD__.

MTS_BEGIN_NAMESPACE
namespace assert_detail {
#if __MTS_DEBUG_BUILD__
inline void custom_assert(const char* expr_str, bool expr, const char* file, int line, const std::string& msg) {
  if (MTS_LIKELY(expr)) {
    return;
  }

  std::cerr << "Assert failed:\t" << msg << "\n"
            << "Expected:\t" << expr_str << "\n"
            << "Source:\t\t" << file << ", line " << line << "\n";

  MTS_DEBUGTRAP();
}

inline void custom_error(const char* file, int line, const std::string& msg) {
  std::cerr << "Assert failed:\t" << msg << "\n"
            << "Source:\t\t" << file << ", line " << line << "\n";

  MTS_DEBUGTRAP();
}
#endif // __MTS_DEBUG_BUILD__
} // namespace assert_detail.
MTS_END_NAMESPACE
