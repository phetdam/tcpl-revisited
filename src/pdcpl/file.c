/**
 * @file file.c
 * @author Derek Huang
 * @brief C source for pdcpl file utilities
 * @copyright MIT License
 */

#include "pdcpl/file.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

#include <stdlib.h>

#ifdef _WIN32
/**
 * Get the path of the directory designated for temporary files on Windows.
 *
 * @param path Address of a `char *` to hold the directory path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
HRESULT
pdcpl_win_gettempdir(char **path)
{
  if (!path)
    return E_INVALIDARG;
  // allocate buffer to max possible size
  LPSTR buf = malloc(sizeof(CHAR) * (MAX_PATH + 1));
  if (!buf)
    return E_OUTOFMEMORY;
  // attempt to get the path. on error, 0 is returned
  DWORD path_len = GetTempPathA(MAX_PATH + 1, buf);
  if (!path_len) {
    free(buf);
    return HRESULT_FROM_WIN32(GetLastError());
  }
  // success, so reallocate buffer to correct size (+1 for null terminator)
  LPSTR path_buf = realloc(buf, path_len + 1);
  if (!path_buf) {
    free(buf);
    return E_OUTOFMEMORY;
  }
  // on success, write to path. CHAR is just a char typedef
  *path = (char *) path_buf;
  return S_OK;
}
#endif  // _WIN32
