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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

#include "mts/macro.h"

//
// MARK: Namespace.
//

#define __MTS_ABI_NAMESPACE__ MTS_CONCAT(__, __MTS_ABI_VERSION__)

// Inline namespaces are available in Clang/GCC/MSVC regardless of C++ dialect.
#define MTS_BEGIN_NAMESPACE                                                                                            \
  namespace MTS_NAMESPACE {                                                                                            \
  inline namespace __MTS_ABI_NAMESPACE__ {

#define MTS_END_NAMESPACE                                                                                              \
  }                                                                                                                    \
  }

#define _VMTS MTS_NAMESPACE::__MTS_ABI_NAMESPACE__

#define MTS_BEGIN_SUB_NAMESPACE(X) MTS_BEGIN_NAMESPACE namespace X {
#define MTS_END_SUB_NAMESPACE(X)                                                                                       \
  MTS_END_NAMESPACE                                                                                                    \
  }

//
// MARK: Config.
//
#ifndef __has_include
  #define __has_include(x) 0
#endif

#ifndef __has_feature
  #define __has_feature(x) 0
#endif

#ifndef __has_extension
  #define __has_extension(x) 0
#endif

#ifndef __has_attribute
  #define __has_attribute(x) 0
#endif

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

//
// MARK: Build type.
//
#undef __MTS_DEBUG_BUILD__
#undef __MTS_RELEASE_BUILD__

#ifdef NDEBUG
  // Release.
  #define __MTS_RELEASE_BUILD__ 1
  #define __MTS_DEBUG_BUILD__ 0
#else
  // Debug.
  #define __MTS_RELEASE_BUILD__ 0
  #define __MTS_DEBUG_BUILD__ 1
#endif

//
// MARK: C++ version.
//
#undef __MTS_CPP_20__

// C++ 20.
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
  #define __MTS_CPP_20__ 1

#else
  #define __MTS_CPP_20__ 0
  #error "mts only support c++20 and up."
#endif

//
// Bitness type.
//
#undef __MTS_32_BIT__
#undef __MTS_64_BIT__

// 32 bit.
#if INTPTR_MAX == INT32_MAX
  #define __MTS_32_BIT__ 1
  #define __MTS_64_BIT__ 0

// 64 bit.
#elif INTPTR_MAX == INT64_MAX
  #define __MTS_32_BIT__ 0
  #define __MTS_64_BIT__ 1

// Unknown.
#else
  #define __MTS_32_BIT__ 0
  #define __MTS_64_BIT__ 0
  #error "mts only support 32 and 64 bit architecture."
#endif

//
// MARK: Compiler type.
//
#undef __MTS_CLANG__
#undef __MTS_GCC__
#undef __MTS_INTEL__
#undef __MTS_MINGW__
#undef __MTS_MSVC__
#undef __MTS_WASM__

// Clang.
#if defined(__clang__)
  #define __MTS_CLANG__ 1
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 0

// GCC.
#elif defined(__GNUC__) || defined(__GNUG__)
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 1
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 0

// Intel.
#elif (defined(SYCL_LANGUAGE_VERSION) && defined(__INTEL_LLVM_COMPILER)) || defined(__INTEL_COMPILER)
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 1
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 0

// MinGW.
#elif defined(__MINGW32__) || defined(__MINGW64__)
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 1
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 0

// Microsoft visual studio.
#elif defined(_MSC_VER)
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 1
  #define __MTS_WASM__ 0

// Web assembly.
#elif defined(__EMSCRIPTEN__)
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 1

// Unknown compiler.
#else
  #define __MTS_CLANG__ 0
  #define __MTS_GCC__ 0
  #define __MTS_INTEL__ 0
  #define __MTS_MINGW__ 0
  #define __MTS_MSVC__ 0
  #define __MTS_WASM__ 0

  #error "mts unsupported compiler."
#endif

//
// MARK: Platform type.
//
#undef __MTS_ANDROID__
#undef __MTS_BSD__
#undef __MTS_IOS__
#undef __MTS_LINUX__
#undef __MTS_MACOS__
#undef __MTS_SOLARIS__
#undef __MTS_WINDOWS__

// Android.
#if defined(__ANDROID__)
  #define __MTS_ANDROID__ 1
  #define __MTS_BSD__ 0
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 0
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 0
  #define __MTS_WINDOWS__ 0

// Linux.
#elif defined(__linux__) || defined(__linux) || defined(linux)
  #define __MTS_ANDROID__ 0
  #define __MTS_BSD__ 0
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 1
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 0
  #define __MTS_WINDOWS__ 0

