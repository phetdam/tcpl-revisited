/**
 * @file memory.c
 * @author Derek Huang
 * @brief C source for memory utilities
 * @copyright MIT License
 */

#include "pdcpl/memory.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "pdcpl/common.h"

/**
 * Expand the buffer by the specified number of bytes.
 *
 * @param buf Address to buffer
 * @param expand_size Number of bytes to expand buffer by
 * @returns 0 on success, -EINVAL if `buf` or its data is `NULL`, -ENOMEM if
 *  the call to `realloc` fails, i.e. not enough memory to realloc
 */
int
pdcpl_buffer_expand_exact(pdcpl_buffer *buf, size_t expand_size)
{
  if (!buf || !buf->data)
    return -EINVAL;
  if (!expand_size)
    return 0;
  void *new_data = realloc(buf->data, buf->size + expand_size);
  if (!new_data)
    return -ENOMEM;
  buf->data = new_data;
  buf->size += expand_size;
  return 0;
}

/**
 * Expand buffer if writing `write_size` bytes at `pos` exceeds capacity.
 *
 * The most general of the dynamic buffer expansion functions.
 *
 * @param buf Address to buffer
 * @param pos Position in buffer data to begin writing at
 * @param write_size Number of bytes to write, can spill out of buffer
 * @param compute_expansion Function used to compute the number of bytes to
 *  expand the existing capacity of the `buf` buffer by
 * @param data Additional data used by the `compute_expansion` function
 * @returns 0 on success, -EINVAL if any pointer args are `NULL`, -ENOMEM if
 *  the call to realloc fails, any negative `int` if `compute_expansion` fails
 */
int
pdcpl_buffer_dynexpand_custom(
  pdcpl_buffer *buf,
  const void *pos,
  size_t write_size,
  pdcpl_buffer_expansion_computer compute_expansion,
  void *data)
{
  // can't have pointers be NULL, pos be outside the buffer
  if (
    !buf ||
    !buf->data ||
    !pos ||
    !compute_expansion ||
    PDCPL_PTR_LT(pos, buf->data) ||
    PDCPL_PTR_GT(pos, PDCPL_PTR_SHIFT(buf->data, +, buf->size - 1))
  )
    return -EINVAL;
  // if pos + write_size - 1 exceeds the buffer boundary, realloc
  if (
    PDCPL_PTR_SHIFT(pos, +, write_size) >
    PDCPL_PTR_SHIFT(buf->data, +, buf->size)
  ) {
    // compute desired expansion size. if negative, return as error
    ptrdiff_t ex_size = compute_expansion(buf, pos, write_size, data);
    if (ex_size < 0)
      return (int) ex_size;
    // attempt realloc, and if successful, update buf struct
    void *new_data = realloc(buf->data, buf->size + ex_size);
    if (!new_data)
      return -ENOMEM;
    buf->data = new_data;
    buf->size += ex_size;
  }
  return 0;
}

/**
 * Compute min bytes needed to accommodate writing write_size bytes at pos.
 *
 * Assumes that `buf->data` <= `pos` <= `buf->data + buf->size - 1`, i.e. we
 * are writing within the relevant buffer, and that `pos + write_size` >
 * `buf->data + buf->size`, i.e. the address last byte to be written will fall
 * outside the bounds of the available buffer's memory (-1 removed).
 *
 * @param buf Address to buffer
 * @param pos Position in data buffer to write to
 * @param write_size Number of bytes to write
 * @param data Data needed for computation (unused)
 */
static inline ptrdiff_t
pdcpl_compute_expansion_exact(
  pdcpl_buffer *buf, const void *pos, size_t write_size, void *data)
{
  (void) data;  // silence compiler warning
  return PDCPL_PTR_OP(pos, -, buf->data) + write_size - buf->size;
}

/**
 * Expand buffer if writing `write_size` bytes at `pos` exceeds capacity.
 *
 * Only expands buffer capacity by the exact amount number of bytes needed in
 * order to fit the bytes that will be written starting at `pos`.
 *
 * @param buf Address to buffer
 * @param pos Position in buffer data to begin writing at
 * @param write_size Number of bytes to write, can spill out of buffer
 * @returns 0 on success, -EINVAL if any pointer args are `NULL`, -ENOMEM if
 *  the call to realloc fails, i.e. not enough memory to realloca
 */
int
pdcpl_buffer_dynexpand_exact(
  pdcpl_buffer *buf, const void *pos, size_t write_size)
{
  return pdcpl_buffer_dynexpand_custom(
    buf, pos, write_size, pdcpl_compute_expansion_exact, NULL
  );
}

/**
 * Compute min bytes as multiple of `BUFSIZ` needed to accommodate write.
 *
 * Same assumptions as `pdcpl_compute_expansion_exact` on parameters.
 *
 * @param buf Address to buffer
 * @param pos Position in data buffer to write to
 * @param write_size Number of bytes to write
 * @param data Data needed for computation (unused)
 */
static inline ptrdiff_t
pdcpl_compute_expansion_default(
  pdcpl_buffer *buf, const void *pos, size_t write_size, void *data)
{
  // min number of bytes to expand
  ptrdiff_t ex_min = pdcpl_compute_expansion_exact(buf, pos, write_size, data);
  // return BUFSIZ multiple, extra BUFSIZ based on divisibility
  return BUFSIZ * (ex_min / BUFSIZ + !!(ex_min % BUFSIZ));
}

/**
 * Expand buffer if writing `write_size` bytes at `pos` exceeds capacity.
 *
 * The buffer capacity will be expanded by the smallest multiple of `BUFSIZ`
 * bytes needed to fit the bytes that will be written starting at `pos`.
 *
 * @param buf Address to buffer
 * @param pos Position in buffer data to begin writing at
 * @param write_size Number of bytes to write, can spill out of buffer
 * @returns 0 on success, -EINVAL if any pointer args are `NULL`, -ENOMEM if
 *  the call to realloc fails, i.e. not enough memory to realloca
 */
int
pdcpl_buffer_dynexpand(pdcpl_buffer *buf, const void *pos, size_t write_size)
{
  return pdcpl_buffer_dynexpand_custom(
    buf, pos, write_size, pdcpl_compute_expansion_default, NULL
  );
}
