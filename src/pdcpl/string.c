/**
 * @file string.c
 * @author Derek Huang
 * @brief C source for the pdcpl string library
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/dllexport.h"
#include "pdcpl/memory.h"
#include "pdcpl/warnings.h"

/**
 * Return columns needed to fit a specified signed int with specified padding.
 *
 * Padding is applied on both sides of the printed value.
 *
 * @param value Value to get print width for
 * @param padding Padding to apply on both sides of the print width
 */
PDCPL_PUBLIC
unsigned short
pdcpl_printpwtd(ptrdiff_t value, unsigned short padding)
{
  // padding is on both sides + add extra col if value is negative
// MSVC complains about initialization from int to unsigned short in C++
PDCPL_MSVC_WARNING_DISABLE(4365)
  unsigned short pad_width = 2 * padding + ((value < 0) ? 1 : 0);
PDCPL_MSVC_WARNING_ENABLE()
  // if zero, need 1 column
// MSVC complains about return conversion from int to unsigned short in C++
PDCPL_MSVC_WARNING_DISABLE(4365)
  if (!value)
    return 1 + pad_width;
PDCPL_MSVC_WARNING_ENABLE()
  // log10 of PTRDIFF_MAX will never exceed unsigned short max
PDCPL_MSVC_WARNING_DISABLE(4365)
  return (unsigned short) ceil(log10(fabs(value))) + pad_width;
PDCPL_MSVC_WARNING_ENABLE()
}

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
 * If `feof(f)` is nonzero, this function still succeeds, but `*ncp` is zero
 * and `*sp` will be an empty string, i.e. just the null terminator.
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
 * @param s Non-empty string to convert
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

/**
 * Get a copy of `s` with all chars matching those in `ds` removed.
 *
 * Naive implementation that has time complexity of `O(strlen(s)strlen(ds))`.
 *
 * @param s Original source string
 * @param op Address to `char *` pointing to the squeezed string
 * @param ds String of chars to remove from the original, can be empty
 * @returns 0 on success, -EINVAL if any parameters are `NULL`, -ENOMEM if any
 *  of the memory allocation operations fail
 */
PDCPL_PUBLIC
int
pdcpl_strsq(const char *s, char **op, const char *ds)
{
  if (!s || !op || !ds)
    return -EINVAL;
  // length of s, ds. both can be empty, i.e. only contain '\0'
  size_t s_len = strlen(s), ds_len = strlen(ds);
  // stripped string to copy contents to
  char *ss;
  // if s is empty, just copy the empty string
  if (!s_len) {
    ss = calloc(1, sizeof *ss);
    if (!ss)
      return -ENOMEM;
    *op = ss;
    return 0;
  }
  // otherwise, allocate memory for ss
  ss = malloc((s_len + 1) * sizeof *ss);
  // if ds is empty, no chars to strip, so just copy
  if (!ds) {
    *op = strcpy(ss, s);
    return 0;
  }
  // otherwise, strip chars. use cursor so we don't need another loop index
  char *ss_cur = ss;
  for (size_t i = 0; i < s_len; i++) {
    // true if char should not be copied; skip if we have a match
    bool no_copy = false;
    for (size_t j = 0; j < ds_len; j++) {
      // match, so skip and stop inner loop
      if (s[i] == ds[j]) {
        no_copy = true;
        break;
      }
    }
    // only copy if no match
    if (!no_copy)
      *ss_cur++ = s[i];
  }
  // done copying, so write '\0', realloc, update *op, done
  *ss_cur = '\0';
  ss = realloc(ss, (strlen(ss) + 1) * sizeof *ss);
  if (!ss)
    return -ENOMEM;
  *op = ss;
  return 0;
}

/**
 * Return a new string that expands any char ranges into the actual characters.
 *
 * For example, regex-like ranges `0-9` and `a-z` would be expanded into the
 * full set of characters that would be contained. If an invalid range is
 * specified, e.g. `h-a`, the function will return an error.
 *
 * @param in Input string
 * @param op Address to `char *` pointing to the expanded string
 * @param nwp Address to `size_t` given length of new string, can be `NULL`
 */
