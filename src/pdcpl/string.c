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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
  // at least one line if nc >= 1, so increment nl to avoid undercount
  if (nc)
    nl++;
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
  // at least one line if nc >= 1, so increment nl to avoid undercount
  if (nc)
    nl++;
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

/**
 * Detab characters read from one stream when writing to another.
 *
 * Each tab character is replaced with an appropriate number of spaces until
 * the next tab stop. The number of chars read and written can also be
 * optionally stored if the appropriate pointers are not `NULL`.
 *
 * @param in `FILE *` stream to read from
 * @param out `FILE *` stream to write to
 * @param spaces Number of spaces per tab stop, must be positive
 * @param nrp Address to write number of chars read (can be `NULL`)
 * @param nwp Address to write number of chars written (can be `NULL`)
 * @returns 0 on success, -errno if there is a stream error
 */
PDCPL_PUBLIC
int
pdcpl_detab(FILE *in, FILE *out, unsigned int spaces, size_t *nrp, size_t *nwp)
{
  // we allow ncp, nwp to be optional
  if (!in || !out || !spaces)
    return -EINVAL;
  // fgetc() output, counter for spaces, number of chars read + written
  int c;
  unsigned int i;
  size_t n_read = 0, n_write = 0;
  // current column number of current line, first column is 0
  size_t line_col = 0;
  // number of spaces to next tab stop
  unsigned int to_next;
  // simple per-character replacement, updating read/written char counts
  while ((c = fgetc(in)) != EOF) {
    if (c == '\n')
      line_col = 0;
    if (c == '\t') {
      to_next = pdcpl_next_tab_size(line_col, spaces);
      for (i = 0; i < to_next; i++) {
        fputc(' ', out);
        // need to advance the current column as we write spaces
        line_col++;
        n_write++;
      }
    }
    else {
      fputc(c, out);
      // goes inside the else to prevent double-increment when we read a tab
      // as we already increment line_col enough inside the for loop
      line_col++;
      n_write++;
    }
    n_read++;
  }
  // check errors using errno
  if (ferror(in) || ferror(out))
    return -errno;
  // set read/written char counts individually if they are not NULL
  if (nrp)
    *nrp = n_read;
  if (nwp)
    *nwp = n_write;
  return 0;
}

/**
 * Convert a string of hex digits into the equivalent integer value.
 *
 * The string must match `-{0,1}(0x|0X){0,1}[0-9a-fA-F]+`.
 *
 * @param s Non-empty string to conver
 * @param out Address to `intmax_t` to store converted value
 * @returns 0 on success, -EINVAL if `out` or `s` are `NULL` as well as if
 *  `s` is empty or misspecified (not a valid hex string).
 */
PDCPL_PUBLIC
int
pdcpl_htoj(const char *s, intmax_t *out)
{
  if (!s || !out)
    return -EINVAL;
  // current hex multipler, length of s
  intmax_t mult = 1;
  size_t s_len = strlen(s);
  // we don't allow empty strings
  if (!s_len)
    return -EINVAL;
  // non-const convenience pointer to s + number of start chars to skip
  char *t = (char *) s;
  unsigned short skip_chars = 0;
  // check for negative sign + increase skip
  if (*t == '-') {
    mult = -mult;
    t++;
    skip_chars++;
  }
  // check if there is a "0x" or "0X" + increase skip. note that we must ensure
  // that s_len - skip_chars is >= 2, otherwise we will be out of bounds. there
  // is no more need to increment t, so we just don't do that
  if (s_len - skip_chars >= 2 && *t == '0' && (t[1] == 'x' || t[1] == 'X'))
    skip_chars += 2;
  // current hex char value, local value to update
  int hex_c;
  intmax_t value = 0;
  // loop backwards so we go from lowest to highest digit
  for (size_t i = 0; i < s_len - skip_chars; i++) {
    // get hex value of current char; is < 0 on error
    hex_c = pdcpl_hexval(s[s_len - i - 1]);
    if (hex_c < 0)
      return -EINVAL;
    // update hex value and advance multiplier
    value += mult * hex_c;
    mult *= 16;
  }
  // done, so write the value and return 0
  *out = value;
  return 0;
}