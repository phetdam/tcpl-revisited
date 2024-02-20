/**
 * @file 3.4.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 3-4
 * @copyright MIT License
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

/**
 * Modernized K&R implementation for converting an int to a character string.
 *
 * Uses the same logic as the original in section 3.6, page 64, but instead
 * uses pdcpl library functions for string reversal and allocates memory
 * itself. Also follows POSIX function return conventions.
 *
 * @param x Value to convert to string
 * @param sp Address of `char *` to write string to
 * @returns 0 on success, -EINVAL if `sp` is `NULL`, -ENOMEM if `malloc` fails
 */
static int knr_itoa(int x, char **sp)
{
  if (!sp)
    return -EINVAL;
  // buffer to write digits in reverse to
  char *rev = malloc((pdcpl_printwtd(x) + 1) * sizeof *rev);
  if (!rev)
    return -ENOMEM;
  // -- begin K&R impl --
  int i, sign;
  // record original x and take absolute value of current x
  if ((sign = x) < 0)
    x = -x;
  // write digits in reversed order
  i = 0;
  do {
    rev[i++] = (char) (x % 10 + '0');
  } while ((x /= 10) > 0);
  // write sign if necessary, write '\0'
  if (sign < 0)
    rev[i++] = '-';
  rev[i] = '\0';
  // -- end K&R impl --
  // reverse the string + free original buffer
  char *out;
  int ret = pdcpl_strrev(rev, &out, NULL);
  free(rev);
  // might have error, otherwise update sp and return
  if (ret)
    return ret;
  *sp = out;
  return 0;
}

PDCPL_PROGRAM_USAGE_DEF
(
  "Show corrected itoa implementation compared to K&R implementation.\n"
  "\n"
  "The original K&R implementation cannot handle the largest negative int\n"
  "under two's complement, i.e. -pow(2, CHAR_BIT * sizeof(int)). This is\n"
  "because in order to compute the digits, it negates the input integer, but\n"
  "if the input is the largest negative int, this will cause overflow as the\n"
  "largest positive int is pow(2, CHAR_BIT * sizeof(int)) - 1.\n"
  "\n"
  "The pdcpl jtoa and itoa implementations do not have this shortcoming as\n"
  "they multiply each digit by the input value's sign.\n"
  "\n"
  "Note that on Windows this program may be erroneously flagged as being a\n"
  "virus. Therfore, to run the program correctly, please add the program or\n"
  "the directory it resides in to the antivirus scan exclusion list."
)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  int x;
  // compare K&R and pdcpl itoa on non-extreme input
  x = -182371;
  char *out_knr, *out;
  if (knr_itoa(x, &out_knr))
    return EXIT_FAILURE;
  if (pdcpl_itoa(x, &out, NULL))
    return EXIT_FAILURE;
  printf("::   K&R itoa (%d) :: %s\n", x, out_knr);
  printf(":: pdcpl itoa (%d) :: %s\n", x, out);
  free(out_knr);
  free(out);
  // compare K&R and pdcpl itoa on INT_MIN
  x = INT_MIN;
  if (knr_itoa(x, &out_knr))
    return EXIT_FAILURE;
  if (pdcpl_itoa(x, &out, NULL))
    return EXIT_FAILURE;
  printf("::   K&R itoa (INT_MIN) :: %s\n", out_knr);
  printf(":: pdcpl itoa (INT_MIN) :: %s\n", out);
  free(out_knr);
  free(out);
  return EXIT_SUCCESS;
}
