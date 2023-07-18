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
#include <fcntl.h>
#include <io.h>
#endif  // _WIN32

#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
/**
 * Get the path of the directory designated for temporary files on Windows.
 *
 * @param path Address of `char *` to hold the directory path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
HRESULT
pdcpl_win_gettempdir(char **path)
{
  if (!path)
    return E_INVALIDARG;
  // allocate buffer to max possible size. need +1 for null terminator and note
  // that GetTempPathA max return value is MAX_PATH + 1
  LPSTR buf = malloc((MAX_PATH + 2) * sizeof *buf);
  if (!buf)
    return E_OUTOFMEMORY;
  // attempt to get the path. on error, 0 is returned
  DWORD path_len = GetTempPathA(MAX_PATH + 2, buf);
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
  *path = path_buf;
  return S_OK;
}

/**
 * Get a unique temporary file name using the Windows temp file directory.
 *
 * @param path Address of `char *` to hold the temp file path string
 * @returns `S_OK` on success, `E_INVALIDARG` if `path` is `NULL`,
 *  `E_OUTOFMEMORY` if buffer[s] cannot be allocated, or
 *  `HRESULT_FROM_WIN32(GetLastError())` if a Win32 call failed
 */
HRESULT
pdcpl_win_gettempfilename(char **path)
{
  if (!path)
    return E_INVALIDARG;
  // function return status
  HRESULT status;
  // get temp dir path and exit if error
  char *temp_dir_path;
  if FAILED(status = pdcpl_win_gettempdir(&temp_dir_path))
    return status;
  // allocate buffer for temp file path
  LPSTR buf = malloc(MAX_PATH * sizeof *buf);
  if (!buf) {
    status = E_OUTOFMEMORY;
    goto final;
  }
  // get temp file path. with 0, temp file is created and closed on success
  if (!GetTempFileNameA(temp_dir_path, "pdw", 0U, buf)) {
    free(buf);
    status = HRESULT_FROM_WIN32(GetLastError());
    goto final;
  }
  // resize name buffer to correct size
  size_t path_len = strlen(buf);
  char *temp_file_path = realloc(buf, path_len + 1);
  if (!temp_file_path) {
    free(buf);
    status = E_OUTOFMEMORY;
    goto final;
  }
  // delete the created temporary file. clean up temp_file_path on error
  if (!DeleteFileA(temp_file_path)) {
    free(temp_file_path);
    status = HRESULT_FROM_WIN32(GetLastError());
    goto final;
  }
  // done, just assign to path, update status, fall through
  *path = temp_file_path;
  status = S_OK;
// final clean up of temp dir path
final:
  free(temp_dir_path);
  return status;
}

/**
 * Get a `HANDLE` to a new unique temporary file.
 *
 * @param file_handle Address of `HANDLE` to write to
 * @param access Desired file access, e.g. `GENERIC_READ`, `GENERIC_WRITE`
 * @param sharing File sharing mode, e.g. `0U` for exclusive process access,
 *  `FILE_SHARE_READ`, `FILE_SHARE_WRITE`, etc.
 * @returns `S_OK` on success, `E_INVALIDARG` if `file_handle` is `NULL`,
 *  `HRESULT_FROM_WIN32(GetLastError())` on file creation or other Win32 error
 */
HRESULT
pdcpl_win_gettempfh(LPHANDLE file_handle, DWORD access, DWORD sharing)
{
  if (!file_handle)
    return E_INVALIDARG;
  // function return status
  HRESULT status;
  // get unique temp file path
  char *temp_file_path;
  if FAILED(status = pdcpl_win_gettempfilename(&temp_file_path))
    return status;
  // create temporary file
  HANDLE handle = CreateFileA(
    temp_file_path,
    access,
    // although we use FILE_FLAG_DELETE_ON_CLOSE, FILE_SHARE_DELETE is not
    // needed since the file path's uniqueness means the handle is unique too
    sharing,
    NULL,
    // name is unique so no need to worry about opening existing
    CREATE_ALWAYS,
    // avoid writing to disk and delete when handle is closed
    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
    NULL
  );
  if (handle == INVALID_HANDLE_VALUE) {
    status = HRESULT_FROM_WIN32(GetLastError());
    goto final;
  }
  // otherwise, handle is valid, so set status and fall through
  *file_handle = handle;
  status = S_OK;
// error creating temp file
final:
  free(temp_file_path);
  return status;
}

/**
 * Get a file descriptor to a new unique temporary file.
 *
 * @param fd Address of file descriptor
 * @param flags Access flags from `fcntl.h`, e.g. `_O_RDONLY`, `_O_RDWR`
 * @returns `S_OK` on success, `E_INVALIDARG` if `fd` is `NULL`,
 *  `HRESULT_FROM_WIN32(GetLastError())` if `pdcpl_win_gettempfh` fails, or a
 *  generic `E_FAIL` if `_open_osfhandle` fails
 */
HRESULT
pdcpl_win_gettempfd(int *fd, int flags)
{
  if (!fd)
    return E_INVALIDARG;
  // function return status
  HRESULT status;
  // get temp file handle
  HANDLE hnd;
  if FAILED(status = pdcpl_win_gettempfh(&hnd, GENERIC_READ | GENERIC_WRITE, 0U))
    return status;
  // attempt to open file descriptor
  int fdesc = _open_osfhandle((intptr_t) hnd, flags);
  if (fdesc < 0) {
    // close open handle first
    if (!CloseHandle(hnd))
      return HRESULT_FROM_WIN32(GetLastError());
    // _open_osfhandle doesn't have a very useful error status
    return E_FAIL;
  }
  // otherwise, we succeeded. file descriptor owns the handle now
  *fd = fdesc;
  return S_OK;
}
#endif  // _WIN32
