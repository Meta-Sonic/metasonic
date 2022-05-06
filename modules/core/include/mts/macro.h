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

/// @def __MTS_CONCAT1
#define __MTS_CONCAT1(_X, _Y) _X##_Y

/// @def MTS_CONCAT
#define MTS_CONCAT(_X, _Y) __MTS_CONCAT1(_X, _Y)

/// @def MTS_STRINGIFY
#define MTS_STRINGIFY(X) MTS_STR(X)

/// @def MTS_STR
#define MTS_STR(X) #X

/// @def MTS_DEFER
#define MTS_DEFER(M, ...) M(__VA_ARGS__)

#define MTS_DETAIL_EXPAND(x) x

#define MTS_FIRST_(a, ...) a
#define MTS_SECOND_(a, b, ...) b

#define MTS_FIRST(...) MTS_FIRST_(__VA_ARGS__, )
#define MTS_SECOND(...) MTS_SECOND_(__VA_ARGS__, )

#define MTS_IS_PROBE(...) MTS_SECOND(__VA_ARGS__, 0)
#define MTS_PROBE() ~, 1

#define MTS_CAT(a, b) a##b

#define MTS_NOT(x) MTS_IS_PROBE(MTS_CAT(MTS__NOT_, x))
#define MTS__NOT_0 PROBE()

#define MTS_BOOL(x) MTS_NOT(MTS_NOT(x))

#define MTS_HAS_ARGS(...) MTS_BOOL(MTS_FIRST(MTS__END_OF_ARGUMENTS_ __VA_ARGS__)())
#define MTS__END_OF_ARGUMENTS_() 0

#define MTS_VA_REST_(A, ...) __VA_ARGS__
#define MTS_VA_REST(...) MTS_VA_REST_(__VA_ARGS__)

#define MTS_VA_LIST(...) __VA_ARGS__

// Accept any number of args >= N, but expand to just the Nth one.
#define MTS_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,     \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, \
    _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, \
    _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, \
    _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, N, ...)                                                          \
  N

// Get the number of VAARGS.
#define MTS_SIZEOF_VAARGS(...)                                                                                         \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83,   \
      82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55,  \
      54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27,  \
      26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

// Define some macros to help us create overrides based on the
// arity of a for-each-style macro.
#define MTS_DETAIL_FECALL_0(call)
#define MTS_DETAIL_FECALL_1(call, f) call(f)
#define MTS_DETAIL_FECALL_2(call, f, ...) call(f) MTS_DETAIL_FECALL_1(call, __VA_ARGS__)
#define MTS_DETAIL_FECALL_3(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_2(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_4(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_3(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_5(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_4(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_6(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_5(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_7(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_6(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_8(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_7(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_9(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_8(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_10(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_9(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_11(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_10(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_12(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_11(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_13(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_12(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_14(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_13(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_15(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_14(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_16(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_15(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_17(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_16(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_18(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_17(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_19(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_18(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_20(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_19(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_21(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_20(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_22(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_21(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_23(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_22(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_24(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_23(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_25(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_24(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_26(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_25(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_27(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_26(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_28(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_27(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_29(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_28(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_30(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_29(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_31(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_30(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_32(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_31(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_33(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_32(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_34(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_33(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_35(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_34(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_36(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_35(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_37(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_36(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_38(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_37(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_39(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_38(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_40(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_39(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_41(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_40(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_42(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_41(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_43(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_42(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_44(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_43(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_45(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_44(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_46(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_45(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_47(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_46(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_48(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_47(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_49(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_48(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_50(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_49(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_51(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_50(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_52(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_51(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_53(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_52(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_54(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_53(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_55(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_54(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_56(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_55(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_57(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_56(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_58(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_57(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_59(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_58(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_60(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_59(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_61(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_60(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_62(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_61(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_63(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_62(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_64(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_63(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_65(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_64(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_66(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_65(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_67(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_66(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_68(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_67(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_69(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_68(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_70(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_69(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_71(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_70(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_72(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_71(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_73(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_72(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_74(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_73(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_75(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_74(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_76(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_75(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_77(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_76(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_78(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_77(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_79(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_78(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_80(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_79(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_81(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_80(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_82(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_81(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_83(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_82(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_84(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_83(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_85(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_84(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_86(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_85(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_87(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_86(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_88(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_87(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_89(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_88(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_90(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_89(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_91(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_90(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_92(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_91(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_93(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_92(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_94(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_93(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_95(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_94(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_96(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_95(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_97(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_96(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_98(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_97(call, __VA_ARGS__))
#define MTS_DETAIL_FECALL_99(call, f, ...) call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALL_98(call, __VA_ARGS__))

