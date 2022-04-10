#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

//
// MARK: Warning.
//
#if defined(__GNUC__)
  #define __MTS_WARNING(X) _Pragma(MTS_STR(GCC warning(X " at line " MTS_DEFER(MTS_STR, __LINE__))))
#elif defined(_MSC_VER)
  #define __MTS_WARNING(X) _Pragma(MTS_STR(message(X " at line " MTS_DEFER(MTS_STR, __LINE__))))
#else
  #warning "This compiler is not supported for warning."
#endif

//
// MARK: Static warning.
//
#if defined(__GNUC__)
  #define __MTS_ATTRIBUTE_WARNING(foo, msg) foo __attribute__((diagnose_if(1, msg, "warning")))
#elif defined(_MSC_VER)
  #define __MTS_ATTRIBUTE_WARNING(foo, msg) __declspec(deprecated(msg)) foo
#else
  #warning "This compiler is not supported for attribute warning."
#endif

//
//

#if __MTS_MSVC__
  // TODO: ....
  #define MTS_PRAGMA_(x) __pragma(#x)
  #define MTS_PRAGMA(x) MTS_PRAGMA_(x)
  #define MTS_PRAGMA_PUSH() __pragma(warning(push))
  #define MTS_PRAGMA_POP() __pragma(warning(pop))
  #define MTS_PRAGMA_DIAGNOSTIC(TYPE, VALUE) MTS_PRAGMA(warning(TYPE : VALUE))
#else
  #define MTS_PRAGMA_(x) _Pragma(#x)
  #define MTS_PRAGMA(x) MTS_PRAGMA_(x)
  #define MTS_PRAGMA_PUSH() MTS_PRAGMA(clang diagnostic push)
  #define MTS_PRAGMA_POP() MTS_PRAGMA(clang diagnostic pop)
  #define MTS_PRAGMA_DIAGNOSTIC(TYPE, VALUE) MTS_PRAGMA(clang diagnostic TYPE VALUE)
#endif //

/// @macro __MTS_STATIC_WARNING
///
#if __MTS_MSVC__
  #define __MTS_STATIC_WARNING(msg, ...)                                                                               \
    struct MTS_CONCAT(static_warning, __LINE__) {                                                                      \
      __MTS_ATTRIBUTE_WARNING(static constexpr void _(const std::false_type&), msg) {}                                 \
      static constexpr void _(const std::true_type&) {}                                                                \
      MTS_CONCAT(static_warning, __LINE__)() { _(std::bool_constant<(__VA_ARGS__)>()); }                               \
    }

#else
  // gcc-compat warns when using diagnose_if __attribute__((diagnose_if(...)))
  #define __MTS_STATIC_WARNING(msg, ...)                                                                               \
    struct MTS_CONCAT(static_warning, __LINE__) {                                                                      \
      MTS_PRAGMA_PUSH()                                                                                                \
      MTS_PRAGMA_DIAGNOSTIC(ignored, "-Wgcc-compat")                                                                   \
      __MTS_ATTRIBUTE_WARNING(static constexpr void _(const std::false_type&), msg) {}                                 \
      static constexpr void _(const std::true_type&) {}                                                                \
      MTS_CONCAT(static_warning, __LINE__)() { _(std::bool_constant<(__VA_ARGS__)>()); }                               \
      MTS_PRAGMA_POP()                                                                                                 \
    }
#endif //

//
// MARK: Helper macros.
//

/// @macro MTS_GNUC_PREREQ
/// Extend the default __GNUC_PREREQ even if glibc's features.h isn't available.
#ifndef MTS_GNUC_PREREQ
  #if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
    #define MTS_GNUC_PREREQ(maj, min, patch)                                                                           \
      ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= ((maj) << 20) + ((min) << 10) + (patch))

  #elif defined(__GNUC__) && defined(__GNUC_MINOR__)
    #define MTS_GNUC_PREREQ(maj, min, patch)                                                                           \
      ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))

  #else
    #define MTS_GNUC_PREREQ(maj, min, patch) 0
  #endif
#endif

/// Only use __has_cpp_attribute in C++ mode. GCC defines __has_cpp_attribute in
/// C mode, but the :: in __has_cpp_attribute(scoped::attribute) is invalid.
#ifndef __MTS_HAS_CPP_ATTRIBUTE
  #if defined(__cplusplus) && defined(__has_cpp_attribute)
    #define __MTS_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
  #else
    #define __MTS_HAS_CPP_ATTRIBUTE(x) 0
  #endif
#endif

