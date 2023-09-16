/**
 * @file histogram.h
 * @author Derek Huang
 * @brief C/C++ header for build text histograms
 */

#ifndef PDCPL_HISTOGRAM_H_
#define PDCPL_HISTOGRAM_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Struct holding the data needed for printing a histogram with equal bins.
 *
 * @param nb Number of histogram bins
 * @param counts Histogram bin counts, i.e. values in each bin
 * @param bw Histogram bin width
 * @param bmin Lower value bound of first bin (inclusive)
 * @param bmax Upper value bound of last bin (exclusive)
 */
typedef struct {
  size_t nb;
  size_t *counts;
  double bw;
  double bmin;
  double bmax;
} pdcpl_histdata;

/**
 * Initializes a new `pdcpl_histdata` struct.
 *
 * Zeroed memory is allocated for the bin counts array. After calling this
 * function, `PDCPL_FREE_HISTDATA` should be called to clean up.
 *
 * @param dp `pdcpl_histdata *` to init
 * @param nb_ Number of histogram bins
 * @param nw_ Histogram bin width
 * @param bmn Lower value bound of first bin (inclusive)
 * @param bmx Upper value bound of last bin (exclusive)
 */
#define PDCPL_INIT_HISTDATA(dp, nb_, bw_, bmn, bmx) \
  do { \
    dp->nb = nb_; \
    dp->counts = calloc(bc, sizeof dp->counts); \
    dp->bw = bw_; \
    dp->bmin = bmn; \
    dp->bmax = bmx; \
  } \
  while (0)

/**
 * Frees memory allocated for bin counts by `PDCPL_INIT_HISTDATA`.
 *
 * The `counts` pointer will be set to `NULL` as well.
 *
 * @param dp `pdcpl_histdata *` to free memory for
 */
#define PDCPL_FREE_HISTDATA(dp) \
  do { \
    free(dp->counts); \
    dp->counts = NULL; \
  } \
  while (0)

/**
 * Reset the histogram bin counts to zero.
 *
 * @param dp `pdcpl_histdata *` to free memory for
 */
#define PDCPL_RESET_HISTDATA(dp) memset(dp->counts, 0, dp->nb)

/**
 * Set equal histogram bin widths.
 *
 * @param dp `pdcpl_histdata *` to set bin widths for
 * @param bw Bin width to set
 */
#define PDCPL_SET_HISTDATA_BINS(dp, bw) \
  do { \
    for (size_t i = 0; i < dp->nb; i++) \
      dp->bins = bw; \
  } \
  while (0)

/**
 * Increment histogram bin counts appropriately given a value.
 *
 * If `dp` is `NULL`, this function is a no-op and succeeds.
 *
 * @param dp Address to histdata struct
 * @param value Value to bucket
 * @returns 0 on success, -1 if `value < dp->bmin`, 1 if `value >= d->bmax`
 */
PDCPL_PUBLIC int
pdcpl_histdata_bucket(pdcpl_histdata *dp, double value);

PDCPL_EXTERN_C_END

#endif  // PDCPL_HISTOGRAM_H_
