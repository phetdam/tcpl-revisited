/**
 * @file platform.h
 * @author Derek Huang
 * @brief C/C++ header for platform identification
 * @copyright MIT License
 */

#ifndef PDCPL_PLATFORM_H_
#define PDCPL_PLATFORM_H_

/**
 * Original preprocessor tests taken from my xkcd-alt project. The macro names
 * have of course been changed to be suitable for this project's use.
 */

// system name, real build defines using CMAKE_SYSTEM_NAME
#ifndef PDCPL_SYSTEM_NAME
// try to guess if not defined
#if defined(_WIN32)
#define PDCPL_SYSTEM_NAME "Windows"
#elif defined(__APPLE__) && defined(__MACH__)
#define PDCPL_SYSTEM_NAME "MacOS"
#elif defined(__CYGWIN__)
#define PDCPL_SYSTEM_NAME "Cygwin"
#elif defined(__linux__)
#define PDCPL_SYSTEM_NAME "Linux"
#else
#define PDCPL_SYSTEM_NAME "unknown"
#endif
#endif  // PDCPL_SYSTEM_NAME

// system version, hard to determine. real build uses CMAKE_SYSTEM_VERSION
#ifndef PDCPL_SYSTEM_VERSION
#define PDCPL_SYSTEM_VERSION "unknown"
#endif  // PDCPL_SYSTEM_VERSION

// system arch, hard to determine. real build uses CMAKE_SYSTEM_PROCESSOR
#ifndef PDCPL_SYSTEM_ARCH
#if defined(_WIN32)
#ifdef _WIN64
#define PDCPL_SYSTEM_ARCH "AMD64"
#else
#define PDCPL_SYSTEM_ARCH "x86"
#endif  // !_WIN64
#elif defined(__linux__)
#define PDCPL_SYSTEM_ARCH "x86_64"
#else
#define PDCPL_SYSTEM_ARCH "unknown"
#endif
#endif  // PDCPL_SYSTEM_ARCH

#endif  // PDCPL_PLATFORM_H_