/**
 * Provide a for-each construct for variadic macros. Supports up
 * to 99 args.
 *
 * Example usage1:
 *     #define FWD_DECLARE_CLASS(cls) class cls;
 *     CALL_MACRO_X_FOR_EACH(FWD_DECLARE_CLASS, Foo, Bar)
 *
 * Example usage 2:
 *     #define START_NS(ns) namespace ns {
 *     #define END_NS(ns) }
 *     #define MY_NAMESPACES System, Net, Http
 *     CALL_MACRO_X_FOR_EACH(START_NS, MY_NAMESPACES)
 *     typedef foo int;
 *     CALL_MACRO_X_FOR_EACH(END_NS, MY_NAMESPACES)
 */
#define MTS_FOR_EACH(macro, ...)                                                                                       \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, MTS_DETAIL_FECALL_99, MTS_DETAIL_FECALL_98, MTS_DETAIL_FECALL_97,     \
      MTS_DETAIL_FECALL_96, MTS_DETAIL_FECALL_95, MTS_DETAIL_FECALL_94, MTS_DETAIL_FECALL_93, MTS_DETAIL_FECALL_92,    \
      MTS_DETAIL_FECALL_91, MTS_DETAIL_FECALL_90, MTS_DETAIL_FECALL_89, MTS_DETAIL_FECALL_88, MTS_DETAIL_FECALL_87,    \
      MTS_DETAIL_FECALL_86, MTS_DETAIL_FECALL_85, MTS_DETAIL_FECALL_84, MTS_DETAIL_FECALL_83, MTS_DETAIL_FECALL_82,    \
      MTS_DETAIL_FECALL_81, MTS_DETAIL_FECALL_80, MTS_DETAIL_FECALL_79, MTS_DETAIL_FECALL_78, MTS_DETAIL_FECALL_77,    \
      MTS_DETAIL_FECALL_76, MTS_DETAIL_FECALL_75, MTS_DETAIL_FECALL_74, MTS_DETAIL_FECALL_73, MTS_DETAIL_FECALL_72,    \
      MTS_DETAIL_FECALL_71, MTS_DETAIL_FECALL_70, MTS_DETAIL_FECALL_69, MTS_DETAIL_FECALL_68, MTS_DETAIL_FECALL_67,    \
      MTS_DETAIL_FECALL_66, MTS_DETAIL_FECALL_65, MTS_DETAIL_FECALL_64, MTS_DETAIL_FECALL_63, MTS_DETAIL_FECALL_62,    \
      MTS_DETAIL_FECALL_61, MTS_DETAIL_FECALL_60, MTS_DETAIL_FECALL_59, MTS_DETAIL_FECALL_58, MTS_DETAIL_FECALL_57,    \
      MTS_DETAIL_FECALL_56, MTS_DETAIL_FECALL_55, MTS_DETAIL_FECALL_54, MTS_DETAIL_FECALL_53, MTS_DETAIL_FECALL_52,    \
      MTS_DETAIL_FECALL_51, MTS_DETAIL_FECALL_50, MTS_DETAIL_FECALL_49, MTS_DETAIL_FECALL_48, MTS_DETAIL_FECALL_47,    \
      MTS_DETAIL_FECALL_46, MTS_DETAIL_FECALL_45, MTS_DETAIL_FECALL_44, MTS_DETAIL_FECALL_43, MTS_DETAIL_FECALL_42,    \
      MTS_DETAIL_FECALL_41, MTS_DETAIL_FECALL_40, MTS_DETAIL_FECALL_39, MTS_DETAIL_FECALL_38, MTS_DETAIL_FECALL_37,    \
      MTS_DETAIL_FECALL_36, MTS_DETAIL_FECALL_35, MTS_DETAIL_FECALL_34, MTS_DETAIL_FECALL_33, MTS_DETAIL_FECALL_32,    \
      MTS_DETAIL_FECALL_31, MTS_DETAIL_FECALL_30, MTS_DETAIL_FECALL_29, MTS_DETAIL_FECALL_28, MTS_DETAIL_FECALL_27,    \
      MTS_DETAIL_FECALL_26, MTS_DETAIL_FECALL_25, MTS_DETAIL_FECALL_24, MTS_DETAIL_FECALL_23, MTS_DETAIL_FECALL_22,    \
      MTS_DETAIL_FECALL_21, MTS_DETAIL_FECALL_20, MTS_DETAIL_FECALL_19, MTS_DETAIL_FECALL_18, MTS_DETAIL_FECALL_17,    \
      MTS_DETAIL_FECALL_16, MTS_DETAIL_FECALL_15, MTS_DETAIL_FECALL_14, MTS_DETAIL_FECALL_13, MTS_DETAIL_FECALL_12,    \
      MTS_DETAIL_FECALL_11, MTS_DETAIL_FECALL_10, MTS_DETAIL_FECALL_9, MTS_DETAIL_FECALL_8, MTS_DETAIL_FECALL_7,       \
      MTS_DETAIL_FECALL_6, MTS_DETAIL_FECALL_5, MTS_DETAIL_FECALL_4, MTS_DETAIL_FECALL_3, MTS_DETAIL_FECALL_2,         \
      MTS_DETAIL_FECALL_1, MTS_DETAIL_FECALL_0)(macro, __VA_ARGS__))

