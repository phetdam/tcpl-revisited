/**
 * @file string.h
 * @author Derek Huang
 * @brief C/C++ header for string helpers
 * @copyright MIT License
 */

#ifndef PDCPL_STRING_H_
#define PDCPL_STRING_H_

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"
#include "pdcpl/sa.h"

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
PDCPL_PUBLIC unsigned short
pdcpl_printpwtd(ptrdiff_t value, unsigned short padding);

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
PDCPL_PUBLIC int
pdcpl_strwc(PDCPL_SA(Opt(In)) const char *s, PDCPL_SA(Out) pdcpl_wcresults *rp);

/**
 * Count words, chars, and lines from a `FILE *` stream and save the results.
 *
 * @param f `FILE *` stream, can be `NULL`
 * @param rp Pointer to results struct
 * @returns 0 if no error, -EINVAL if `rp` is `NULL`
 */
PDCPL_PUBLIC int
pdcpl_fwc(PDCPL_SA(Opt(In)) FILE *f, PDCPL_SA(Out) pdcpl_wcresults *rp);

/**
 * Read a word from a `FILE *` stream to a buffer.
 *
 * On success, `*wp` will point to a `NULL`-terminated `char` buffer containing
 * the word read from `f`. If `ncp` is not `NULL`, it contains the length of
 * the buffer - 1, i.e. the length of the word.
 *
 * If `feof(f)` is nonzero, this function still succeeds, but `*ncp` is zero
 * and `*wp` will be `NULL`. If using this function in a loop, one should
 * check either `*wp` or `*ncp` in the loop condition.
 *
 * @param f `FILE *` stream to read line from
 * @param wp Address of a `char *` for pointing to the word buffer
 * @param ncp Address of `size_t` to store word length (can be `NULL`)
 * @returns 0 if no error, -EINVAL if `f` or `wp` are `NULL`, -ERANGE if the
 *  word read exceeds `SIZE_MAX - 1`, -ENOMEM if buffer [re]allocation fails
 */
PDCPL_PUBLIC int
pdcpl_getword(
  PDCPL_SA(In) FILE *f,
  PDCPL_SA(Opt(Out)) char **wp,
  PDCPL_SA(Opt(Out)) size_t *ncp);

/**
 * Read an arbitrary line from a `FILE *` stream to a buffer.
 *
 * On success, `*sp` will point to a `NULL`-terminated `char` buffer containing
 * the contents of the line, excluding the `\n`, read from `f`. If `ncp` is not
 * `NULL`, it contains the length of the buffer - 1, i.e. the line length.
 *
 * If `feof(f)` is nonzero, this function still succeeds, but `*ncp` is zero
 * and `*sp` will be `NULL`. If using this function in a loop, one should only
 * check that `*sp` is not `NULL` to determine if there is no more input to
 * get, as `*sp` could point to an empty string.
 *
 * @param f `FILE *` stream to read line from
 * @param sp Address of a `char *` for pointing to the line buffer
 * @param ncp Address of `size_t` to store line length (can be `NULL`)
 * @returns 0 if no error, -EINVAL if `f` or `sp` are `NULL`, -ERANGE if the
 *  line read exceeds `SIZE_MAX - 1`, -ENOMEM if buffer `(m|re)alloc` fails
 */
PDCPL_PUBLIC int
pdcpl_getline(
  PDCPL_SA(In) FILE *f,
  PDCPL_SA(Opt(Out)) char **sp,
  PDCPL_SA(Opt(Out)) size_t *ncp);

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
PDCPL_PUBLIC int
pdcpl_strrev(
  PDCPL_SA(Opt(In)) const char *s,
  PDCPL_SA(Opt(Out)) char **srp,
  PDCPL_SA(Opt(Out)) size_t *ncp);

/**
 * Compute the next tab stop from the current column position.
 *
 * @param col Current column, first column is 0
 * @param tab_size Tab size
 */
PDCPL_INLINE size_t
pdcpl_next_tab_stop(size_t col, unsigned int tab_size)
{
  return (col % tab_size == 0) ? col : (col / tab_size + 1) * tab_size;
}

/**
 * Return the number of columns advanced by the next tab from `col`.
 *
 * For example, if `col` was 13 and `tab_size` was 8, function returns 3.
 *
 * @param col Current column, first column is 0
 * @param tab_size Tab size
 */