// Apple macos or ios.
#elif defined(__APPLE__)
  // Apple.
  #include <TargetConditionals.h>
  #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    // ios.
    #define __MTS_ANDROID__ 0
    #define __MTS_BSD__ 0
    #define __MTS_IOS__ 1
    #define __MTS_LINUX__ 0
    #define __MTS_MACOS__ 0
    #define __MTS_SOLARIS__ 0
    #define __MTS_WINDOWS__ 0

  #elif TARGET_OS_MAC
    // Mac OS.
    #define __MTS_ANDROID__ 0
    #define __MTS_BSD__ 0
    #define __MTS_IOS__ 0
    #define __MTS_LINUX__ 0
    #define __MTS_MACOS__ 1
    #define __MTS_SOLARIS__ 0
    #define __MTS_WINDOWS__ 0

  #else
    // Unknown apple platform.
    #define __MTS_ANDROID__ 0
    #define __MTS_BSD__ 0
    #define __MTS_IOS__ 0
    #define __MTS_LINUX__ 0
    #define __MTS_MACOS__ 0
    #define __MTS_SOLARIS__ 0
    #define __MTS_WINDOWS__ 0

    #error "mts unsupported platform."
  #endif

// BSD.
#elif defined(BSD) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  #define __MTS_ANDROID__ 0
  #define __MTS_BSD__ 1
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 0
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 0
  #define __MTS_WINDOWS__ 0

// Solaris.
#elif defined(__sun) && defined(__SVR4)
  #define __MTS_ANDROID__ 0
  #define __MTS_BSD__ 0
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 0
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 1
  #define __MTS_WINDOWS__ 0

// Windows.
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
  #define __MTS_ANDROID__ 0
  #define __MTS_BSD__ 0
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 0
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 0
  #define __MTS_WINDOWS__ 1

// Unknown platform.
#else
  #define __MTS_ANDROID__ 0
  #define __MTS_BSD__ 0
  #define __MTS_IOS__ 0
  #define __MTS_LINUX__ 0
  #define __MTS_MACOS__ 0
  #define __MTS_SOLARIS__ 0
  #define __MTS_WINDOWS__ 0

  #error "mts unsupported platform."
#endif

//
// MARK: Architecture type.
//
#undef __MTS_ARCH_X86__
#undef __MTS_ARCH_X86_64__
#undef __MTS_ARCH_ARM_32__
#undef __MTS_ARCH_ARM_64__

#if defined(__arm64) || defined(__arm64__) || defined(_M_ARM64)
  #define __MTS_ARCH_X86__ 0
  #define __MTS_ARCH_X86_64__ 0
  #define __MTS_ARCH_ARM_32__ 0
  #define __MTS_ARCH_ARM_64__ 1

#elif defined(__arm__) || defined(_M_ARM)
  #define __MTS_ARCH_X86__ 0
  #define __MTS_ARCH_X86_64__ 0
  #define __MTS_ARCH_ARM_32__ 1
  #define __MTS_ARCH_ARM_64__ 0

#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__amd64) || defined(_M_X64)
  #define __MTS_ARCH_X86__ 0
  #define __MTS_ARCH_X86_64__ 1
  #define __MTS_ARCH_ARM_32__ 0
  #define __MTS_ARCH_ARM_64__ 0

#elif defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)                \
    || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__I86__) || defined(__INTEL__)

  #define __MTS_ARCH_X86__ 1
  #define __MTS_ARCH_X86_64__ 0
  #define __MTS_ARCH_ARM_32__ 0
  #define __MTS_ARCH_ARM_64__ 0

#else
  #define __MTS_ARCH_X86__ 0
  #define __MTS_ARCH_X86_64__ 0
  #define __MTS_ARCH_ARM_32__ 0
  #define __MTS_ARCH_ARM_64__ 0

  #error "mts unsupported architecture."
#endif

//
// MARK: unistd.h
//
#undef __MTS_UNISTD__

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
  #define __MTS_UNISTD__ 1
#else
  #define __MTS_UNISTD__ 0
#endif

#undef __MTS_HAS_EXCEPTIONS__
#undef __MTS_EXCEPTIONS_DISABLED__

/// @macro MTS_HAS_EXCEPTIONS
/// Built with exception support.
///
#if !__has_feature(cxx_exceptions)
  #define __MTS_EXCEPTIONS_DISABLED__ 1
#endif

#if !__EXCEPTIONS
  #define __MTS_EXCEPTIONS_DISABLED__ 1
#endif

#if __MTS_EXCEPTIONS_DISABLED__
  #define __MTS_HAS_EXCEPTIONS__ 0
#else
  #define __MTS_HAS_EXCEPTIONS__ 1
#endif

//#if __has_feature(cxx_exceptions) || (defined(__GNUC__) && __EXCEPTIONS)                                      \
//    || (defined(_MSC_VER) && defined(_CPPUNWIND))
//  #define __MTS_HAS_EXCEPTIONS__ 1
//#else
//  #define __MTS_HAS_EXCEPTIONS__ 0
//#endif

#if __MTS_MACOS__ || __MTS_IOS__
  #define __MTS_USE_ACCELERATE__ 1
#endif // __MTS_MACOS__ || __MTS_IOS__

#if __MTS_MSVC__
  #define MTS_PUSH_MACROS __pragma(push_macro("min")) __pragma(push_macro("max"))
  #define MTS_POP_MACROS __pragma(pop_macro("min")) __pragma(pop_macro("max"))
#else
  #define MTS_PUSH_MACROS _Pragma("push_macro(\"min\")") _Pragma("push_macro(\"max\")")
  #define MTS_POP_MACROS _Pragma("pop_macro(\"min\")") _Pragma("pop_macro(\"max\")")
#endif
