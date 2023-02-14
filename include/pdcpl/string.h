/**
 * @file string.h
 * @author Derek Huang
 * @brief C/C++ header for string helpers
 * @copyright MIT License
 */

#ifndef PDCPL_STRING_H_
#define PDCPL_STRING_H_

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
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
PDCPL_INLINE const char *
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
 * Return `true` if the char is a known escape character, including `EOF`.
 *
 * @param c Incoming character
 */
PDCPL_INLINE bool
pdcpl_isesc(int c)
{
  return (pdcpl_stresc(c)) ? true : false;
}

/**
 * Return columns needed to fit a specified signed int with specified padding.
 *
 * Padding is applied on both sides of the printed value.
 *
 * @param value Value to get print width for
 * @param padding Padding to apply on both sides of the print width
 */
PDCPL_INLINE unsigned short
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
PDCPL_INLINE unsigned short
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
 * Set the values of the `pdcpl_wcresults`.
 *
 * @param rp `pdcl_wcresults *` to set
 * @param wc Word count to set
 * @param cc Character count to set
 * @param lc Line count to set
 */
#define PDCPL_SET_WCRESULTS(rp, wc, cc, lc) \
  do { \
    rp->nw = wc; \
    rp->nc = cc; \
    rp->nl = lc; \
  } \
  while (0)

/**
 * Zero out a `pdcpl_wcresults` struct.
 *
 * @param rp `pdcl_wcresults *` to zero
 */
#define PDCPL_ZERO_WCRESULTS(rp) PDCPL_SET_WCRESULTS(rp, 0, 0, 0)

/**
 * Check if we should increment the line count.
 *
 * This macro should be used within a character reading loop.
 *
 * @param c Name holding character being read
 * @param nl Name holding number of lines counted
 */
#define PDCPL_WC_CHECK_LINE(c, nl) \
  if (c == '\n') \
      nl++

/**
 * Check if inside/outside a word, updating state and word count as needed.
 *
 * This macro should be used within a character reading loop.
 *
 * A word is counted on entry, otherwise a string like "abc" has 0 words. When
 * whitespace is seen, then we are outside a word. It's important to check for
 * whitespace first, as in_word is typically first initialized to `false`.
 *
 * @param c Name holding character being read
 * @param in_word `true` if inside a word, `false` otherwise
 * @param nw Name holding number of words counted
 */
#define PDCPL_WC_CHECK_WORD(c, in_word, nw) \
  do { \
    if (isspace(c)) \
      in_word = false; \
    else if (!in_word) { \
      in_word = true; \
      nw++; \
    } \
  } \
  while (0)

/**
 * Count words, chars, and lines in a string and save the results.
 *
 * @param s `NULL`-terminated string, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -EINVAL if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strwc(const char *s, pdcpl_wcresults *rp);

/**
 * Count words, chars, and lines from a `FILE *` stream and save the results.
 *
 * @param f `FILE *` stream, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -EINVAL if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_fwc(FILE *f, pdcpl_wcresults *rp);

/**
 * Read an arbitrary line from a `FILE *` stream to a buffer.
 *
 * On success, `*sp` will point to a `NULL`-terminated `char` buffer containing
 * the contents of the line, excluding the `\n`, read from `f`. If `ncp` is not
 * `NULL`, it contains the length of the buffer - 1, i.e. the line length.
 *
 * @param f `FILE *` stream to read line from
 * @param sp Address of a `char *` for pointing to the line buffer
 * @param ncp Address of `size_t` to store line length (can be `NULL`)
 * @returns 0 if no error, -EINVAL if `f` or `sp` are `NULL`, -ERANGE if the
 *  line read exceeds `SIZE_MAX - 1`, -ENOMEM if buffer `(m|re)alloc` fails
 */
PDCPL_PUBLIC
int
pdcpl_getline(FILE *f, char **sp, size_t *ncp);

PDCPL_EXTERN_C_END

#endif  // PDCPL_STRING_H_
