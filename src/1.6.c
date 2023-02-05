/**
 * @file 1.6.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-6
 * @copyright MIT License
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
// need POSIX feature for fmemopen on non-Windows systems
#define _POSIX_C_SOURCE 200809L
#endif  // _WIN32

#include <errno.h>
#ifdef _WIN32
#include <fcntl.h>
#include <inttypes.h>
#include <io.h>
#include <limits.h>
#include <stdint.h>
#endif  // _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

// on warning level 3 or above, MSVC warns that strerror() is unsafe, so we
// create a wrapper that disables the warning and use strerror() as a macro
#ifdef _WIN32
static const char * win32_strerror(int errnum)
{
__pragma(warning (push))
__pragma(warning (disable: 4996))
  const char* errstr = strerror(errnum);
__pragma(warning (pop))
  return errstr;
}

#define strerror(eno) win32_strerror(eno)
#endif  // _WIN32

/**
 * Handle an error that results in `errno` being set with an optional message.
 *
 * @param has_error `true` if error, `false` if no error
 * @param message Custom message to use before displaying `errno` message
 */
#define PDCPL_HANDLE_ERRNO_EX(has_error, message) \
  do { \
    if (has_error) { \
      if (strlen(message)) \
        fprintf(stderr, "Error: %s: %s\n", message, strerror(errno)); \
      else \
        fprintf(stderr, "Error: %s\n", strerror(errno)); \
      return EXIT_FAILURE; \
    } \
  } \
  while (0)

/**
 * Handle an error that results in `errno` being set.
 *
 * @param has_error `true` if error, `false` if no error
 */
#define PDCPL_HANDLE_ERRNO(has_error) PDCPL_HANDLE_ERRNO_EX(has_error, "")

#define BUF_CONTENTS "abcdefg"

PDCPL_PROGRAM_USAGE_DEF
(
  "Verifies that getchar() != EOF is either 0 or 1."
  "\n\n"
  "Checks the expression value for each character in \"" BUF_CONTENTS "\" as\n"
  "if the contents were piped into stdin on the command line."
  "\n\n"
  "For ease of validation, we actually use fgetc() instead of getchar() and\n"
  "simply read the contents from a FILE * stream. getchar() is typically a\n"
  "layer over getc(stdin) or fgetc(stdin) anyways."
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // string to treat as in-memory file, its length, and its associated stream
  const char *buf = BUF_CONTENTS;
// on Windows, buf_len, stream unused (until we figure out what to do)
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4101 4189)
#endif  // _MSC_VER
  size_t buf_len = strlen(buf);
  FILE *stream;
#ifdef _MSC_VER
#pragma warning (pop)
#endif  // _MSC_VER
#ifdef _WIN32
  printf("No Windows impementation yet.\n");
  /*
  HANDLE buf_handle = CreateFileMappingA(
    INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD) buf_len, NULL
  );
  if (!buf_handle) {
    fprintf(
      stderr,
      "Error: Could not create file mapping handle. HRESULT: %" PRIx32 "\n",
      (int32_t) HRESULT_FROM_WIN32(GetLastError())
    );
    return EXIT_FAILURE;
  }
  LPVOID buf_map = MapViewOfFile(buf_handle, FILE_MAP_WRITE, 0, 0, buf_len);
  if (!buf_map) {
    fprintf(
      stderr,
      "Error: Could not create file map view. HRESULT: %" PRIx32 "\n",
      (int32_t) HRESULT_FROM_WIN32(GetLastError())
    );
    if (!CloseHandle(buf_handle))
      fprintf(
        stderr,
        "Error: Could not close file mapping handle. HRESULT: %" PRIx32 "\n",
        (int32_t) HRESULT_FROM_WIN32(GetLastError())
      );
    return EXIT_FAILURE;
  }
  CopyMemory(buf_map, buf, buf_len);
  int buf_fd = _open_osfhandle((intptr_t) buf_handle, _O_RDONLY);
  if (buf_fd == -1) {
    fprintf(
      stderr,
      "Error: Could not open OS file descriptor from handle. "
        "HRESULT: %" PRIx32 "\n",
      (int32_t) HRESULT_FROM_WIN32(GetLastError())
    );
    CloseHandle(buf_handle);  // check error
    UnmapViewOfFile(buf_map);  // check error
    return EXIT_FAILURE;
  }
  stream = _fdopen(buf_fd, "r");
  */
#else
  // none of this is reachable on Windows (yet)
  stream = fmemopen((void *) buf, buf_len, "r");
  PDCPL_HANDLE_ERRNO(!stream);
  // holds the value of pseudo getchar() != EOF
  int neof;
  do {
    printf("getchar() /*pseudo*/ != EOF (%d)\n", neof = (fgetc(stream) != EOF));
  }
  while (neof);
  // close file to clean up
  PDCPL_HANDLE_ERRNO(fclose(stream));
#endif  // _WIN32
  return EXIT_SUCCESS;
}