PDCPL_INLINE unsigned int
pdcpl_next_tab_size(size_t col, unsigned int tab_size)
{
  return (unsigned int) (pdcpl_next_tab_stop(col, tab_size) - col);
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
PDCPL_PUBLIC int
pdcpl_detab(
  PDCPL_SA(In) FILE *in,
  PDCPL_SA(Out) FILE *out,
  unsigned int spaces,
  PDCPL_SA(Opt(Out)) size_t *nrp,
  PDCPL_SA(Opt(Out)) size_t *nwp);

/**
 * Return the hex value if valid hex character, -EINVAL otherwise.
 *
 * `c` must match `[0-9a-fA-F]{1}` to be a valid hex character.
 *
 * @param c Character to examine
 */
PDCPL_INLINE int
pdcpl_hexval(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -EINVAL;
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
PDCPL_PUBLIC int
pdcpl_htoj(PDCPL_SA(In) const char *s, PDCPL_SA(Out) intmax_t *out);

/**
 * Convert a string of hex digits into the equivalent integer value.
 *
 * The string must match `-{0,1}(0x|0X){0,1}[0-9a-fA-F]+`.
 *
 * @param s Non-empty string to convert
 * @param out Address to `int` to store converted value
 * @returns 0 on success, -EINVAL if `out` or `s` are `NULL` as well as if
 *  `s` is empty or misspecified (not a valid hex string).
 */
PDCPL_INLINE int
pdcpl_htoi(PDCPL_SA(In) const char *s, PDCPL_SA(Out) int *out)
{
  if (!out)
    return -EINVAL;
  // use pdcpl_htoj and just cast the value
  intmax_t value;
  if (pdcpl_htoj(s, &value))
    return -EINVAL;
  *out = (int) value;
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
PDCPL_PUBLIC int
pdcpl_strsq(
  PDCPL_SA(In) const char *s,
  PDCPL_SA(Out) char **op,
  PDCPL_SA(In) const char *ds);

/**
 * Return lowercase equivalent to an uppercase ASCII character.
 *
 * If the input is not an uppercase ASCII character, it is returned verbatim.
 *
 * @param c ASCII char
 */
PDCPL_INLINE char
pdcpl_tolower(char c)
{
  return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
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
 * @returns 0 on success, -EINVAL if `in` or `op` are `NULL`, -ENOMEM if there
 *  is not enough memory to expand or reallocate the output buffer
 */
PDCPL_PUBLIC int
pdcpl_strexpand(
  PDCPL_SA(In) const char *in,
  PDCPL_SA(Out) char **op,
  PDCPL_SA(Opt(Out)) size_t *nwp);

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
PDCPL_PUBLIC int
pdcpl_jtoa(
  ptrdiff_t x, PDCPL_SA(Out) char **sp, PDCPL_SA(Opt(Out)) size_t *ncp);

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
PDCPL_INLINE int
pdcpl_itoa(int x, PDCPL_SA(Out) char **sp, PDCPL_SA(Opt(Out)) size_t *ncp)
{
  return pdcpl_jtoa(x, sp, ncp);
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
PDCPL_PUBLIC int
pdcpl_strfind(
  PDCPL_SA(In) const char *s,
  PDCPL_SA(In) const char *ss,
  PDCPL_SA(Out) size_t *pp);

/**
 * Get the index of the rightmost occurrence of `ss` in `s`.
 *
 * If there are no occurrences, the reported index is `SIZE_MAX`.
 *
 * @param s String to be searched for `ss`
 * @param ss String to search for within `s`
 * @param pp Address of `size_t` to write rightmost occurrence index to
 */
PDCPL_PUBLIC int
pdcpl_strrfind(
  PDCPL_SA(In) const char *s,
  PDCPL_SA(In) const char *ss,
  PDCPL_SA(Out) size_t *pp);

/**
 * Concatenate two strings together into a new string.
 *
 * @param s1 First string
 * @param s2 Second string
 * @param op Address to `char *` to write the output string
 * @param ncp Address to `size_t` to write output string length (can be `NULL`)
 */
PDCPL_PUBLIC int
pdcpl_strcat(
  PDCPL_SA(In) const char *s1,
  PDCPL_SA(In) const char *s2,
  PDCPL_SA(Out) char **op,
  PDCPL_SA(Opt(Out)) size_t *ncp);

PDCPL_EXTERN_C_END

#endif  // PDCPL_STRING_H_
