/**
 * @file 5.16.c
 * @author Derek Huang
 * @brief The C Programming Language exercises 5-14 through 5-16
 * @copyright MIT License
 */

// MSVC does not define __STDC_WANT_SECURE_LIB__ in limits.h
#ifdef _MSC_VER
#define __STDC_WANT_SECURE_LIB__ 0
#endif  // _MSC_VER

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#define PDCPL_HAS_PROGRAM_OPTIONS
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/memory.h"
#include "pdcpl/string.h"

/**
 * Number of lines to expand line buffer by on each reallocation.
 *
 * We set this as a macro so that the program option help updates too.
 */
#define PDCPL_SORT_CHUNK_LINES 1024

/**
 * Enum for the possible sorting methods.
 *
 * `SORT_MODE_MAX` just gives the total number of sort modes available.
 */
typedef enum {
  SORT_MODE_DEFAULT,
  SORT_MODE_DEFAULT_IGNORE_CASE,
  SORT_MODE_NUMERIC,
  SORT_MODE_DIRECTORY,
  SORT_MODE_DIRECTORY_IGNORE_CASE,
  SORT_MODE_MAX
} sort_mode;

/**
 * Static globals set during program option parsing.
 */
static sort_mode sort_program_mode = SORT_MODE_DEFAULT;
static bool reverse_target = false;
static size_t chunk_lines_target = PDCPL_SORT_CHUNK_LINES;

/**
 * Action to determine whether or not we sort using numeric comparison.
 */
