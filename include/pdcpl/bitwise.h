/**
 * @file bitwise.h
 * @author Derek Huang
 * @brief C/C++ header for bitwise functions
 * @copyright MIT License
 */

#ifndef PDCPL_BITWISE_H_
#define PDCPL_BITWISE_H_

#include <stdint.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Mask for the rightmost `n` bits of a value.
 *
 * Note that `~0` must be used instead of `1`, as `1` defaults to signed int.
 *
 * @param n Width of mask for rightmost bits
 */
#define PDCPL_BITMASK(n) ~(~0 << n)

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
pdcpl_bitcount(unsigned int x);

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
  unsigned int in, unsigned int *out, unsigned short pos, unsigned short n);

PDCPL_EXTERN_C_END

#endif  // PDCPL_BITWISE_H_
