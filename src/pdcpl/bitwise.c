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
 * Return `x` rotated to the right by `n` bits.
 *
 * @param x Value to rotate
 * @param n Number of bits to rotate by
 */
PDCPL_PUBLIC
unsigned int
pdcpl_rrotbits(unsigned int x, unsigned short n)
{
  // number of bits x the incoming value
  unsigned short n_inbits = 8 * (sizeof x);
  // only rotate the minimum amount necessary
  while (n > n_inbits)
    n -= n_inbits;
  // save the rotated bits, right shift, and paste the rotated bits back
  unsigned int rotbits = PDCPL_GETBITS(x, n - 1, n);
  return (x >> n) + (rotbits << (n_inbits - n));
}
