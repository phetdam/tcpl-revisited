/**
 * @file string.c
 * @author Derek Huang
 * @brief C source for the pdcpl string library
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <stdbool.h>
#include <stddef.h>

#include "pdcpl/dllexport.h"

/**
 * Count words, chars, and lines in a string and save the results.
 *
 * @param s `NULL`-terminated string, can be `NULL`
 * @param res Pointer to results struct
 * @returns 0 if no error, -1 if `res` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strwc(const char *s, pdcpl_wcresults *res)
{
  // allow valid result even with NULL string
  if (!s) {
    res->nw = res->nc = res->nl = 0;
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
    if (c == '\n')
      nl++;
    // we count a word on entry, as otherwise a string like "abc" has 0 words
    if (!in_word) {
      in_word = true;
      nw++;
    }
    // not in a word if we see whitespace
    else if (c == ' ' || c == '\t' || c == '\n')
      in_word = false;
  }
  // populate results
  res->nw = nw;
  res->nc = nc;
  res->nl = nl;
  return 0;
}
