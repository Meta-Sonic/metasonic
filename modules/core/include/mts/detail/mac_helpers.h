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

template <typename CFType>
using CFUniquePtr = std::unique_ptr<typename std::remove_pointer<CFType>::type, CFObjectDeleter<CFType>>;

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
