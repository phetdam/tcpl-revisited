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
