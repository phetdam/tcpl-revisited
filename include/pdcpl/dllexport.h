/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL macro export defs
 * @copyright MIT License
 */

#ifndef PDCPL_DLLEXPORT_H_
#define PDCPL_DLLEXPORT_H_

#ifdef _WIN32
// all DLLs should be built with PDCPL_BUILD_DLL defined
#ifdef PDCPL_BUILD_DLL
// use  to handle import/export automatically
#define PDCPL_PUBLIC __declspec(dllexport)
#else
#define PDCPL_PUBLIC __declspec(dllimport)
#endif  // PDCPL_BUILD_DLL
#else
#define PDCPL_PUBLIC
#endif  // _WIN32

#endif  // PDCPL_DLLEXPORT_H_
