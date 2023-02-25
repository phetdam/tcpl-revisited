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
 * Count the number of 1-bits in a value.
 *
 * By noting that `x &= (x - 1)` deletes the rightmost 1-bit, we can reduce the
 * number of loop iterations since we no longer have to go bit-by-bit.
 *
 * @param x Value to count 1-bits for
 */
PDCPL_PUBLIC
unsigned short
pdcpl_bitcount(uintmax_t x);

PDCPL_EXTERN_C_END

#endif  // PDCPL_BITWISE_H_