// Helper macros for MTS_FOR_EACH_VA
#define MTS_DETAIL_FECALLVA_0(call)
#define MTS_DETAIL_FECALLVA_1(call, args, f) call(f, args)
#define MTS_DETAIL_FECALLVA_2(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_FECALLVA_1(call, MTS_VA_LIST(args), __VA_ARGS__)
#define MTS_DETAIL_FECALLVA_3(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_2(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_4(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_3(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_5(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_4(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_6(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_5(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_7(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_6(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_8(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_7(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_9(call, args, f, ...)                                                                      \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_8(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_10(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_9(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_11(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_10(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_12(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_11(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_13(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_12(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_14(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_13(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_15(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_14(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_16(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_15(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_17(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_16(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_18(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_17(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_19(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_18(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_20(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_19(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_21(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_20(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_22(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_21(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_23(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_22(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_24(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_23(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_25(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_24(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_26(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_25(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_27(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_26(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_28(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_27(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_29(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_28(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_30(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_29(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_31(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_30(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_32(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_31(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_33(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_32(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_34(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_33(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_35(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_34(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_36(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_35(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_37(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_36(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_38(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_37(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_39(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_38(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_40(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_39(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_41(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_40(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_42(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_41(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_43(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_42(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_44(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_43(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_45(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_44(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_46(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_45(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_47(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_46(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_48(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_47(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_49(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_48(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_50(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_49(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_51(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_50(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_52(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_51(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_53(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_52(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_54(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_53(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_55(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_54(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_56(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_55(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_57(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_56(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_58(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_57(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_59(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_58(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_60(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_59(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_61(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_60(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_62(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_61(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_63(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_62(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_64(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_63(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_65(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_64(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_66(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_65(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_67(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_66(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_68(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_67(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_69(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_68(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_70(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_69(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_71(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_70(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_72(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_71(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_73(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_72(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_74(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_73(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_75(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_74(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_76(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_75(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_77(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_76(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_78(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_77(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_79(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_78(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_80(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_79(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_81(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_80(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_82(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_81(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_83(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_82(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_84(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_83(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_85(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_84(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_86(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_85(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_87(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_86(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_88(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_87(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_89(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_88(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_90(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_89(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_91(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_90(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_92(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_91(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_93(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_92(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_94(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_93(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_95(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_94(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_96(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_95(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_97(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_96(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_98(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_97(call, MTS_VA_LIST(args), __VA_ARGS__))
#define MTS_DETAIL_FECALLVA_99(call, args, f, ...)                                                                     \
  call(f, args) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLVA_98(call, MTS_VA_LIST(args), __VA_ARGS__))