PDCPL_PUBLIC
int
pdcpl_strexpand(const char *in, char **op, size_t *nwp)
{
  if (!in || !op)
    return -EINVAL;
  // shared error status code to be returned
  int ret = 0;
  // output index + length of input string
  size_t j = 0, in_len = strlen(in);
  // lower and upper range chars, output buffer
  char lower, upper;
  pdcpl_buffer buf = pdcpl_buffer_new(BUFSIZ * sizeof *in);
  if (!pdcpl_buffer_ready(&buf))
    return -ENOMEM;
  // number of chars to write in the range
  unsigned short r_len;
  // proceed through string
  for (size_t i = 0; i < in_len; i++) {
    // if we see '-' and have an alphanumeric char behind and in front, we can
    // define our range. lower < upper in order for it to be valid
    if (
      in[i] == '-' &&
      i > 0 && i < in_len - 1 &&
      isalnum(in[i - 1]) && isalnum(in[i + 1])
    ) {
      lower = in[i - 1];
      upper = in[i + 1];
      if (lower >= upper) {
        ret = -EINVAL;
        goto error;
      }
      // figure out how long the range is, realloc if need be, and write range
      r_len = upper - lower + 1;
      ret = pdcpl_buffer_dynexpand(&buf, PDCPL_PTR_SHIFT(buf.data, +, j), r_len);
      if (ret)
        goto error;
      // don't start from k = 0 since we already wrote the lower range char
      for (unsigned short k = 1; k < r_len; k++)
        PDCPL_INDEX_CHAR(buf.data, j++) = (char) (lower + k);
      // advance one extra index in input to skip char after current '-'
      i++;
    }
    // otherwise, just copy verbatim
    else
      PDCPL_INDEX_CHAR(buf.data, j++) = in[i];
  }
  // done writing, so realloc to j + 1, write '\0'
  ret = pdcpl_buffer_realloc(&buf, j + 1);
  if (ret)
    return -ENOMEM;
  PDCPL_INDEX_CHAR(buf.data, j) = '\0';
  // done, so assign data pointer to *op, written chars to nwp if not NULL
  *op = buf.data;
  if (nwp)
    *nwp = j;
  return 0;
// only clear buffer contents on error. no need to check pdcpl_buffer_clear
// return value as buf will not have NULL data
error:
  pdcpl_buffer_clear(&buf);
  return ret;
}

/**
 * Convert a signed integral value to a character string.
 *
 * Includes an extra space for the negative sign for any negative values.
 *
 * @param x Value to convert to string
 * @param sp Address of `char *` to write string to
 * @param ncp Address of `size_t` to write string length to (can be `NULL`)
 * @returns 0 on succes, -EINVAL if `sp` is `NULL`, -ENOMEM if `malloc` fails
 */
PDCPL_PUBLIC
int
pdcpl_jtoa(ptrdiff_t x, char **sp, size_t *ncp)
{
  if (!sp)
    return -EINVAL;
  // sign of x, chars needed to fit all of x (including sign)
  short sign = (x < 0) ? -1 : 1;
  unsigned short x_len = pdcpl_printwtd(x);
  // number of digits to write
  unsigned short n_digits = (x < 0) ? x_len - 1 : x_len;
  // buffer to write our digits to
  char *out = malloc((x_len + 1) * sizeof *out);
  if (!out)
    return -ENOMEM;
  // write digits going backwards along string
  out[x_len] = '\0';
  for (size_t i = 0; i < n_digits; x /= 10, i++)
    out[x_len - 1 - i] = (char) (sign * (x % 10) + '0');
  // handle sign if necessary + update sp
  if (sign < 0)
    out[0] = '-';
  *sp = out;
  // if ncp is not NULL, record length of the string
  if (ncp)
    *ncp = x_len;
  return 0;
}

