/**
 * @file string.h
 * @author Derek Huang
 * @brief C/C++ header for string helpers
 * @copyright MIT License
 */

#ifndef PDCPL_STRING_H_
#define PDCPL_STRING_H_

#include <math.h>
#include <stddef.h>
#include <stdio.h>

#include "pdcpl/core.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Return the string associated with the escaped character.
 *
 * If the incoming character is not an escape character, return `NULL`. `EOF`
 * is also handled here and treated as an escape character.
 *
 * @param c Incoming character
 */
static inline const char *
pdcpl_stresc(int c)
{
  switch (c) {
    case '\a':
      return "\\a";
    case '\b':
      return "\\b";
// on Windows, \e is not recognized as an escape character
#ifndef _WIN32
    case '\e':
      return "\\e";
#endif  // _WIN32
    case '\f':
      return "\\f";
    case '\n':
      return "\\n";
    case '\r':
      return "\\r";
    case '\t':
      return "\\t";
    case '\v':
      return "\\v";
    case '\\':
      return "\\\\";
    case '\'':
      return "\\\'";
    case '\"':
      return "\\\"";
    case EOF:
      return "EOF";
    default:
      break;
  }
  return NULL;
}

/**
 * Return columns needed to fit a specified signed int with specified padding.
 *
 * Padding is applied on both sides of the printed value.
 *
 * @param value Value to get print width for
 * @param padding Padding to apply on both sides of the print width
 */
static inline unsigned short
pdcpl_printpwtd(ptrdiff_t value, unsigned short padding)
{
  // padding is on both sides + add extra col if value is negative
  unsigned short pad_width = 2 * padding + ((value < 0) ? 1 : 0);
  // if zero, need 1 column
  if (!value)
    return 1 + pad_width;
  // log10 of PTRDIFF_MAX will never exceed unsigned short max
  return (unsigned short) round(log10(fabs(value))) + pad_width;
}

/**
 * Return columns needed to fit a specified signed int.
 *
 * @param value Value to get print width for
 */
static inline unsigned short
pdcpl_printwtd(ptrdiff_t value)
{
  return pdcpl_printpwtd(value, 0);
}

/**
 * Struct holding results of a `pdcpl_strwc` call.
 */
typedef struct {
  size_t nw;  // word count
  size_t nc;  // char count
  size_t nl;  // line count
} pdcpl_wcresults;

/**
 * Count words, chars, and lines in a string and save the results.
 *
 * @param s `NULL`-terminated string, can be `NULL`
 * @param res Pointer to results struct
 * @returns 0 if no error, -1 if `res` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strwc(const char *s, pdcpl_wcresults *res);

PDCPL_EXTERN_C_END

#endif  // PDCPL_STRING_H_
