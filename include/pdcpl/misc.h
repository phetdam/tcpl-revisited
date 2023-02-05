/**
 * @file misc.h
 * @author Derek Huang
 * @brief C/C++ header for miscellaneous functions
 * @copyright MIT License
 */

#ifndef PDCPL_MISC_H_
#define PDCPL_MISC_H_

#include "pdcpl/core.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Converts Fahrenheit to Celsius.
 *
 * @param temp Temperature in Fahrenheit
 */
static inline double
pdcpl_f2ctemp(double temp) { return 5 * (temp - 32) / 9; }

/**
 * Converts Celsius to Fahrenheit.
 *
 * @param temp Temperature in Celsius
 */
static inline double
pdcpl_c2ftemp(double temp) { return 9 * temp / 5 + 32; }

PDCPL_EXTERN_C_END

#endif  // PDCPL_MISC_H_
