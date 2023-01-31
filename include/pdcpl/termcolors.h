/**
 * @file termcolors.h
 * @author Derek Huang
 * @brief C/C++ header for terminal color printing
 * @copyright MIT License
 */

#ifndef PDCPL_TERMCOLORS_H_
#define PDCPL_TERMCOLORS_H_

// Windows terminals don't support *nix escape codes for color printing
#ifdef _WIN32
#define PDCPL_RED
#define PDCPL_GREEN
#define PDCPL_YELLOW
#define PDCPL_BLUE
#define PDCPL_MAGENTA
#define PDCPL_CYAN
#define PDCPL_WHITE
#define PDCPL_NO_COLOR
#else
#define PDCPL_RED "\x1B[31m"
#define PDCPL_GREEN "\x1B[32m"
#define PDCPL_YELLOW "\x1B[33m"
#define PDCPL_BLUE "\x1B[34m"
#define PDCPL_MAGENTA "\x1B[35m"
#define PDCPL_CYAN "\x1B[36m"
#define PDCPL_WHITE "\x1B[37m"
#define PDCPL_NO_COLOR "\x1B[0m"
#endif  // _WIN32

/**
 * Macro for creating a colored string printable with `printf`.
 *
 * On Windows platforms, this has no effect.
 *
 * @param color Color string, ex. `PDCPL_MAGENTA`
 * @param text Text to print
 */
#ifdef _WIN32
#define PDCPL_COLOR_STRING(color, text) text
#else
#define PDCPL_COLOR_STRING(color, text) color text PDCPL_NO_COLOR
#endif  // _WIN32

// macros for each specific color string available
#define PDCPL_RED_STRING(text) PDCPL_COLOR_STRING(PDCPL_RED, text)
#define PDCPL_GREEN_STRING(text) PDCPL_COLOR_STRING(PDCPL_GREEN, text)
#define PDCPL_YELLOW_STRING(text) PDCPL_COLOR_STRING(PDCPL_YELLOW, text)
#define PDCPL_BLUE_STRING(text) PDCPL_COLOR_STRING(PDCPL_BLUE_STRING, text)
#define PDCPL_MAGENTA_STRING(text) PDCPL_COLOR_STRING(PDCPL_MAGENTA_STRING, text)
#define PDCPL_CYAN_STRING(text) PDCPL_COLOR_STRING(PDCPL_CYAN, text)
#define PDCPL_WHITE_STRING(text) PDCPL_COLOR_STRING(PDCPL_WHITE, text)

// Windows-specific terminal color API (TBD)
#ifdef _WIN32
#endif  // _WIN32

#endif  // PDCPL_TERMCOLORS_H_
