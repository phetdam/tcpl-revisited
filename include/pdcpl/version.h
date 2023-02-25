/**
 * @file version.h
 * @author Derek Huang
 * @brief C/C++ header for project version macros
 */

#ifndef PDCPL_VERSION_H_
#define PDCPL_VERSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/platform.h"

// major, minor, patch versions
#ifndef PDCPL_VERSION_MAJOR
#define PDCPL_VERSION_MAJOR 0
#endif  // PDCPL_VERSION_MAJOR
#ifndef PDCPL_VERSION_MINOR
#define PDCPL_VERSION_MINOR 0
#endif  // PDCPL_VERSION_MINOR
#ifndef PDCPL_VERSION_PATCH
#define PDCPL_VERSION_PATCH 0
#endif  // PDCPL_VERSION_PATCH

// version concatenation macros
#define PDCPL_VERSION_CONCAT_I(major, minor, patch) \
  major ## . ## minor ## . ## patch
#define PDCPL_VERSION_CONCAT(major, minor, patch) \
  PDCPL_VERSION_CONCAT_I(major, minor, patch)

// version token, version string
#define PDCPL_VERSION \
  PDCPL_VERSION_CONCAT( \
    PDCPL_VERSION_MAJOR, PDCPL_VERSION_MINOR, PDCPL_VERSION_PATCH \
  )
#define PDCPL_VERSION_STRING PDCPL_STRINGIFY(PDCPL_VERSION)

// build type, real build defines as lowercase CMake CMAKE_BUILD_TYPE
#ifndef PDCPL_BUILD_TYPE
#define PDCPL_BUILD_TYPE "debug"
#endif  // PDXKA_BUILD_TYPE

/**
 * Print version information from within a `PDCPL_ARG_MAIN`.
 *
 * If `PDCPL_SET_PROGRAM_NAME()` has not been called, program name is empty.
 */
#define PDCPL_PRINT_VERSION_INFO() \
  printf( \
    "%s %s (%s, %s %s %s)\n", \
    PDCPL_PROGRAM_NAME, \
    PDCPL_VERSION_STRING, \
    PDCPL_BUILD_TYPE, \
    PDCPL_SYSTEM_ARCH, \
    PDCPL_SYSTEM_NAME, \
    PDCPL_SYSTEM_VERSION \
  )

#endif  // PDCPL_VERSION_H_