// Calls the provided macro on each parameter of the variadic macro.
// Also provides the original __VA_ARGS__ to the macro "lambda".
// ex.
// #define my_macro(val, ...)
// MTS_FOR_EACH_VA(my_macro, ...)
#define MTS_FOR_EACH_VA(macro, ...)                                                                                    \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, MTS_DETAIL_FECALLVA_99, MTS_DETAIL_FECALLVA_98,                       \
      MTS_DETAIL_FECALLVA_97, MTS_DETAIL_FECALLVA_96, MTS_DETAIL_FECALLVA_95, MTS_DETAIL_FECALLVA_94,                  \
      MTS_DETAIL_FECALLVA_93, MTS_DETAIL_FECALLVA_92, MTS_DETAIL_FECALLVA_91, MTS_DETAIL_FECALLVA_90,                  \
      MTS_DETAIL_FECALLVA_89, MTS_DETAIL_FECALLVA_88, MTS_DETAIL_FECALLVA_87, MTS_DETAIL_FECALLVA_86,                  \
      MTS_DETAIL_FECALLVA_85, MTS_DETAIL_FECALLVA_84, MTS_DETAIL_FECALLVA_83, MTS_DETAIL_FECALLVA_82,                  \
      MTS_DETAIL_FECALLVA_81, MTS_DETAIL_FECALLVA_80, MTS_DETAIL_FECALLVA_79, MTS_DETAIL_FECALLVA_78,                  \
      MTS_DETAIL_FECALLVA_77, MTS_DETAIL_FECALLVA_76, MTS_DETAIL_FECALLVA_75, MTS_DETAIL_FECALLVA_74,                  \
      MTS_DETAIL_FECALLVA_73, MTS_DETAIL_FECALLVA_72, MTS_DETAIL_FECALLVA_71, MTS_DETAIL_FECALLVA_70,                  \
      MTS_DETAIL_FECALLVA_69, MTS_DETAIL_FECALLVA_68, MTS_DETAIL_FECALLVA_67, MTS_DETAIL_FECALLVA_66,                  \
      MTS_DETAIL_FECALLVA_65, MTS_DETAIL_FECALLVA_64, MTS_DETAIL_FECALLVA_63, MTS_DETAIL_FECALLVA_62,                  \
      MTS_DETAIL_FECALLVA_61, MTS_DETAIL_FECALLVA_60, MTS_DETAIL_FECALLVA_59, MTS_DETAIL_FECALLVA_58,                  \
      MTS_DETAIL_FECALLVA_57, MTS_DETAIL_FECALLVA_56, MTS_DETAIL_FECALLVA_55, MTS_DETAIL_FECALLVA_54,                  \
      MTS_DETAIL_FECALLVA_53, MTS_DETAIL_FECALLVA_52, MTS_DETAIL_FECALLVA_51, MTS_DETAIL_FECALLVA_50,                  \
      MTS_DETAIL_FECALLVA_49, MTS_DETAIL_FECALLVA_48, MTS_DETAIL_FECALLVA_47, MTS_DETAIL_FECALLVA_46,                  \
      MTS_DETAIL_FECALLVA_45, MTS_DETAIL_FECALLVA_44, MTS_DETAIL_FECALLVA_43, MTS_DETAIL_FECALLVA_42,                  \
      MTS_DETAIL_FECALLVA_41, MTS_DETAIL_FECALLVA_40, MTS_DETAIL_FECALLVA_39, MTS_DETAIL_FECALLVA_38,                  \
      MTS_DETAIL_FECALLVA_37, MTS_DETAIL_FECALLVA_36, MTS_DETAIL_FECALLVA_35, MTS_DETAIL_FECALLVA_34,                  \
      MTS_DETAIL_FECALLVA_33, MTS_DETAIL_FECALLVA_32, MTS_DETAIL_FECALLVA_31, MTS_DETAIL_FECALLVA_30,                  \
      MTS_DETAIL_FECALLVA_29, MTS_DETAIL_FECALLVA_28, MTS_DETAIL_FECALLVA_27, MTS_DETAIL_FECALLVA_26,                  \
      MTS_DETAIL_FECALLVA_25, MTS_DETAIL_FECALLVA_24, MTS_DETAIL_FECALLVA_23, MTS_DETAIL_FECALLVA_22,                  \
      MTS_DETAIL_FECALLVA_21, MTS_DETAIL_FECALLVA_20, MTS_DETAIL_FECALLVA_19, MTS_DETAIL_FECALLVA_18,                  \
      MTS_DETAIL_FECALLVA_17, MTS_DETAIL_FECALLVA_16, MTS_DETAIL_FECALLVA_15, MTS_DETAIL_FECALLVA_14,                  \
      MTS_DETAIL_FECALLVA_13, MTS_DETAIL_FECALLVA_12, MTS_DETAIL_FECALLVA_11, MTS_DETAIL_FECALLVA_10,                  \
      MTS_DETAIL_FECALLVA_9, MTS_DETAIL_FECALLVA_8, MTS_DETAIL_FECALLVA_7, MTS_DETAIL_FECALLVA_6,                      \
      MTS_DETAIL_FECALLVA_5, MTS_DETAIL_FECALLVA_4, MTS_DETAIL_FECALLVA_3, MTS_DETAIL_FECALLVA_2,                      \
      MTS_DETAIL_FECALLVA_1, MTS_DETAIL_FECALLVA_0)(macro, MTS_VA_LIST(__VA_ARGS__), __VA_ARGS__))

