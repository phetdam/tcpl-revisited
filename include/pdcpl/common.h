/**
 * @file common.h
 * @author Derek Huang
 * @brief C/C++ header for common macros
 * @copyright MIT License
 */

#ifndef PDCPL_COMMON_H_
#define PDCPL_COMMON_H_

// extern "C" scoping macros
#ifdef __cplusplus
#define PDCPL_EXTERN_C_BEGIN extern "C" {
#define PDCPL_EXTERN_C_END }
#else
#define PDCPL_EXTERN_C_BEGIN
#define PDCPL_EXTERN_C_END
#endif  // __cplusplus

// stringification macros
#define PDCPL_STRINGIFY_I(x) #x
#define PDCPL_STRINGIFY(x) PDCPL_STRINGIFY_I(x)

// concatenation macros
#define PDCPL_CONCAT_I(x, y) x ## y
#define PDCPL_CONCAT(x, y) PDCPL_CONCAT_I(x, y)

// allow C++-like use of inline in C code
#ifdef __cplusplus
#define PDCPL_INLINE inline
#else
#define PDCPL_INLINE static inline
#endif  // __cplusplus

// macro to indicate extern inline, i.e. never static inline
#define PDCPL_EXTERN_INLINE inline

// macros for the C++ version numbers
#define PDCPL_CPP_14 201402L
#define PDCPL_CPP_17 201703L
#define PDCPL_CPP_20 202002L

// C++ only section; none of these macros are defined when compiling C
#ifdef __cplusplus

// correct C++ integer value even when /Zc:__cplusplus is not defined.
#if defined(_MSC_VER)
#define PDCPL_CPLUSPLUS _MSVC_LANG
#else
#define PDCPL_CPLUSPLUS __cplusplus
#endif  // !defined(_MSC_VER)

// support for pre-C++20 constexpr
#if PDCPL_CPLUSPLUS >= PDCPL_CPP_20
#define PDCPL_CONSTEXPR_20 constexpr
#else
#define PDCPL_CONSTEXPR_20 const
#endif  // __cplusplus < PDCPL_CPP_20

#endif  // __cplusplus

#endif  // PDCPL_COMMON_H_
