/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL macro export definitions
 * @copyright MIT License
 */

#ifndef PDCPL_DLLEXPORT_H_
#define PDCPL_DLLEXPORT_H_

// building pdcpl shared
#if defined(PDCPL_DLL)
// Windows
#if defined(_WIN32)
// export if building DLL, import otherwise
#if defined(PDCPL_BUILD_DLL)
#define PDCPL_PUBLIC __declspec(dllexport)
#else
#define PDCPL_PUBLIC __declspec(dllimport)
#endif  // !defined(PDCPL_BUILD_DLL)
// non-Windows platforms. if using -fvisibility=hidden, for GCC/Clang we could
// have this defined to __attribute__((visibility("default")))
#else
#define PDCPL_PUBLIC
#endif  // !defined(_WIN32)
// build pdcpl static
#else
#define PDCPL_PUBLIC
#endif  // !defined(PDCPL_DLL)

// building pdcpl_bcdp shared
#if defined(PDCPL_DLL)
// Windows
#if defined(_WIN32)
// export if building DLL, import otherwise
#if defined(PDCPL_BCDP_BUILD_DLL)
#define PDCPL_BCDP_PUBLIC __declspec(dllexport)
#else
#define PDCPL_BCDP_PUBLIC __declspec(dllimport)
#endif  // !defined(PDCPL_BUILD_DLL)
// non-Windows platforms
#else
#define PDCPL_BCDP_PUBLIC
#endif  // !defined(_WIN32)
// build pdcpl_bcdp static
#else
#define PDCPL_BCDP_PUBLIC
#endif  // !defined(PDCPL_DLL)

#endif  // PDCPL_DLLEXPORT_H_