// Calls the provided macro on each parameter of the variadic macro.
// Passes on macro_args to your macro lambda.
// ex.
// #define my_macro(some, args, x)
// MTS_FOR_EACH_PARAM(my_macro, MTS_VA_LIST(1, 2), 1, 2);
#define MTS_FOR_EACH_PARAM(macro, macro_args, ...)                                                                     \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, MTS_DETAIL_FECALLVA_99, MTS_DETAIL_FECALLVA_98,                       \
      MTS_DETAIL_FECALLVA_97, MTS_DETAIL_FECALLVA_96, MTS_DETAIL_FECALLVA_95, MTS_DETAIL_FECALLVA_94,                  \
      MTS_DETAIL_FECALLVA_93, MTS_DETAIL_FECALLVA_92, MTS_DETAIL_FECALLVA_91, MTS_DETAIL_FECALLVA_90,                  \
      MTS_DETAIL_FECALLVA_89, MTS_DETAIL_FECALLVA_88, MTS_DETAIL_FECALLVA_87, MTS_DETAIL_FECALLVA_86,                  \
      MTS_DETAIL_FECALLVA_85, MTS_DETAIL_FECALLVA_84, MTS_DETAIL_FECALLVA_83, MTS_DETAIL_FECALLVA_82,                  \
      MTS_DETAIL_FECALLVA_81, MTS_DETAIL_FECALLVA_80, MTS_DETAIL_FECALLVA_79, MTS_DETAIL_FECALLVA_78,                  \
      MTS_DETAIL_FECALLVA_77, MTS_DETAIL_FECALLVA_76, MTS_DETAIL_FECALLVA_75, MTS_DETAIL_FECALLVA_74,                  \
      MTS_DETAIL_FECALLVA_73, MTS_DETAIL_FECALLVA_72, MTS_DETAIL_FECALLVA_71, MTS_DETAIL_FECALLVA_70,                  \
      MTS_DETAIL_FECALLVA_69, MTS_DETAIL_FECALLVA_68, MTS_DETAIL_FECALLVA_67, MTS_DETAIL_FECALLVA_66,                  \
      MTS_DETAIL_FECALLVA_65, MTS_DETAIL_FECALLVA_64, MTS_DETAIL_FECALLVA_63, MTS_DETAIL_FECALLVA_62,                  \
      MTS_DETAIL_FECALLVA_61, MTS_DETAIL_FECALLVA_60, MTS_DETAIL_FECALLVA_59, MTS_DETAIL_FECALLVA_58,                  \
      MTS_DETAIL_FECALLVA_57, MTS_DETAIL_FECALLVA_56, MTS_DETAIL_FECALLVA_55, MTS_DETAIL_FECALLVA_54,                  \
      MTS_DETAIL_FECALLVA_53, MTS_DETAIL_FECALLVA_52, MTS_DETAIL_FECALLVA_51, MTS_DETAIL_FECALLVA_50,                  \
      MTS_DETAIL_FECALLVA_49, MTS_DETAIL_FECALLVA_48, MTS_DETAIL_FECALLVA_47, MTS_DETAIL_FECALLVA_46,                  \
      MTS_DETAIL_FECALLVA_45, MTS_DETAIL_FECALLVA_44, MTS_DETAIL_FECALLVA_43, MTS_DETAIL_FECALLVA_42,                  \
      MTS_DETAIL_FECALLVA_41, MTS_DETAIL_FECALLVA_40, MTS_DETAIL_FECALLVA_39, MTS_DETAIL_FECALLVA_38,                  \
      MTS_DETAIL_FECALLVA_37, MTS_DETAIL_FECALLVA_36, MTS_DETAIL_FECALLVA_35, MTS_DETAIL_FECALLVA_34,                  \
      MTS_DETAIL_FECALLVA_33, MTS_DETAIL_FECALLVA_32, MTS_DETAIL_FECALLVA_31, MTS_DETAIL_FECALLVA_30,                  \
      MTS_DETAIL_FECALLVA_29, MTS_DETAIL_FECALLVA_28, MTS_DETAIL_FECALLVA_27, MTS_DETAIL_FECALLVA_26,                  \
      MTS_DETAIL_FECALLVA_25, MTS_DETAIL_FECALLVA_24, MTS_DETAIL_FECALLVA_23, MTS_DETAIL_FECALLVA_22,                  \
      MTS_DETAIL_FECALLVA_21, MTS_DETAIL_FECALLVA_20, MTS_DETAIL_FECALLVA_19, MTS_DETAIL_FECALLVA_18,                  \
      MTS_DETAIL_FECALLVA_17, MTS_DETAIL_FECALLVA_16, MTS_DETAIL_FECALLVA_15, MTS_DETAIL_FECALLVA_14,                  \
      MTS_DETAIL_FECALLVA_13, MTS_DETAIL_FECALLVA_12, MTS_DETAIL_FECALLVA_11, MTS_DETAIL_FECALLVA_10,                  \
      MTS_DETAIL_FECALLVA_9, MTS_DETAIL_FECALLVA_8, MTS_DETAIL_FECALLVA_7, MTS_DETAIL_FECALLVA_6,                      \
      MTS_DETAIL_FECALLVA_5, MTS_DETAIL_FECALLVA_4, MTS_DETAIL_FECALLVA_3, MTS_DETAIL_FECALLVA_2,                      \
      MTS_DETAIL_FECALLVA_1, MTS_DETAIL_FECALLVA_0)(macro, MTS_VA_LIST(macro_args), __VA_ARGS__))

