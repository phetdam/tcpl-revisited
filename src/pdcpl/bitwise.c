/**
 * @file bitwise.c
 * @author Derek Huang
 * @brief C source for bitwise functions
 * @copyright MIT License
 */

#include "pdcpl/bitwise.h"

#include <errno.h>
#include <stdint.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

/**
 * Count the number of 1-bits in a value.
 *
 * By noting that `x &= (x - 1)` deletes the rightmost 1-bit, we can reduce the
 * number of loop iterations since we no longer have to go bit by bit.
 *
 * @param x Value to count 1-bits for
 */
PDCPL_PUBLIC
unsigned short
pdcpl_bitcount(unsigned int x)
{
  unsigned short n_bits;
  for (n_bits = 0; x; x &= (x - 1))
    n_bits++;
  return n_bits;
}

/**
 * Get the `n` bits `pos + 1 - n` through `pos` from `in`.
 *
 * @param in Value to get bits from
 * @param out Address to `uintmax_t` to write bits to
 * @param pos Index of first bit to get, where `0` is index of rightmost bit
 * @param n Number of bits to get
 * @returns 0 on success, -EINVAL if `out` is `NULL` or `n > (pos + 1)`
 */
PDCPL_PUBLIC
int
pdcpl_getbits(
  unsigned int in, unsigned int *out, unsigned short pos, unsigned short n)
{
  // cannot ask for bits past the rightmost bit
  if (!out || n > (pos + 1))
    return -EINVAL;
  // extra parentheses around shift is for clarity but not necessary
  *out = (in >> (pos + 1 - n)) & PDCPL_BITMASK(n);
  return 0;
}