/**
 * Get the index of the leftmost occurrence of `ss` in `s`.
 *
 * If there are no occurrences, the reported index is `SIZE_MAX`.
 *
 * @param s String to be searched for `ss`
 * @param ss String to search for within `s`
 * @param pp Address of `size_t` to write leftmost occurrence index to
 */
PDCPL_PUBLIC
int
pdcpl_strfind(const char *s, const char *ss, size_t *pp)
{
  if (!s || !ss || !pp)
    return -EINVAL;
  // length of original string and search string
  size_t s_len = strlen(s), ss_len = strlen(ss);
  // obviously ss doesn't fit in s if it is longer or if it's empty
  if (!ss_len || ss_len > s_len)
    goto no_match;
  // otherwise, search. we match first char of ss first to reduce comparisons
  for (size_t i = 0; i < s_len; i++) {
    // first chars match, now check that ss fits in s
    if (s[i] == ss[0]) {
      size_t j;
      for (j = i; j < s_len && j - i < ss_len && s[j] == ss[j - i]; j++)
        ;
      // if j - i == ss_len, we matched all chars and write i
      if (j - i == ss_len) {
        *pp = i;
        return 0;
      }
      // otherwise, keep going
    }
  }
  // search failure still counts as success, but *pp set to SIZE_MAX
no_match:
  *pp = SIZE_MAX;
  return 0;
}

/**
 * Get the index of the rightmost occurrence of `ss` in `s`.
 *
 * If there are no occurrences, the reported index is `SIZE_MAX`.
 *
 * @param s String to be searched for `ss`
 * @param ss String to search for within `s`
 * @param pp Address of `size_t` to write rightmost occurrence index to
 */
PDCPL_PUBLIC
int
pdcpl_strrfind(const char *s, const char *ss, size_t *pp)
{
  if (!s || !ss || !pp)
    return -EINVAL;
  // length of original string and search string
  size_t s_len = strlen(s), ss_len = strlen(ss);
  // obviously ss doesn't fit in s if it is longer or if it's empty
  if (!ss_len || ss_len > s_len)
    goto no_match;
  // otherwise, search. we match last char of ss first to reduce comparisons
  for (size_t i = 0; i < s_len; i++) {
    // last chars match, now check that ss fits in s by going backwards
    if (s[s_len - i - 1] == ss[ss_len - 1]) {
      size_t j;
      for (
        j = i;
        j < s_len &&
        j - i < ss_len &&
        s[s_len - j - 1] == ss[ss_len - (j - i) - 1];
        j++
      )
        ;
      // if j - i == ss_len, we matched all chars and write the position of the
      // first char of the rightmost ss_len, i.e. s_len - i - ss_len
      if (j - i == ss_len) {
        *pp = s_len - i - ss_len;
        return 0;
      }
      // otherwise, keep going
    }
  }
  // search failure still counts as success, but *pp set to SIZE_MAX
no_match:
  *pp = SIZE_MAX;
  return 0;
}

/**
 * Concatenate two strings together into a new string.
 *
 * @param s1 First string
 * @param s2 Second string
 * @param op Address to `char *` to write the output string
 * @param ncp Address to `size_t` to write output string length (can be `NULL`)
 */
PDCPL_PUBLIC
int
pdcpl_strcat(const char *s1, const char *s2, char **op, size_t *ncp)
{
  if (!s1 || !s2)
    return -EINVAL;
  // s1, s2 lengths (both can be zero), output string length, output buffer
  size_t s1_len = strlen(s1), s2_len = strlen(s2);
  size_t out_len = s1_len + s2_len;
  char *buf = malloc((out_len + 1) * sizeof *buf);
  if (!buf)
    return -ENOMEM;
  // copy s1 + s2 + write '\0'
  for (size_t i = 0; i < s1_len; i++)
    buf[i] = s1[i];
  for (size_t i = 0; i < s2_len; i++)
    buf[s1_len + i] = s2[i];
  buf[out_len] = '\0';
  // update *op and *ncp (if not NULL)
  *op = buf;
  if (ncp)
    *ncp = out_len;
  return 0;
}
