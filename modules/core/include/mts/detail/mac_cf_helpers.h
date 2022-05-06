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

#include <CoreFoundation/CoreFoundation.h>
//#import <Cocoa/Cocoa.h>
#include <objc/runtime.h>
#include <objc/objc.h>
#include <objc/message.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <string_view>
#include <unistd.h>

MTS_BEGIN_NAMESPACE

//
// MARK: Core Foundation.
//

template <typename _CFType>
struct cf_object_deleter {
  inline void operator()(_CFType object) const noexcept {
    if (object != nullptr) {
      CFRelease(object);
    }
  }
};

template <class _CFType>
class cf_unique_ptr : public std::unique_ptr<typename std::remove_pointer<_CFType>::type, cf_object_deleter<_CFType>> {
public:
  using base_type = std::unique_ptr<typename std::remove_pointer<_CFType>::type, cf_object_deleter<_CFType>>;
  using element_type = typename base_type::element_type;
  using deleter_type = typename base_type::deleter_type;
  using pointer = typename base_type::pointer;

  using base_type::base_type;

  inline cf_unique_ptr(pointer p) noexcept
      : base_type(p) {}

  inline operator pointer() noexcept { return base_type::get(); }

  inline operator const pointer() const noexcept { return base_type::get(); }
};

template <std::size_t N>
CFDictionaryRef create_cf_dictionary(CFStringRef const (&keys)[N], CFTypeRef const (&values)[N]) {
  return CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys, (const void**)&values, N,
      &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

inline CFStringRef create_cf_string_from_string_view(std::string_view str) {
  return CFStringCreateWithBytes(
      kCFAllocatorDefault, (const UInt8*)str.data(), (CFIndex)str.size(), kCFStringEncodingUTF8, (Boolean) false);
}
// inline CFStringRef create_cf_string_from_string_view(std::string_view str) {
//   return CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)str.data(), (CFIndex)str.size(),
//       kCFStringEncodingUTF8, (Boolean) false, kCFAllocatorNull);
// }

inline CFStringRef create_cf_string_from_string(const std::string& str) {
  return create_cf_string_from_string_view(std::string_view(str));
}

inline CFStringRef create_cf_string_from_u32string_view(std::u32string_view str) {
  return CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)str.data(),
      (CFIndex)(str.size() * sizeof(std::u32string::value_type)), kCFStringEncodingUTF32LE, (Boolean) false);
}

inline CFStringRef create_cf_string_from_u32string(const std::u32string& str) {
  //
  //  return CFStringCreateWithCharactersNoCopy(kCFAllocatorDefault, (const UniChar *)str.data(), (CFIndex)str.size(),
  //  kCFAllocatorNull);
  return CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)str.data(),
      (CFIndex)(str.size() * sizeof(std::u32string::value_type)), kCFStringEncodingUTF32LE, (Boolean) false);

  //  return CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8*)str.data(), (CFIndex)str.size(),
  //                                 kCFStringEncodingUTF8, (Boolean) false);
}

template <typename CFType>
struct CFObjectDeleter {
  void operator()(CFType object) const noexcept {
    if (object != nullptr)
      CFRelease(object);
  }
};

// template <typename CFType>
// using CFUniquePtr = std::unique_ptr<typename std::remove_pointer<CFType>::type, CFObjectDeleter<CFType>>;

template <typename CFType>
struct CFObjectHolder {
  CFObjectHolder() = default;

  CFObjectHolder(const CFObjectHolder&) = delete;
  CFObjectHolder(CFObjectHolder&&) = delete;

  CFObjectHolder& operator=(const CFObjectHolder&) = delete;
  CFObjectHolder& operator=(CFObjectHolder&&) = delete;

  ~CFObjectHolder() noexcept {
    if (object != nullptr)
      CFRelease(object);
  }

  // Public to facilitate passing the pointer address to functions
  CFType object = nullptr;
};

MTS_END_NAMESPACE
