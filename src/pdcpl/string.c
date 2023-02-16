/**
 * @file string.c
 * @author Derek Huang
 * @brief C source for the pdcpl string library
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "pdcpl/dllexport.h"

/**
 * Count words, chars, and lines in a string and save the results.
 *
 * @param s `NULL`-terminated string, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -EINVAL if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strwc(const char *s, pdcpl_wcresults *rp)
{
  if (!rp)
    return -EINVAL;
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
 * @returns 0 if no error, -EINVAL if `rp` is `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_fwc(FILE *f, pdcpl_wcresults *rp)
{
  if (!rp)
    return -EINVAL;
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
pdcpl_getline(FILE *f, char **sp, size_t *ncp)
{
  // we allow ncp to be NULL
  if (!f || !sp)
    return -EINVAL;
  // current buffer size, number of chars read, allocated buffer
  size_t buf_size = BUFSIZ, nc = 0;
  char *buf = malloc(buf_size * sizeof *buf);
  if (!buf)
    return -ENOMEM;
  // fgetc() value
  int c;
  // loop until we hit a newline
  for (; c = fgetc(f), (c != '\n' && c != EOF); nc++) {
    // if the buffer is full, realloc it (might fail)
    if (nc == buf_size) {
      // if it's too big, free the buffer and error out
      if (buf_size + BUFSIZ < buf_size) {
        free(buf);
        return -ERANGE;
      }
      buf_size += BUFSIZ;
      buf = realloc(buf, buf_size);
      if (!buf)
        return -ENOMEM;
    }
    // otherwise, keep going
    buf[nc] = (char) c;
  }
  // done, so update buf_size + realloc (unless size is perfect)
  if (nc != buf_size - 1) {
    buf_size = nc + 1;
    buf = realloc(buf, buf_size);
    if (!buf)
      return -ENOMEM;
  }
  // write '\0', update sp, optionally update ncp
  buf[nc] = '\0';
  *sp = buf;
  if (ncp)
    *ncp = nc;
  return 0;
}

/**
 * Write a reversed copy of a string `s` to a buffer.
 *
 * Behavior is undefined if `s` is not `NULL`-terminated. If the length of `s`
 * exceeds `SIZE_MAX - 1`, then it may be missing its trailing characters.
 *
 * @param s Byte string to reverse, if `NULL` function is a no-op
 * @param srp Address of a `char *` pointing to the reversed string
 * @param ncp Address of a `size_t` giving the string length (can be `NULL`)
 * @returns 0 if no error, -EINVAL if `srp` is `NULL`, -ENOMEM if malloc fails
 */
PDCPL_PUBLIC
int
pdcpl_strrev(const char *s, char **srp, size_t *ncp)
{
  // allow no-op if input string is NULL
  if (!s)
    return 0;
  // target must be valid, although we also allow ncp to be NULL
  if (!srp)
    return -EINVAL;
  // get length of string first + allocate new buffer (don't forget +1)
  size_t len = strlen(s);
  char *rev = malloc((len + 1) * sizeof *rev);
  if (!rev)
    return -ENOMEM;
  // fill in reverse, write '\0', and set *srp
  for (size_t i = 0; i < len; i++)
    rev[len - i - 1] = s[i];
  rev[len] = '\0';
  *srp = rev;
  // if ncp is not NULL, also write the string length written
  if (ncp)
    *ncp = len;
  return 0;
}
