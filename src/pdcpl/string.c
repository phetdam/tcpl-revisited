/**
 * @file string.c
 * @author Derek Huang
 * @brief C source for the pdcpl string library
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "pdcpl/dllexport.h"

/**
 * Count words, chars, and lines in a string and save the results.
 *
 * @param s `NULL`-terminated string, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -1 if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strwc(const char *s, pdcpl_wcresults *rp)
{
  // allow valid result even with NULL string
  if (!s) {
    PDCPL_ZERO_WCRESULTS(rp);
    return 0;
  }
  // number of words, chars, lines + current string position, current char
  size_t nw = 0, nc = 0, nl = 0;
  char *cur = (char *) s;
  char c;
  // indicate if we are in or out of word
  bool in_word = false;
  // until end
  for (; (c = *cur) != '\0'; cur++) {
    nc++;
    PDCPL_WC_CHECK_LINE(c, nl);
    PDCPL_WC_CHECK_WORD(c, in_word, nw);
  }
  // populate results and return
  PDCPL_SET_WCRESULTS(rp, nw, nc, nl);
  return 0;
}

/**
 * Count words, chars, and lines from a `FILE *` stream and save the results.
 *
 * @param f `FILE *` stream, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -1 if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_fwc(FILE *f, pdcpl_wcresults *rp)
{
  if (!f) {
    PDCPL_ZERO_WCRESULTS(rp);
    return 0;
  }
  // number of words, chars, lines + current char
  size_t nw = 0, nc = 0, nl = 0;
  int c;
  // indicate if we are in or out of word
  bool in_word = false;
  // until end of stream
  while ((c = fgetc(f)) != EOF) {
    nc++;
    PDCPL_WC_CHECK_LINE(c, nl);
    PDCPL_WC_CHECK_WORD(c, in_word, nw);
  }
  // populate results and return
  PDCPL_SET_WCRESULTS(rp, nw, nc, nl);
  return 0;
}
