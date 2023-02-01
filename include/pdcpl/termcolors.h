/**
 * @file termcolors.h
 * @author Derek Huang
 * @brief C/C++ header for terminal color printing
 * @copyright MIT License
 */

#ifndef PDCPL_TERMCOLORS_H_
#define PDCPL_TERMCOLORS_H_

#ifdef _WIN32
// try to remove some header bloat
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <stdio.h>
#endif  // _WIN32

// reset all text attributes to default state
#define PDCPL_RESET "\x1B[0m"

// reset just foreground/background text attributes
#define PDCPL_FG_RESET "\x1B[39m"
#define PDCPL_BG_RESET "\x1B[49m"

// macros for changing foreground text colors
#define PDCPL_FG_BLACK "\x1B[30m"
#define PDCPL_FG_RED "\x1B[31m"
#define PDCPL_FG_GREEN "\x1B[32m"
#define PDCPL_FG_YELLOW "\x1B[33m"
#define PDCPL_FG_BLUE "\x1B[34m"
#define PDCPL_FG_MAGENTA "\x1B[35m"
#define PDCPL_FG_CYAN "\x1B[36m"
#define PDCPL_FG_WHITE "\x1B[37m"

// shorter names that imply only foreground color changes
#define PDCPL_BLACK PDCPL_FG_BLACK
#define PDCPL_RED PDCPL_FG_RED
#define PDCPL_GREEN PDCPL_FG_GREEN
#define PDCPL_YELLOW PDCPL_FG_YELLOW
#define PDCPL_BLUE PDCPL_FG_BLUE
#define PDCPL_MAGENTA PDCPL_FG_MAGENTA
#define PDCPL_CYAN PDCPL_FG_CYAN
#define PDCPL_WHITE PDCPL_FG_WHITE

// macros for changing background text colors
#define PDCPL_BG_BLACK "\x1B[40m"
#define PDCPL_BG_RED "\x1B[41m"
#define PDCPL_BG_GREEN "\x1B[42m"
#define PDCPL_BG_YELLOW "\x1B[43m"
#define PDCPL_BG_BLUE "\x1B[44m"
#define PDCPL_BG_MAGENTA "\x1B[45m"
#define PDCPL_BG_CYAN "\x1B[46m"
#define PDCPL_BG_WHITE "\x1B[47m"

/**
 * Print formatted output to `stdout` and process character control sequences.
 *
 * This is much slower on Windows, where several function calls are made to
 * get and set the attributes of the output handle. If there is an error,
 * `GetLastError()` needs to be called, with `HRESULT_FROM_WIN32()` used to
 * get the Windows `HRESULT` for diagnosing errors.
 *
 * On *nix systems, this is simply defined to be `printf`.
 *
 * @param ... Print format and arguments
 */
#ifdef _WIN32
#define PDCPL_PRINTF(...) \
  do { \
    HANDLE out_handle = GetStdHandle(STD_OUTPUT_HANDLE); \
    if (!out_handle || out_handle == INVALID_HANDLE_VALUE) \
      break; \
    DWORD out_mode; \
    if (!GetConsoleMode(out_handle, &out_mode)) \
      break; \
    if ( \
      !SetConsoleMode( \
        out_handle, out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING \
      ) \
    ) \
      break; \
    printf(__VA_ARGS__); \
    if (!SetConsoleMode(out_handle, out_mode)) \
      break; \
  } \
  while (0)
#else
#define PDCPL_PRINTF printf
#endif  // _WIN32

/**
 * Macro for creating a colored string printable with `PDCPL_PRINTF`.
 *
 * Both foreground and background colors can be specified to `color`, for
 * example by passing `PDCPL_FG_GREEN PDCPL_BG_RED`.
 *
 * @param color Color string, ex. `PDCPL_MAGENTA`
 * @param text Text to print
 */
#define PDCPL_COLOR_STRING(color, text) color text PDCPL_RESET

// macros for each specific color string available
#define PDCPL_RED_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_RED, text)
#define PDCPL_GREEN_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_GREEN, text)
#define PDCPL_YELLOW_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_YELLOW, text)
#define PDCPL_BLUE_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_BLUE, text)
#define PDCPL_MAGENTA_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_MAGENTA, text)
#define PDCPL_CYAN_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_CYAN, text)
#define PDCPL_WHITE_STRING(text) PDCPL_COLOR_STRING(PDCPL_FG_WHITE, text)

#endif  // PDCPL_TERMCOLORS_H_
