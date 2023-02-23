/**
 * @file cpp/common.h
 * @author Derek Huang
 * @brief C++ header for common macros
 * @copyright MIT License
 */

#ifndef PDCPL_CPP_COMMON_H_
#define PDCPL_CPP_COMMON_H_

// macros for the C++ version numbers
#define PDCPL_CXX_14 201402L
#define PDCPL_CXX_17 201703L
#define PDCPL_CXX_20 202002L

// support for pre-C++20 constexpr
#if __cplusplus >= PDCPL_CXX_20
#define PDCPL_CXX_20_CONSTEXPR constexpr
#else
#define PDCPL_CXX_20_CONSTEXPR const
#endif  // __cplusplus < PDCPL_CXX_20

#endif  // PDCPL_CPP_COMMON_H_
