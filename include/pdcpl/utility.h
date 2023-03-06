/**
 * @file utility.h
 * @author Derek Huang
 * @brief C/C++ header for helper utilities
 * @copyright MIT License
 */

#ifndef PDCPL_UTILITY_H_
#define PDCPL_UTILITY_H_

/**
 * Swap two lvalues of type `type`.
 *
 * This is the implementation macro and therefore does not expand macros.
 *
 * @param type Type of x, y
 * @param x First `type` lvalue
 * @param y Second `type` lvalue
 */
#define PDCPL_SWAP_I(type, x, y) \
  do { \
    type t = x; \
    x = y; \
    y = t; \
  } \
  while (0)

/**
 * Swap two lvalues of type `type`.
 *
 * @param type Type of x, y
 * @param x First `type` lvalue
 * @param y Second `type` lvalue
 */
#define PDCPL_SWAP(type, x, y) PDCPL_SWAP_I(type, x, y)

#endif  // PDCPL_UTILITY_H_
