/**
 * @file histogram.c
 * @author Derek Huang
 * @brief C source for the pdcpl histogram library
 * @copyright MIT License
 */

#include "pdcpl/histogram.h"

#include <stddef.h>

#include "pdcpl/dllexport.h"

/**
 * Increment histogram bin counts appropriately given a value.
 *
 * If `dp` is `NULL`, this function is a no-op and succeeds.
 *
 * @param dp Address to histdata struct
 * @param value Value to bucket
 * @returns 0 on success, -1 if `value < dp->bmin`, 1 if `value >= d->bmax`
 */
PDCPL_PUBLIC
int
pdcpl_histdata_bucket(pdcpl_histdata *dp, double value)
{
  if (!dp)
    return 0;
  // current bin lower bound
  double bin_lower = dp->bmin;
  // too small; failure
  if (value < bin_lower)
    return -1;
  // loop through each bin, trying to bucket the value
  for (size_t i = 0; i < dp->nb; i++) {
    // value in [bin_lower, bin_lower + dp->bw); success
    if (value >= bin_lower && value < bin_lower + dp->bw) {
      dp->counts[i]++;
      return 0;
    }
    bin_lower += dp->bw;
  }
  // too large; failure
  return 1;
}
