/**
 * @file math.h
 * @author Derek Huang
 * @brief C header for math functions
 * @copyright MIT License
 */

#ifndef PDCPL_MATH_H_
#define PDCPL_MATH_H_

#include <float.h>
#include <math.h>
#include <stdbool.h>

#include "pdcpl/common.h"
#include "pdcpl/features.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Check if two doubles are within an absolute tolerance.
 *
 * @param a First double
 * @param b Second double
 * @param eps Absolute tolerance
 */
PDCPL_INLINE bool
pdcpl_double_near(double a, double b, double eps)
{
  return fabs(a - b) <= fabs(eps);
}

/**
 * Check if the first double is less than the second double.
 *
 * @param a First double
 * @param b Second double
 */
PDCPL_INLINE bool
pdcpl_double_lt(double a, double b)
{
  return a + DBL_EPSILON < b;
}

/**
 * Check if the first double is greater than the second double.
 *
 * @param a First double
 * @param b Second double
 */
PDCPL_INLINE bool
pdcpl_double_gt(double a, double b)
{
  return a > b + DBL_EPSILON;
}

/**
 * Check if the two doubles are essentialy equal.
 *
 * @param a First double
 * @param b Second double
 */
PDCPL_INLINE bool
pdcpl_double_eq(double a, double b)
{
  return pdcpl_double_near(a, b, DBL_EPSILON);
}

/**
 * Check if two floats are within an absolute tolerance.
 *
 * @note Pre-C99 implementation widens to double in `pdcpl_double_near.`
 *
 * @param a First float
 * @param b Second float
 * @param eps Absolute tolerance
 */
PDCPL_INLINE bool
pdcpl_float_near(float a, float b, float eps)
{
// fabsf was introduced in C99
#if defined(PDCPL_HAS_C99)
  return fabsf(a - b) <= fabsf(eps);
#else
  return pdcpl_double_near(a, b, eps);
#endif  // !defined(PDCPL_HAS_C99)
}

/**
 * Check if the first float is less than the second float.
 *
 * @param a First float
 * @param b Second float
 */
PDCPL_INLINE bool
pdcpl_float_lt(float a, float b)
{
  return a + FLT_EPSILON < b;
}

/**
 * Check if the first float is greater than the second float.
 *
 * @param a First float
 * @param b Second float
 */
PDCPL_INLINE bool
pdcpl_float_gt(float a, float b)
{
  return a > b + FLT_EPSILON;
}

/**
 * Check if the two floats are essentialy equal.
 *
 * @param a First float
 * @param b Second float
 */
PDCPL_INLINE bool
pdcpl_float_eq(float a, float b)
{
  return pdcpl_float_near(a, b, FLT_EPSILON);
}

PDCPL_EXTERN_C_END

#endif  // PDCPL_MATH_H_
