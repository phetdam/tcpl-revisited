/**
 * @file warnings.h
 * @author Derek Huang
 * @brief C/C++ header for warnings control
 * @copyright MIT License
 */

#ifndef PDCPL_WARNINGS_H_
#define PDCPL_WARNINGS_H_

#include "pdcpl/common.h"

// macros for disabling and reenabling warnings for MSVC
#ifdef _MSC_VER
/**
 * Push warning state and disable specified warnings.
 *
 * @param ... List of warnings, e.g. 5045, 5105
 */
#define PDCPL_MSVC_WARNING_DISABLE(...) \
  __pragma(warning (push)) \
  __pragma(warning(disable: __VA_ARGS__))

/**
 * Pop warning state.
 */
#define PDCPL_MSVC_WARNING_ENABLE() __pragma(warning(pop))
#else
/**
 * Push warning state and disable specified warnings.
 *
 * @param ... List of warnings, e.g. 5045, 5105
 */
#define PDCPL_MSVC_WARNING_DISABLE(...)

/**
 * Pop warning state.
 */
#define PDCPL_MSVC_WARNING_ENABLE()
#endif  // _MSC_VER

// macros for disabling and reenabling warnings for GCC/Clang
#ifdef __GNUC__
/**
 * Push warning state and disable warning option.
 *
 * @param option Warning option, e.g. "-Wuninitialized"
 */
#define PDCPL_GCC_WARNING_DISABLE(option) \
  _Pragma("GCC diagnostic push") \
  _Pragma(PDCPL_STRINGIFY(GCC diagnostic ignored option))

/**
 * Pop warning state.
 */
#define PDCPL_GCC_WARNING_ENABLE() _Pragma("GCC diagnostic pop")
#else
/**
 * Push warning state and disable warning option.
 *
 * @param option Warning option, e.g. "-Wuninitialized"
 */
#define PDCPL_GCC_WARNING_DISABLE(option)

/**
 * Pop warning state.
 */
#define PDCPL_GCC_WARNING_ENABLE()
#endif  // __GNUC__

#endif  // PDCPL_WARNINGS_H_