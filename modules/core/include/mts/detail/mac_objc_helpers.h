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
#include "mts/detail/mac_cf_helpers.h"

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
//
//
//

template <typename>
struct MetaSuperFn {
  static constexpr auto value = objc_msgSendSuper;
};


template <typename SuperType, typename ReturnType, typename... Params>
inline ReturnType ObjCMsgSendSuper(id self, SEL sel, Params... params) {
  using SuperFn = ReturnType (*)(struct objc_super*, SEL, Params...);
  const auto fn = reinterpret_cast<SuperFn>(MetaSuperFn<ReturnType>::value);

  objc_super s = { self, [SuperType class] };
  return fn(&s, sel, params...);
}

struct NSObjectDeleter {
  void operator()(NSObject* object) const noexcept {
    if (object != nullptr)
      [object release];
  }
};

template <typename NSType>
using NSUniquePtr = std::unique_ptr<NSType, NSObjectDeleter>;

template <typename Type>
inline Type getIvar(id self, const char* name) {
  void* v = nullptr;
  object_getInstanceVariable(self, name, &v);
  return static_cast<Type>(v);
}

struct random_initializer {
  random_initializer() { std::srand((unsigned)std::time(NULL) * getpid()); }
};

inline std::string gen_random_string(const int len) {
  static random_initializer __random_initializer;

  static const char alphanum[] = "0123456789"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[std::rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}

template <typename SuperclassType>
struct ObjCClass {
  ObjCClass(const char* root_name)
      : cls(objc_allocateClassPair([SuperclassType class], std::string(root_name + gen_random_string(10)).c_str(), 0)) {
  }

  ObjCClass(const ObjCClass&) = delete;

  ~ObjCClass() {
    std::string kvoSubclassName = std::string("NSKVONotifying_") + class_getName(cls);

    if (objc_getClass(kvoSubclassName.c_str()) == nullptr) {
      objc_disposeClassPair(cls);
    }
    //        if (objc_getClass(kvoSubclassName.toUTF8()) == nullptr) {
    //            objc_disposeClassPair (cls);
    //        }
  }

  ObjCClass& operator=(const ObjCClass&) = delete;

  void registerClass() { objc_registerClassPair(cls); }

  SuperclassType* createInstance() const { return class_createInstance(cls, 0); }

  template <typename Type>
  void addIvar(const char* name) {
    BOOL b = class_addIvar(cls, name, sizeof(Type), (uint8_t)rint(log2(sizeof(Type))), @encode(Type));
    mts_assert(b, "");
//    _VDNFT::unused(b);
  }

  template <typename FunctionType>
  void addMethod(SEL selector, FunctionType callbackFn, const char* signature) {
    BOOL b = class_addMethod(cls, selector, (IMP)callbackFn, signature);
    mts_assert(b, "");
//    _VDNFT::unused(b);
  }

  template <typename FunctionType>
  void addMethod(SEL selector, FunctionType callbackFn, const char* sig1, const char* sig2) {
    addMethod(selector, callbackFn, (std::string(sig1) + sig2).c_str());
  }

  template <typename FunctionType>
  void addMethod(SEL selector, FunctionType callbackFn, const char* sig1, const char* sig2, const char* sig3) {
    addMethod(selector, callbackFn, (std::string(sig1) + sig2 + sig3).c_str());
  }

  template <typename FunctionType>
  void addMethod(
      SEL selector, FunctionType callbackFn, const char* sig1, const char* sig2, const char* sig3, const char* sig4) {
    addMethod(selector, callbackFn, (std::string(sig1) + sig2 + sig3 + sig4).c_str());
  }

  void addProtocol(Protocol* protocol) {
    BOOL b = class_addProtocol(cls, protocol);
    mts_assert(b, "");
//    _VDNFT::unused(b);
  }

  template <typename ReturnType, typename... Params>
  static ReturnType sendSuperclassMessage(id self, SEL sel, Params... params) {
    return ObjCMsgSendSuper<SuperclassType, ReturnType, Params...>(self, sel, params...);
  }

  Class cls;

private:
  //    static String getRandomisedName (const char* root)
  //    {
  //        return root + String::toHexString (juce::Random::getSystemRandom().nextInt64());
  //    }

  //    JUCE_DECLARE_NON_COPYABLE (ObjCClass)
};

template <class _ClassName>
struct ObjCLifetimeManagedClass : public ObjCClass<NSObject> {
  ObjCLifetimeManagedClass()
      : ObjCClass<NSObject>("ObjCLifetimeManagedClass_") {
    addIvar<_ClassName*>("cppObject");
    addMethod(@selector(initWithJuceObject:), initWithJuceObject, "@@:@");
    addMethod(@selector(dealloc), dealloc, "v@:");

    registerClass();
  }

  static id initWithJuceObject(id _self, SEL, _ClassName* obj) {
    NSObject* self = sendSuperclassMessage<NSObject*>(_self, @selector(init));
    object_setInstanceVariable(self, "cppObject", obj);

    return self;
  }

  static void dealloc(id _self, SEL) {
    if (auto* obj = getIvar<_ClassName*>(_self, "cppObject")) {
      delete obj;
      object_setInstanceVariable(_self, "cppObject", nullptr);
    }

    sendSuperclassMessage<void>(_self, @selector(dealloc));
  }

  static ObjCLifetimeManagedClass objCLifetimeManagedClass;
};

template <typename Class>
ObjCLifetimeManagedClass<Class> ObjCLifetimeManagedClass<Class>::objCLifetimeManagedClass;

// this will return an NSObject which takes ownership of the JUCE instance passed-in
// This is useful to tie the life-time of a juce instance to the life-time of an NSObject
template <typename Class>
inline NSObject* createNSObjectFromJuceClass(Class* obj) {
  //    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wobjc-method-access")
  return [ObjCLifetimeManagedClass<Class>::objCLifetimeManagedClass.createInstance() initWithJuceObject:obj];
  //    JUCE_END_IGNORE_WARNINGS_GCC_LIKE
}

// Get the JUCE class instance that was tied to the life-time of an NSObject with the
// function above
template <typename Class>
inline Class* getJuceClassFromNSObject(NSObject* obj) {
  return obj != nullptr ? getIvar<Class*>(obj, "cppObject") : nullptr;
}

template <typename ReturnT, class Class, typename... Params>
inline ReturnT (^CreateObjCBlock(Class* object, ReturnT (Class::*fn)(Params...)))(Params...) {
  __block Class* _this = object;
  __block ReturnT (Class::*_fn)(Params...) = fn;

  return [[^ReturnT(Params... params) {
      return (_this->*_fn)(params...);
  } copy] autorelease];
}

template <typename BlockType>
class ObjCBlock {
public:
  ObjCBlock() { block = nullptr; }
  template <typename R, class C, typename... P>
  ObjCBlock(C* _this, R (C::*fn)(P...))
      : block(CreateObjCBlock(_this, fn)) {}
  ObjCBlock(BlockType b)
      : block([b copy]) {}
  ObjCBlock& operator=(const BlockType& other) {
    if (block != nullptr) {
      [block release];
    }
    block = [other copy];
    return *this;
  }
  bool operator==(const void* ptr) const { return ((const void*)block == ptr); }
  bool operator!=(const void* ptr) const { return ((const void*)block != ptr); }
  ~ObjCBlock() {
    if (block != nullptr)
      [block release];
  }

  operator BlockType() { return block; }

private:
  BlockType block;
};
MTS_END_NAMESPACE
