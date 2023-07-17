/**
 * @file file.h
 * @author Derek Huang
 * @brief C header for pdcpl file utilities
 * @copyright MIT License
 */

#ifndef PDCPL_FILE_H_
#define PDCPL_FILE_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

#ifdef _WIN32
/**
 * Get the path of the directory designated for temporary files on Windows.
 *
 * @param path Address of `char *` to hold the directory path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
PDCPL_PUBLIC
HRESULT
pdcpl_win_gettempdir(char **path);

/**
 * Get a unique temporary file name using the Windows temp file directory.
 *
 * @param path Address of `char *` to hold the temp file path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer[s] cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
PDCPL_PUBLIC
HRESULT
pdcpl_win_gettempfilename(char **path);
#endif  // _WIN32

PDCPL_EXTERN_C_END

// don't pollute translation units
#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN
#endif  // _WIN32

#endif  // PDCPL_FILE_H_