/// @macro MTS_ALWAYS_INLINE
/// On compilers where we have a directive to do so, mark a method "always inline"
/// because it is performance sensitive. GCC 3.4 supported this but is buggy in
/// various cases and produces unimplemented errors, just use it in GCC 4.0 and later.
#if __has_attribute(always_inline) || MTS_GNUC_PREREQ(4, 0, 0)
  #define __MTS_ALWAYS_INLINE inline __attribute__((always_inline))
#elif __MTS_MSVC__
  #define __MTS_ALWAYS_INLINE __forceinline
#else
  #define __MTS_ALWAYS_INLINE inline
#endif

/// @macro MTS_LIKELY and MTS_UNLIKELY
#if __has_builtin(__builtin_expect) || MTS_GNUC_PREREQ(4, 0, 0)
  #define __MTS_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
  #define __MTS_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
#else
  #define __MTS_LIKELY(EXPR) (EXPR)
  #define __MTS_UNLIKELY(EXPR) (EXPR)
#endif

/// @macro MTS_ATTRIBUTE_UNUSED
/// Some compilers warn about unused functions. When a function is sometimes
/// used or not depending on build settings (e.g. a function only called from
/// within "assert"), this attribute can be used to suppress such warnings.
///
/// However, it shouldn't be used for unused *variables*, as those have a much
/// more portable solution:
///   (void)unused_var_name;
/// Prefer cast-to-void wherever it is sufficient.
#if defined(__cplusplus) && __cplusplus > 201402L && __MTS_HAS_CPP_ATTRIBUTE(maybe_unused)
  #define __MTS_ATTRIBUTE_UNUSED [[maybe_unused]]

#elif __has_attribute(unused) || MTS_GNUC_PREREQ(3, 1, 0)
  #define __MTS_ATTRIBUTE_UNUSED __attribute__((__unused__))

#else
  #define __MTS_ATTRIBUTE_UNUSED
#endif

/// @macro MTS_NODISCARD
/// Warn if a type or return value is discarded.
#if defined(__cplusplus) && __cplusplus > 201402L && __MTS_HAS_CPP_ATTRIBUTE(nodiscard)
  // Use the 'nodiscard' attribute in C++17 or newer mode.
  #define __MTS_NODISCARD [[nodiscard]]

#elif __MTS_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)
  // Clang in C++14 mode claims that it has the 'nodiscard' attribute, but also
  // warns in the pedantic mode that 'nodiscard' is a C++17 extension (PR33518).
  // Use the 'nodiscard' attribute in C++14 mode only with GCC.
  // TODO: remove this workaround when PR33518 is resolved.
  #define __MTS_NODISCARD [[clang::warn_unused_result]]

#elif defined(__GNUC__) && __MTS_HAS_CPP_ATTRIBUTE(nodiscard)
  #define __MTS_NODISCARD [[nodiscard]]

#else
  #define __MTS_NODISCARD
#endif

/// @macro MTS_ATTRIBUTE_RETURNS_NONNULL.
#if __has_attribute(returns_nonnull) || MTS_GNUC_PREREQ(4, 9, 0)
  #define __MTS_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#elif __MTS_MSVC__
  #define __MTS_ATTRIBUTE_RETURNS_NONNULL _Ret_notnull_
#else
  #define __MTS_ATTRIBUTE_RETURNS_NONNULL
#endif

/// @macro MTS_ATTRIBUTE_NO_RETURN.
#if __has_attribute(noreturn) || MTS_GNUC_PREREQ(4, 9, 0)
  #define __MTS_ATTRIBUTE_NO_RETURN __attribute__((noreturn))
#elif __MTS_MSVC__
  #define __MTS_ATTRIBUTE_NO_RETURN __declspec(noreturn)
#else
  #define __MTS_ATTRIBUTE_NO_RETURN
#endif

/// @macro MTS_ATTRIBUTE_HIDDEN.
#if __has_attribute(visibility) || MTS_GNUC_PREREQ(4, 0, 0)
  #define __MTS_ATTRIBUTE_HIDDEN __attribute__((visibility("hidden")))
#else
  #define __MTS_ATTRIBUTE_HIDDEN
#endif

///// @macro MTS_ATTRIBUTE_DEFAULT.
//#if __has_attribute(visibility) || MTS_GNUC_PREREQ(4, 0, 0)
//  #define __MTS_ATTRIBUTE_HIDDEN __attribute__((visibility("hidden")))
//#else
//  #define __MTS_ATTRIBUTE_HIDDEN
//#endif

#if __MTS_MSVC__
  #define MTS_API __declspec(dllexport)
#else
  #define MTS_API __attribute__((visibility("default")))
#endif

//#if __has_attribute(internal_linkage)
//#  define _LIBCPP_INTERNAL_LINKAGE __attribute__ ((internal_linkage))
//#else
//#  define _LIBCPP_INTERNAL_LINKAGE _LIBCPP_ALWAYS_INLINE
//#endif

