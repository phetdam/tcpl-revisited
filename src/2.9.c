/**
 * @file 2.9.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 2-9
 * @copyright MIT License
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/bitwise.h"
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

/**
 * Count the number of 1-bits in a value.
 *
 * The original implementation, excluding changes to the input and return
 * types, from the book in section 2.10, page 50, which goes bit by bit.
 * Unlike `pdcpl_bitcount`, this requires more iterations as well as evaluation
 * of a conditional, which makes it the slower implementation.
 */
static unsigned short
bitcount(unsigned int x)
{
  unsigned short b;
  for (b = 0; x != 0; x >>= 1)
    if (x & 01)
      b++;
  return b;
}

/**
 * Return microseconds taken to count bits for `x` `n` times.
 *
 * @param f Function to use for counting bits
 * @param x Value to count bits for
 * @param n Number of iterations to run
 */
static double
bitcount_us(unsigned short (*f)(unsigned int), unsigned int x, size_t n)
{
  clock_t clock_start = clock();
  for (size_t i = 0; i < n; i++)
    f(x);
  clock_t clock_end = clock();
  return (double) (clock_end - clock_start) / CLOCKS_PER_SEC * 1e6;
}

#define N_BITCOUNTS 10000000
#define B_VALUE 0b1010111010110110000101

PDCPL_PROGRAM_USAGE_DEF
(
  "Print time taken by " PDCPL_STRINGIFY(N_BITCOUNTS)
    " iterations of counting 1-bits."
  "\n"
  "Times taken by the original example implementation in The C Programming\n"
  "Language and the suggested faster implementation left as an exercise are\n"
  "both displayed, The faster implementation makes use of the fact that for\n"
  "some value x, x &= (x - 1) zeros out the rightmost 1-bit of x. This is\n"
  "because for any x, x - 1 will have the rightmost 1-bit of x be 0, with\n"
  "subsequent bits set to 1, as per binary arithmetic rules. Thus,\n"
  "x &= (x - 1) zeros out the rightmost 1-bit of x.\n"
  "\n"
  "The reason this property makes the faster implementation faster is because\n"
  "in the original implementation, the for loop must go bit by bit and\n"
  "evaluate an extra conditional statement each iteration. However, the\n"
  "faster implementation can move multiple bits per iteration and needs not\n"
  "evaluate an extra conditional like the original implementation must.\n"
  "\n"
  "For example, using 10000000 as the number of iterations and\n"
  "0b1010111010110110000101 as the value to count bits, the output printed\n"
  "to screen by this program would be something like\n"
  "\n"
  "  ::  TCPL bitcount (10000000 iterations) :: 421.875 ms\n"
  "  :: pdcpl bitcount (10000000 iterations) :: 171.875 ms"
);

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // use %d specifier since numeric values are int by default
  printf("::  TCPL bitcount (%d iterations) :: ", N_BITCOUNTS);
  printf("%g ms\n", bitcount_us(bitcount, B_VALUE, N_BITCOUNTS) / 1e3);
  printf(":: pdcpl bitcount (%d iterations) :: ", N_BITCOUNTS);
  printf("%g ms\n", bitcount_us(pdcpl_bitcount, B_VALUE, N_BITCOUNTS) / 1e3);
  return EXIT_SUCCESS;
}
