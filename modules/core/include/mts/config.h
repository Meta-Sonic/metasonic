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

#define __MTS_VERSION_MAJOR__ 1
#define __MTS_VERSION_MINOR__ 0
#define __MTS_VERSION_PATCH__ 0

#ifndef __MTS_ABI_VERSION__
  #define __MTS_ABI_VERSION__ 1
#endif // __MTS_ABI_VERSION__.

#define MTS_NAMESPACE mts

#include "mts/detail/config.h"
#include "mts/detail/platform_macro.h"

// Build type.
#if __MTS_DEBUG_BUILD__
  #define MTS_DEBUG
#elif __MTS_RELEASE_BUILD__
  #define MTS_RELEASE
#else
  #warning Unknown build type
#endif

// C++ version.
#if __MTS_CPP_17__
  #define MTS_CPP_17
#elif __MTS_CPP_20__
  #define MTS_CPP_20
#else
  #warning Unknown cpp version
#endif

// Bitness.
#if __MTS_32_BIT__
  #define MTS_32
#elif __MTS_64_BIT__
  #define MTS_64
#else
  #error "mts only support 32 and 64 bit architecture."
#endif

// Compiler.
#if __MTS_CLANG__
  #define MTS_CLANG
#elif __MTS_GCC__
  #define MTS_GCC
#elif __MTS_INTEL__
  #define MTS_INTEL
#elif __MTS_MINGW__
  #define MTS_MINGW
#elif __MTS_MSVC__
  #define MTS_MSVC
#elif __MTS_WASM__
  #define MTS_WASM
#else
  #warning Unknown compiler
#endif

// Platform.
#if __MTS_ANDROID__
  #define MTS_ANDROID
#elif __MTS_BSD__
  #define MTS_BSD
#elif __MTS_IOS__
  #define MTS_IOS
#elif __MTS_LINUX__
  #define MTS_LINUX
#elif __MTS_MACOS__
  #define MTS_MACOS
#elif __MTS_SOLARIS__
  #define MTS_SOLARIS
#elif __MTS_WINDOWS__
  #define MTS_WINDOWS
#else
  #warning Unknown compiler
#endif

// Architecture.
#if __MTS_ARCH_X86__
  #define MTS_X86
#elif __MTS_ARCH_X86_64__
  #define MTS_X86_64
#elif __MTS_ARCH_ARM_32__
  #define MTS_ARM_32
#elif __MTS_ARCH_ARM_64__
  #define MTS_ARM_64
#else
  #warning Unknown architecture
#endif

// Unistd.
#if __MTS_UNISTD__
  #define MTS_UNISTD
#endif

// Exceptions.
// __MTS_HAS_EXCEPTIONS__
#if __MTS_HAS_EXCEPTIONS__
  #define MTS_EXCEPTIONS
#endif

/// @def MTS_WARNING
#define MTS_WARNING(X) __MTS_WARNING(X)

/// @def MTS_STATIC_WARNING
#define MTS_STATIC_WARNING(msg, ...) __MTS_STATIC_WARNING(msg, __VA_ARGS__)

/// @def MTS_HAS_CPP_ATTRIBUTE
/// Only use __has_cpp_attribute in C++ mode. GCC defines __has_cpp_attribute in
/// C mode, but the :: in __has_cpp_attribute(scoped::attribute) is invalid.
#define MTS_HAS_CPP_ATTRIBUTE(X) __MTS_HAS_CPP_ATTRIBUTE(X)

/// @def MTS_ALWAYS_INLINE
/// On compilers where we have a directive to do so, mark a method "always inline"
/// because it is performance sensitive. GCC 3.4 supported this but is buggy in
/// various cases and produces unimplemented errors, just use it in GCC 4.0 and later.
#define MTS_ALWAYS_INLINE __MTS_ALWAYS_INLINE

/// @def MTS_LIKELY
#define MTS_LIKELY(EXPR) __MTS_LIKELY(EXPR)

/// @defMTS_UNLIKELY
#define MTS_UNLIKELY(EXPR) __MTS_UNLIKELY(EXPR)

/// @def MTS_ATTRIBUTE_UNUSED
/// Some compilers warn about unused functions. When a function is sometimes
/// used or not depending on build settings (e.g. a function only called from
/// within "assert"), this attribute can be used to suppress such warnings.
///
/// However, it shouldn't be used for unused *variables*, as those have a much
/// more portable solution:
///   (void)unused_var_name;
/// Prefer cast-to-void wherever it is sufficient.
#define MTS_ATTRIBUTE_UNUSED __MTS_ATTRIBUTE_UNUSED

/// @def MTS_NODISCARD
/// Warn if a type or return value is discarded.
#define MTS_NODISCARD __MTS_NODISCARD

/// @macro MTS_ATTRIBUTE_RETURNS_NONNULL.
#define MTS_ATTRIBUTE_RETURNS_NONNULL __MTS_ATTRIBUTE_RETURNS_NONNULL

/// @macro MTS_ATTRIBUTE_NO_RETURN.
#define MTS_ATTRIBUTE_NO_RETURN __MTS_ATTRIBUTE_NO_RETURN

/// @def MTS_ATTRIBUTE_HIDDEN.
#define MTS_ATTRIBUTE_HIDDEN __MTS_ATTRIBUTE_HIDDEN

/// @def MTS_PACKED
/// Used to specify a packed structure.
/// MTS_PACKED(
///   struct A {
///      int i;
///      int j;
///      int k;
///      long long l;
///   });
#define MTS_PACKED(X) __MTS_PACKED(X)

/// @def MTS_PACKED_START
/// @def MTS_PACKED_END
/// @code
///   MTS_PACKED_START
///   struct B {
///     int i;
///     int j;
///     int k;
///     long long l;
///   };
///   MTS_PACKED_END
/// @endcode
#define MTS_PACKED_START __MTS_PACKED_START
#define MTS_PACKED_END __MTS_PACKED_END

#define MTS_PACKED_N_START(N) __MTS_PACKED_N_START(N)

/// @macro MTS_PRETTY_FUNCTION.
#define MTS_PRETTY_FUNCTION __MTS_PRETTY_FUNCTION

/// @def MTS_BUILTIN_TRAP
/// On compilers which support it, expands to an expression which causes the program to exit abnormally.
#define MTS_BUILTIN_TRAP() __MTS_BUILTIN_TRAP()

/// @def MTS_DEBUGTRAP
/// On compilers which support it, expands to an expression which causes the program to break
/// while running under a debugger.
#define MTS_DEBUGTRAP() __MTS_DEBUGTRAP()

/// @def MTS_NOP().
#define MTS_NOP() __MTS_NOP()