//#ifndef _LIBCPP_HIDE_FROM_ABI
//#  if _LIBCPP_HIDE_FROM_ABI_PER_TU
//#    define _LIBCPP_HIDE_FROM_ABI _LIBCPP_HIDDEN _LIBCPP_INTERNAL_LINKAGE
//#  else
//#    define _LIBCPP_HIDE_FROM_ABI _LIBCPP_HIDDEN _LIBCPP_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
//#  endif
//#endif

//#if JUCE_MSVC
// #ifdef JUCE_DLL_BUILD
//  #define JUCE_API __declspec (dllexport)
//  #pragma warning (disable: 4251)
// #elif defined (JUCE_DLL)
//  #define JUCE_API __declspec (dllimport)
//  #pragma warning (disable: 4251)
// #endif
// #ifdef __INTEL_COMPILER
//  #pragma warning (disable: 1125) // (virtual override warning)
// #endif
//#elif defined (JUCE_DLL) || defined (JUCE_DLL_BUILD)
// #define JUCE_API __attribute__ ((visibility("default")))
//#endif

/// @macro MTS_PACKED
/// Used to specify a packed structure.
/// MTS_PACKED(
///   struct A {
///      int i;
///      int j;
///      int k;
///      long long l;
///   });
///
/// MTS_PACKED_START
/// struct B {
///   int i;
///   int j;
///   int k;
///   long long l;
/// };
/// MTS_PACKED_END
#if __MTS_MSVC__
  #define __MTS_PACKED(d) __pragma(pack(push, 1)) d __pragma(pack(pop))
  #define __MTS_PACKED_START __pragma(pack(push, 1))
  #define __MTS_PACKED_N_START(N) __pragma(pack(push, N))
  #define __MTS_PACKED_END __pragma(pack(pop))

#else
  #define __MTS_PACKED(d) d __attribute__((packed))
  #define __MTS_PACKED_START _Pragma("pack(push, 1)")
  #define __MTS_PACKED_N_START(N) MTS_PRAGMA(pack(push, N))
  #define __MTS_PACKED_END _Pragma("pack(pop)")
#endif

/// @macro MTS_PRETTY_FUNCTION
/// Gets a user-friendly looking function signature for the current scope
/// using the best available method on each platform.  The exact format of the
/// resulting string is implementation specific and non-portable, so this should
/// only be used, for example, for logging or diagnostics.
#if __MTS_MSVC__
  #define __MTS_PRETTY_FUNCTION __FUNCSIG__
#elif __MTS_GCC__ || __MTS_CLANG__ || __MTS_MINGW__
  #define __MTS_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
  #define __MTS_PRETTY_FUNCTION __func__
#endif

/// @macro MTS_BUILTIN_TRAP
/// On compilers which support it, expands to an expression which causes the program to exit abnormally.
#if __has_builtin(__builtin_trap) || MTS_GNUC_PREREQ(4, 3, 0)
  #define __MTS_BUILTIN_TRAP() __builtin_trap()

#elif defined(_MSC_VER)
  // The __debugbreak intrinsic is supported by MSVC, does not require forward
  // declarations involving platform-specific typedefs (unlike RaiseException),
  // results in a call to vectored exception handlers, and encodes to a short
  // instruction that still causes the trapping behavior we want.
  #define __MTS_BUILTIN_TRAP() __debugbreak()

#else
  #define __MTS_BUILTIN_TRAP() *(volatile int*)0x11 = 0
#endif

/// @macro MTS_DEBUGTRAP
/// On compilers which support it, expands to an expression which causes the program to break
/// while running under a debugger.
#if __has_builtin(__builtin_debugtrap)
  #define __MTS_DEBUGTRAP() __builtin_debugtrap()

#elif __MTS_MSVC__
  // The __debugbreak intrinsic is supported by MSVC and breaks while
  // running under the debugger, and also supports invoking a debugger
  // when the OS is configured appropriately.
  #define __MTS_DEBUGTRAP() __debugbreak()

#elif __MTS_CLANG__ && (defined(unix) || defined(__unix__) || defined(__unix) || defined(__MACH__))
  #include <signal.h>
  #define __MTS_DEBUGTRAP() raise(SIGTRAP)

#else
  #define __MTS_DEBUGTRAP() std::abort()
#endif

/// @macro MTS_NOP().
#if __MTS_INTEL__
  #if __MTS_MSVC__
    #define __MTS_NOP() _mm_pause()
  #else
    #define __MTS_NOP() __builtin_ia32_pause()
  #endif
#elif __MTS_ARM__
  #define __MTS_NOP() asm volatile("yield")
#else
  #define __MTS_NOP() ({ (void)0; })
#endif
