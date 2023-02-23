/**
 * @file warnings.h
 * @author Derek Huang
 * @brief C/C++ header for warnings control
 * @copyright MIT License
 */

#ifndef PDCPL_WARNINGS_H_
#define PDCPL_WARNINGS_H_

// macros for disabling and reenabling warnings for MSVC, including push + pop.
// on other compilers, these macros are empty.
#ifdef _MSC_VER
#define PDCPL_MSVC_WARNING_DISABLE(...) \
  __pragma(warning (push)) \
  __pragma(warning(disable: __VA_ARGS__))
#define PDCPL_MSVC_WARNING_ENABLE() __pragma(warning(pop))
#else
#define PDCPL_MSVC_WARNING_DISABLE(...)
#define PDCPL_MSVC_WARNING_ENABLE()
#endif  // _MSC_VER

#endif  // PDCPL_WARNINGS_H_