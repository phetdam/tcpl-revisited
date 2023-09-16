/**
 * @file bitwise.h
 * @author Derek Huang
 * @brief C/C++ header for bitwise functions
 * @copyright MIT License
 */

#ifndef PDCPL_BITWISE_H_
#define PDCPL_BITWISE_H_

#include <errno.h>
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
#define PDCPL_BITMASK(n) ~(~0 << (n))

/**
 * Count the number of 1-bits in a value.
 *
 * By noting that `x &= (x - 1)` deletes the rightmost 1-bit, we can reduce the
 * number of loop iterations since we no longer have to go bit by bit.
 *
 * @param x Value to count 1-bits for
 */
PDCPL_PUBLIC unsigned short
pdcpl_bitcount(unsigned int x);

/**
 * Get the `n` bits `pos + 1 - n` through `pos` from `in`.
 *
 * Does not check validity of inputs, so use with caution.
 *
 * @param in Value to get bits from
 * @param pos Index of first bit to get, where `0` is index of rightmost bit
 * @param n Number of bits to get
 */
#define PDCPL_GETBITS(in, pos, n) (((in) >> (pos + 1 - n)) & PDCPL_BITMASK(n))

/**
 * Get the `n` bits `pos + 1 - n` through `pos` from `in`.
 *
 * A safer version of `PDCPL_GETBITS` that checks the input parameters.
 *
 * @param in Value to get bits from
 * @param out Address to `unsigned int` to write value with bits to
 * @param pos Index of first bit to get, where `0` is index of rightmost bit
 * @param n Number of bits to get
 * @returns 0 on success, -EINVAL if `out` is `NULL` or `n > (pos + 1)`
 */
PDCPL_INLINE int
pdcpl_getbits(
  unsigned int in, unsigned int *out, unsigned short pos, unsigned short n)
{
  // cannot ask for bits past the rightmost bit
  if (!out || n > (pos + 1))
    return -EINVAL;
  *out = PDCPL_GETBITS(in, pos, n);
  return 0;
}

/**
 * Set the `n` bits `pos + 1 - n` through `pos` from `in`.
 *
 * @param in Value to set bits for
 * @param out Address to `unsigned int` to write updated value to
 * @param pos Index of first bit to get, where `0` is index of rightmost bit
 * @param n Number of bits to get
 * @returns 0 on success, -EINVAL if `out` is `NULL` or `n > (pos + 1)`
 */
PDCPL_INLINE int
pdcpl_setbits(
  unsigned int in,
  unsigned int *out,
  unsigned short pos,
  unsigned short n,
  unsigned int src)
{
  if (!out || n > (pos + 1))
    return -EINVAL;
  // rightmost index of bits to set (also bits to shift to reach target field)
  unsigned short rpos = (unsigned short) (pos + 1 - n);
  // zero out the rightmost bits pos + 1 - n through pos of in and then set
  // the zero bits using the n rightmost bits from src
  in &= (~PDCPL_GETBITS(in, pos, n) << rpos) + PDCPL_GETBITS(in, rpos - 1, rpos);
  in |= (src & PDCPL_BITMASK(n)) << rpos;
  *out = in;
  return 0;
}

/**
 * Invert the `n` bits `pos + 1 - n` through `pos` from `in`.
 *
 * @param in Value to invert bits for
 * @param out Address to `unsigned int` to write updated value to
 * @param pos Index of first bit to get, where `0` is index of rightmost bit
 * @param n Number of bits to get
 * @returns 0 on success, -EINVAL if `out` is `NULL` or `n > (pos + 1)`
 */
PDCPL_INLINE int
pdcpl_invbits(
  unsigned int in, unsigned int *out, unsigned short pos, unsigned short n)
{
  // get the bits that we want to invert + set using the inverted bits
  unsigned int in_bits;
  if (pdcpl_getbits(in, &in_bits, pos, n))
    return -EINVAL;
  // no need to mask as only the n rightmost bits are used
  return pdcpl_setbits(in, out, pos, n, ~in_bits);
}

/**
 * Return `x` rotated to the right by `n` bits.
 *
 * @param x Value to rotate
 * @param n Number of bits to rotate by
 */
PDCPL_PUBLIC unsigned int
pdcpl_rrotbits(unsigned int x, unsigned short n);

PDCPL_EXTERN_C_END

#endif  // PDCPL_BITWISE_H_
