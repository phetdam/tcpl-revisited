/**
 * @file dllexport.h
 * @author Derek Huang
 * @brief C/C++ header for DLL macro export defs
 * @copyright MIT License
 */

#ifndef BDCL_DLLEXPORT_H_
#define BDCL_DLLEXPORT_H_

#ifdef _WIN32
// build bdcl DLL with BDCL_BUILD_DLL defined
#ifdef BDCL_BUILD_DLL
// use  to handle import/export automatically
#define BDCL_PUBLIC __declspec(dllexport)
#else
#define BDCL_PUBLIC __declspec(dllimport)
#endif  // BDCL_BUILD_DLL
#else
#define BDCL_PUBLIC
#endif  // _WIN32

#endif  // BDCL_DLLEXPORT_H_