#define MTS_DETAIL_FECALLLAST_0(call)
#define MTS_DETAIL_FECALLLAST_1(call, last_call, f) last_call(f)
#define MTS_DETAIL_FECALLLAST_2(call, last_call, f, ...) call(f) MTS_DETAIL_FECALLLAST_1(call, last_call, __VA_ARGS__)
#define MTS_DETAIL_FECALLLAST_3(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_2(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_4(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_3(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_5(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_4(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_6(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_5(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_7(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_6(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_8(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_7(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_9(call, last_call, f, ...)                                                               \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_8(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_10(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_9(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_11(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_10(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_12(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_11(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_13(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_12(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_14(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_13(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_15(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_14(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_16(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_15(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_17(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_16(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_18(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_17(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_19(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_18(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_20(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_19(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_21(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_20(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_22(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_21(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_23(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_22(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_24(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_23(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_25(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_24(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_26(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_25(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_27(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_26(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_28(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_27(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_29(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_28(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_30(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_29(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_31(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_30(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_32(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_31(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_33(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_32(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_34(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_33(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_35(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_34(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_36(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_35(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_37(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_36(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_38(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_37(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_39(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_38(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_40(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_39(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_41(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_40(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_42(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_41(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_43(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_42(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_44(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_43(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_45(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_44(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_46(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_45(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_47(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_46(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_48(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_47(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_49(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_48(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_50(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_49(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_51(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_50(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_52(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_51(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_53(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_52(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_54(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_53(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_55(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_54(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_56(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_55(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_57(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_56(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_58(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_57(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_59(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_58(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_60(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_59(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_61(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_60(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_62(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_61(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_63(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_62(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_64(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_63(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_65(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_64(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_66(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_65(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_67(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_66(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_68(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_67(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_69(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_68(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_70(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_69(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_71(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_70(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_72(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_71(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_73(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_72(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_74(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_73(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_75(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_74(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_76(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_75(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_77(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_76(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_78(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_77(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_79(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_78(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_80(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_79(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_81(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_80(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_82(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_81(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_83(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_82(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_84(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_83(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_85(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_84(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_86(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_85(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_87(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_86(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_88(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_87(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_89(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_88(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_90(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_89(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_91(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_90(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_92(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_91(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_93(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_92(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_94(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_93(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_95(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_94(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_96(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_95(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_97(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_96(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_98(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_97(call, last_call, __VA_ARGS__))
#define MTS_DETAIL_FECALLLAST_99(call, last_call, f, ...)                                                              \
  call(f) MTS_DETAIL_EXPAND(MTS_DETAIL_FECALLLAST_98(call, last_call, __VA_ARGS__))

