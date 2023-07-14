/**
 * @file cpp/common.h
 * @author Derek Huang
 * @brief C++ header for common macros
 * @copyright MIT License
 */

#ifndef PDCPL_CPP_COMMON_H_
#define PDCPL_CPP_COMMON_H_

// macros for the C++ version numbers
#define PDCPL_CPP_14 201402L
#define PDCPL_CPP_17 201703L
#define PDCPL_CPP_20 202002L

// correct C++ integer value even when /Zc:__cplusplus is not defined
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

#endif  // PDCPL_CPP_COMMON_H_
