/**
 * @file file.h
 * @author Derek Huang
 * @brief C header for pdcpl file utilities
 * @copyright MIT License
 */

#ifndef PDCPL_FILE_H_
#define PDCPL_FILE_H_

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_MEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

#include <stdio.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"
#include "pdcpl/features.h"

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
PDCPL_PUBLIC HRESULT
pdcpl_win_gettempdir(char **path);

/**
 * Get a unique temporary file name using the Windows temp file directory.
 *
 * @param path Address of `char *` to hold the temp file path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer[s] cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
PDCPL_PUBLIC HRESULT
pdcpl_win_gettempfilename(char **path);

/**
 * Get a `HANDLE` to a new unique temporary file.
 *
 * The file is in-memory if possible and is deleted when on `HANDLE` close.
 *
 * @param file_handle Address of `HANDLE` to write to
 * @param access Desired file access, e.g. `GENERIC_READ`, `GENERIC_WRITE`
 * @param sharing File sharing mode, e.g. `0U` for exclusive process access,
 *  `FILE_SHARE_READ`, `FILE_SHARE_WRITE`, etc.
 * @returns `S_OK` on success, `E_INVALIDARG` if `file_handle` is `NULL`,
 *  `HRESULT_FROM_WIN32(GetLastError())` on file creation or other Win32 error
 */
PDCPL_PUBLIC HRESULT
pdcpl_win_gettempfh(LPHANDLE file_handle, DWORD access, DWORD sharing);

/**
 * Get a file descriptor to a new unique temporary file.
 *
 * The underlying file, which is in-memory if possible, is deleted when the
 * file descriptor associated with the underlying handle is closed.
 *
 * @param fd Address of file descriptor
 * @param flags Access flags from `fcntl.h`, e.g. `_O_RDONLY`, `_O_RDWR`
 * @returns `S_OK` on success, `E_INVALIDARG` if `fd` is `NULL`,
 *  `HRESULT_FROM_WIN32(GetLastError())` if `pdcpl_win_gettempfh` fails, or a
 *  generic `E_FAIL` if `_open_osfhandle` fails
 */
PDCPL_PUBLIC HRESULT
pdcpl_win_gettempfd(int *fd, int flags);
#endif  // _WIN32

#if defined(_WIN32) || defined(PDCPL_POSIX)
/**
 * Open file stream from a descriptor using the specified mode.
 *
 * @note Only available on Windows or for POSIX systems.
 *
 * @param fd File descriptor
 * @param mode Open mode, e.g. `"r"`, `"w+"`, etc.
 * @returns File stream, `NULL` on error. If `NULL`, check `errno` for status.
 */
PDCPL_PUBLIC FILE *
pdcpl_fdopen(int fd, const char *mode);
#endif  // !defined(_WIN32) && !defined(PDCPL_POSIX)

#ifdef _WIN32
/**
 * Open a file stream to a temporary file for binary reading + writing.
 *
 * The underlying file, which is in-memory if possible, is deleted on close.
 * This function is intended as a partial substitute for `fmemopen`.
 *
 * @param win_err Address to `HRESULT` to store Windows errors, can be `NULL`
 * @returns File stream, `NULL` on error. If `NULL`, check `errno` for status
 *  and `*win_err` if `win_err` was specified and not `NULL`.
 */
PDCPL_PUBLIC FILE *
pdcpl_win_tempfile(HRESULT *win_err);
#endif  // _WIN32

PDCPL_EXTERN_C_END

// don't pollute translation units
#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN
#endif  // _WIN32

#endif  // PDCPL_FILE_H_