// Calls your provided macro on each variadic parameter.
// Uses a second macro for the last element.
// ex.
// #define add_comma(x) x,
// #define last(x) x
// MTS_FOR_EACH_LAST(add_comma, last, ...)
#define MTS_FOR_EACH_LAST(macro, last_macro, ...)                                                                      \
  MTS_DETAIL_EXPAND(MTS_GET_NTH_ARG(__VA_ARGS__, MTS_DETAIL_FECALLLAST_99, MTS_DETAIL_FECALLLAST_98,                   \
      MTS_DETAIL_FECALLLAST_97, MTS_DETAIL_FECALLLAST_96, MTS_DETAIL_FECALLLAST_95, MTS_DETAIL_FECALLLAST_94,          \
      MTS_DETAIL_FECALLLAST_93, MTS_DETAIL_FECALLLAST_92, MTS_DETAIL_FECALLLAST_91, MTS_DETAIL_FECALLLAST_90,          \
      MTS_DETAIL_FECALLLAST_89, MTS_DETAIL_FECALLLAST_88, MTS_DETAIL_FECALLLAST_87, MTS_DETAIL_FECALLLAST_86,          \
      MTS_DETAIL_FECALLLAST_85, MTS_DETAIL_FECALLLAST_84, MTS_DETAIL_FECALLLAST_83, MTS_DETAIL_FECALLLAST_82,          \
      MTS_DETAIL_FECALLLAST_81, MTS_DETAIL_FECALLLAST_80, MTS_DETAIL_FECALLLAST_79, MTS_DETAIL_FECALLLAST_78,          \
      MTS_DETAIL_FECALLLAST_77, MTS_DETAIL_FECALLLAST_76, MTS_DETAIL_FECALLLAST_75, MTS_DETAIL_FECALLLAST_74,          \
      MTS_DETAIL_FECALLLAST_73, MTS_DETAIL_FECALLLAST_72, MTS_DETAIL_FECALLLAST_71, MTS_DETAIL_FECALLLAST_70,          \
      MTS_DETAIL_FECALLLAST_69, MTS_DETAIL_FECALLLAST_68, MTS_DETAIL_FECALLLAST_67, MTS_DETAIL_FECALLLAST_66,          \
      MTS_DETAIL_FECALLLAST_65, MTS_DETAIL_FECALLLAST_64, MTS_DETAIL_FECALLLAST_63, MTS_DETAIL_FECALLLAST_62,          \
      MTS_DETAIL_FECALLLAST_61, MTS_DETAIL_FECALLLAST_60, MTS_DETAIL_FECALLLAST_59, MTS_DETAIL_FECALLLAST_58,          \
      MTS_DETAIL_FECALLLAST_57, MTS_DETAIL_FECALLLAST_56, MTS_DETAIL_FECALLLAST_55, MTS_DETAIL_FECALLLAST_54,          \
      MTS_DETAIL_FECALLLAST_53, MTS_DETAIL_FECALLLAST_52, MTS_DETAIL_FECALLLAST_51, MTS_DETAIL_FECALLLAST_50,          \
      MTS_DETAIL_FECALLLAST_49, MTS_DETAIL_FECALLLAST_48, MTS_DETAIL_FECALLLAST_47, MTS_DETAIL_FECALLLAST_46,          \
      MTS_DETAIL_FECALLLAST_45, MTS_DETAIL_FECALLLAST_44, MTS_DETAIL_FECALLLAST_43, MTS_DETAIL_FECALLLAST_42,          \
      MTS_DETAIL_FECALLLAST_41, MTS_DETAIL_FECALLLAST_40, MTS_DETAIL_FECALLLAST_39, MTS_DETAIL_FECALLLAST_38,          \
      MTS_DETAIL_FECALLLAST_37, MTS_DETAIL_FECALLLAST_36, MTS_DETAIL_FECALLLAST_35, MTS_DETAIL_FECALLLAST_34,          \
      MTS_DETAIL_FECALLLAST_33, MTS_DETAIL_FECALLLAST_32, MTS_DETAIL_FECALLLAST_31, MTS_DETAIL_FECALLLAST_30,          \
      MTS_DETAIL_FECALLLAST_29, MTS_DETAIL_FECALLLAST_28, MTS_DETAIL_FECALLLAST_27, MTS_DETAIL_FECALLLAST_26,          \
      MTS_DETAIL_FECALLLAST_25, MTS_DETAIL_FECALLLAST_24, MTS_DETAIL_FECALLLAST_23, MTS_DETAIL_FECALLLAST_22,          \
      MTS_DETAIL_FECALLLAST_21, MTS_DETAIL_FECALLLAST_20, MTS_DETAIL_FECALLLAST_19, MTS_DETAIL_FECALLLAST_18,          \
      MTS_DETAIL_FECALLLAST_17, MTS_DETAIL_FECALLLAST_16, MTS_DETAIL_FECALLLAST_15, MTS_DETAIL_FECALLLAST_14,          \
      MTS_DETAIL_FECALLLAST_13, MTS_DETAIL_FECALLLAST_12, MTS_DETAIL_FECALLLAST_11, MTS_DETAIL_FECALLLAST_10,          \
      MTS_DETAIL_FECALLLAST_9, MTS_DETAIL_FECALLLAST_8, MTS_DETAIL_FECALLLAST_7, MTS_DETAIL_FECALLLAST_6,              \
      MTS_DETAIL_FECALLLAST_5, MTS_DETAIL_FECALLLAST_4, MTS_DETAIL_FECALLLAST_3, MTS_DETAIL_FECALLLAST_2,              \
      MTS_DETAIL_FECALLLAST_1, MTS_DETAIL_FECALLLAST_0)(macro, last_macro, __VA_ARGS__))