static
PDCPL_CLIOPT_ACTION(numeric_sort_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  sort_program_mode = SORT_MODE_NUMERIC;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to determine wheter or not we sort using directory order.
 */
static
PDCPL_CLIOPT_ACTION(directory_sort_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  sort_program_mode = SORT_MODE_DIRECTORY;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to determine whether we sort in reverse, i.e. in descending order.
 */
static
PDCPL_CLIOPT_ACTION(reverse_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  reverse_target = true;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to determine whether we should ignore case or not while sorting.
 */
static
PDCPL_CLIOPT_ACTION(ignore_case_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  switch (sort_program_mode) {
    case SORT_MODE_DEFAULT:
      sort_program_mode = SORT_MODE_DEFAULT_IGNORE_CASE;
      break;
    case SORT_MODE_DIRECTORY:
      sort_program_mode = SORT_MODE_DIRECTORY_IGNORE_CASE;
      break;
    default:
      break;
  }
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to set number of lines to allocate when line buffer needs expansion.
 */
static
PDCPL_CLIOPT_ACTION(chunk_lines_action)
{
  PDCPL_CLIOPT_ACTION_ARGI_GUARD
  unsigned long chunk_lines = strtoul(argv[argi + 1], NULL, 10);
  // general conversion failure
  if (!chunk_lines)
    return PDCPL_CLIOPT_ERROR_CANT_CONVERT;
  // handle out of range error. errno is set to ERANGE here
  if (chunk_lines == ULONG_MAX)
    return -errno;
  chunk_lines_target = chunk_lines;
  return PDCPL_CLIOPT_PARSE_OK;
}

PDCPL_PROGRAM_USAGE_DEF
(
  "A minimal sort clone reading only from stdin.\n"
  "\n"
  "Sorts incoming input lines, by default lexicographically, although the\n"
  "actual sorting behavior can be adjusted using option flags.\n"
  "\n"
  "An original implementation of the sort program from The C Programming\n"
  "Language that includes features requested in exercises 5-14, 5-15, 5-16."
)

PDCPL_PROGRAM_OPTIONS_DEF
{
  {
    "-n", "--numeric-sort",
    "Compare lines by their atof() numeric values",
    0,
    numeric_sort_action,
    NULL
  },
  {
    "-d", "--directory-sort",
    "Compare lines only by their alphanumeric or whitespace characters. If "
    "specified with -n, --numeric-sort, the last specified option will be "
    "chosen as the sorting method.",
    0,
    directory_sort_action,
    NULL
  },
  {
    "-r", "--reverse",
    "Sort in descending instead of ascending order",
    0,
    reverse_action,
    NULL
  },
  {
    "-f", "--ignore-case",
    "Ignore alphabetic case when sorting. Has no effect if specified with -n, "
    "--numeric-sort",
    0,
    ignore_case_action,
    NULL
  },
  {
    "-l", "--chunk-lines",
    "Controls the amount of lines added to the line buffer when reallocation "
    "is needed to fit more input lines. Defaults to "
    PDCPL_STRINGIFY(PDCPL_SORT_CHUNK_LINES),
    1,
    chunk_lines_action,
    NULL
  },
  PDCPL_PROGRAM_OPTIONS_END
};

typedef int (*qsort_cmp)(const void *, const void *);

/**
 * Compare two strings in lexicographic order.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*a` sorts before `*b`, 0 if equal, 1 otherwise
 */
static inline int
strptr_cmp(const char **a, const char **b)
{
  return strcmp(*a, *b);
}

/**
 * Compare two strings in reverse lexicographic order.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*b` sorts before `*a`, 0 if equal, 1 otherwise
 */
static inline int
strptr_cmp_r(const char **a, const char **b)
{
  return -strptr_cmp(a, b);
}

/**
 * Compare two strings in lexicographic order, ignoring case.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*a` sorts before `*b` when ignoring case, 0 if equal when
 *  ignoring case, 1 when `*b` sorts before `*a` when ignoring case
 */
static int
strptr_lcmp(const char **a, const char **b)
{
  // strings and current char of strings
  const char *sa, *sb;
  char ca, cb;
  // compare until we can break
  for (sa = *a, sb = *b; *sa != '\0' && *sb != '\0'; sa++, sb++) {
    // a sorts first, b sorts first
    if ((ca = (char) tolower(*sa)) < (cb = (char) tolower(*sb)))
      return -1;
    if (cb < ca)
      return 1;
  }
  // strings are equal
  if (*sa == '\0' && *sb == '\0')
    return 0;
  // sa is shorter, sb is shorter
  if (*sa == '\0')
    return -1;
  return 1;
}

/**
 * Compare two strings in reverse lexicographic order, ignoring case.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*b` sorts before `*a` when ignoring case, 0 if equal when
 *  ignoring case, 1 when `*a` sorts before `*b` when ignoring case
 */
static inline int
strptr_lcmp_r(const char **a, const char **b)
{
  return -strptr_lcmp(a, b);
}

/**
 * Compare two strings by their `atof` double value.
 *
 * Clone of `numcmp` from section 5.11 in book, page 121.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `atof(*a)` is less than `atof(*b)`, 0 if the `atof` values
 *  are equal, 1 when `atof(*b)` is less than `atof(*a)`
 */
static int
strptr_ncmp(const char **a, const char **b)
{
  double va = atof(*a);
  double vb = atof(*b);
  if (va < vb)
    return -1;
  if (vb < va)
    return 1;
  return 0;
}

/**
 * Compare two strings in reverse by their `atof` double value.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `atof(*b)` is less than `atof(*a)`, 0 if the `atof` values
 *  are equal, 1 when `atof(*a)` is less than `atof(*b)`
 */
static inline int
strptr_ncmp_r(const char **a, const char **b)
{
  return -strptr_ncmp(a, b);
}

/**
 * Compare two strings by directory order.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*a` has directory order before `*b`, 0 if equal in terms of
 *  directory order, 1 when `*b` has directory order before `*a`
 */
static int
strptr_dcmp(const char **a, const char **b)
{
  // strings and current char of strings
  const char *sa, *sb;
  char ca, cb;
  // compare only on alphanumeric or whitespace chars until we break
  for (sa = *a, sb = *b; *sa != '\0' && *sb != '\0'; sa++, sb++) {
    // advance sa, sb until *sa, *sb are both whitespace/alphanumeric or '\0'
    while ((ca = *sa) != '\0' && ca != ' ' && !isalnum(ca))
      ca = *sa++;
    while ((cb = *sb) != '\0' && cb != ' ' && !isalnum(cb))
      cb = *sb++;
    // break if either are '\0'
    if (ca == '\0' || cb == '\0')
      break;
    // otherwise compare and continue
    if (ca < cb)
      return -1;
    if (cb < ca)
      return 1;
  }
  // strings are equal
  if (*sa == '\0' && *sb == '\0')
    return 0;
  // sa is shorter, sb is shorter
  if (*sa == '\0')
    return -1;
  return 1;
}

/**
 * Compare two strings in reverse by directory order.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*b` has directory order before `*a`, 0 if equal in terms of
 *  directory order, 1 when `*a` has directory order before `*b`
 */
static inline int
strptr_dcmp_r(const char **a, const char **b)
{
  return -strptr_dcmp(a, b);
}

/**
 * Compare two strings by directory order, ignoring case.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*a` has directory order before `*b` when ignoring case, 0 if
 *  equal in terms of directory order when ignoring case, 1 when `*b` has
 *  directory order before `*a` when ignoring case
 */
static int
strptr_ldcmp(const char **a, const char **b)
{
  // strings and current char of strings
  const char *sa, *sb;
  char ca, cb;
  // compare only on alphanumeric or whitespace chars until we break
  for (sa = *a, sb = *b; *sa != '\0' && *sb != '\0'; sa++, sb++) {
    // advance sa, sb until *sa, *sb are both whitespace/alphanumeric or '\0'
    while ((ca = (char) tolower(*sa)) != '\0' && ca != ' ' && !isalnum(ca))
      ca = *sa++;
    while ((cb = (char) tolower(*sb)) != '\0' && cb != ' ' && !isalnum(cb))
      cb = *sb++;
    // break if either are '\0'
    if (ca == '\0' || cb == '\0')
      break;
    // otherwise compare and continue
    if (ca < cb)
      return -1;
    if (cb < ca)
      return 1;
  }
  // strings are equal
  if (*sa == '\0' && *sb == '\0')
    return 0;
  // sa is shorter, sb is shorter
  if (*sa == '\0')
    return -1;
  return 1;
}

/**
 * Compare two strings by directory order, ignoring case.
 *
 * @param a Address of first string
 * @param b Address of second string
 * @returns -1 if `*a` has directory order before `*b` when ignoring case, 0 if
 *  equal in terms of directory order when ignoring case, 1 when `*b` has
 *  directory order before `*a` when ignoring case
 */
static inline int
strptr_ldcmp_r(const char **a, const char **b)
{
  return -strptr_ldcmp(a, b);
}

/**
 * Determine which `qsort` compare function should be used.
 *
 * @param out Address to a `qsort_cmp` to use as the compare function
 * @param mode Program sort mode
 * @param rev_sort `true` to sort in reverse, `false` not to
 * @returns 0 on success, -EINVAL if `out` is `NULL`
 */
static int
set_qsort_cmp(qsort_cmp *out, sort_mode mode, bool rev_sort)
{
  if (!out)
    return -EINVAL;
  switch (mode) {
    case SORT_MODE_DEFAULT:
      *out = (rev_sort) ? (qsort_cmp) strptr_cmp_r : (qsort_cmp) strptr_cmp;
      break;
    case SORT_MODE_DEFAULT_IGNORE_CASE:
      *out = (rev_sort) ? (qsort_cmp) strptr_lcmp_r : (qsort_cmp) strptr_lcmp;
      break;
    case SORT_MODE_NUMERIC:
      *out = (rev_sort) ? (qsort_cmp) strptr_ncmp_r : (qsort_cmp) strptr_ncmp;
      break;
    case SORT_MODE_DIRECTORY:
      *out = (rev_sort) ? (qsort_cmp) strptr_dcmp_r : (qsort_cmp) strptr_dcmp;
      break;
    case SORT_MODE_DIRECTORY_IGNORE_CASE:
      *out = (rev_sort) ? (qsort_cmp) strptr_ldcmp_r : (qsort_cmp) strptr_ldcmp;
      break;
    default:
      return -EINVAL;
  }
  return 0;
}

// local convenience macros to shorten the official names
#define ERRNO_EXIT(expr) PDCPL_MAIN_ERRNO_EXIT(expr)
#define EXIT(expr) PDCPL_MAIN_EXIT(expr)
#define EXIT_EX(expr, message) PDCPL_MAIN_EXIT_EX(expr, message)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // number of lines to expand storage by in terms of bytes
  size_t expand_size = sizeof(char **) * chunk_lines_target;
  // create new buffer to hold the lines, i.e. use as array of char **
  pdcpl_buffer linebuf = pdcpl_buffer_new(expand_size);
  EXIT_EX(!pdcpl_buffer_ready(&linebuf), "no initial line buffer\n");
  // number of lines read, current line
  size_t n_lines;
  char *line;
  // read the lines until there are no more
  for (n_lines = 0; ; n_lines++) {
    // handle line reading error + break if no lines to read
    ERRNO_EXIT(pdcpl_getline(stdin, &line, NULL));
    if (!line)
      break;
    // if necessary, expand the buffer by chunk_lines_target. note division by
    // sizeof(char **) to get the stored number of lines
    if (n_lines && !(n_lines % (linebuf.size / sizeof(char **))))
      ERRNO_EXIT(pdcpl_buffer_expand_exact(&linebuf, expand_size));
    // treat the buffer as a char ** to store the line strings
    PDCPL_INDEX((char **) linebuf.data, n_lines) = line;
  }
  // if no lines, just exit, otherwise realloc linebuf to n_lines lines
  if (!n_lines)
    return EXIT_SUCCESS;
  ERRNO_EXIT(pdcpl_buffer_realloc(&linebuf, n_lines * sizeof(char **)));
  // set compare function according to options
  qsort_cmp cmp;
  ERRNO_EXIT(set_qsort_cmp(&cmp, sort_program_mode, reverse_target));
  qsort(linebuf.data, n_lines, sizeof(char **), cmp);
  // print out + free the lines stored in linebuf's data pointer
  for (size_t i = 0; i < n_lines; i++) {
    printf("%s\n", PDCPL_INDEX((char **) linebuf.data, i));
    free(PDCPL_INDEX((char **) linebuf.data, i));
  }
  // clean up line buffer + exit
  pdcpl_buffer_clear(&linebuf);
  return EXIT_SUCCESS;
}
