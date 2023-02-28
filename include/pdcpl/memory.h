/**
 * @file memory.h
 * @author Derek Huang
 * @brief C/C++ header for C memory utilities
 * @copyright MIT License
 */

#ifndef PDCPL_MEMORY_H_
#define PDCPL_MEMORY_H_

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

/**
 * Simple struct for a memory buffer.
 *
 * @param data Pointer to beginning of buffer
 * @param size Number of bytes in buffer
 */
typedef struct {
  void *data;
  size_t size;
} pdcpl_buffer;

/**
 * Create a new buffer struct.
 *
 * If `malloc` fails, the `data` pointer will be `NULL` and the `size` is `0`.
 * If `buf_size` is `0`, then no memory is allocated, and again `data` pointer
 * is `NULL` while the `size` member will be `0`.
 *
 * @param buf_size Number of bytes in buffer
 */
PDCPL_INLINE pdcpl_buffer
pdcpl_buffer_new(size_t buf_size)
{
  pdcpl_buffer buf;
  buf.data = (buf_size) ? malloc(buf_size) : NULL;
  buf.size = (buf.data) ? buf_size : 0;
  return buf;
}

/**
 * Free the memory held by the buffer, set data to `NULL`, reset size to 0.
 *
 * If the buffer `data` is `NULL`, `size` is still set to 0 anyways.
 *
 * @param buf Address to buffer
 * @returns 0 on success, -EINVAL if `buf` is `NULL`
 */
PDCPL_INLINE int
pdcpl_buffer_clear(pdcpl_buffer *buf)
{
  if (!buf)
    return -EINVAL;
  free(buf->data);
  buf->size = 0;
  return 0;
}

/**
 * Expand the buffer by the specified number of bytes.
 *
 * @param buf Address to buffer
 * @param expand_size Number of bytes to expand buffer by
 * @returns 0 on success, -EINVAL if `buf` or its data is `NULL`, -ENOMEM if
 *  the call to `realloc` fails, i.e. not enough memory to realloc
 */
PDCPL_PUBLIC
int
pdcpl_buffer_expand_exact(pdcpl_buffer *buf, size_t expand_size);

/**
 * Expand the buffer by `BUFSIZ`.
 *
 * @param buf Address to buffer
 * @returns 0 on success, -EINVAL if `buf` or its data is `NULL`, -ENOMEM if
 *  the call to `realloc` fails, i.e. not enough memory to realloc
 */
PDCPL_INLINE int
pdcpl_buffer_expand(pdcpl_buffer *buf)
{
  return pdcpl_buffer_expand_exact(buf, BUFSIZ);
}

/**
 * Typedef for functions used to compute buffer expansions for `dynexpand*`.
 *
 * @param buf Address to buffer
 * @param pos Position in buffer data to begin writing at
 * @param write_size Number of bytes to write, can spill out of buffer
 * @param data Additional data used by the function, can be left unused
 */
typedef ptrdiff_t
(*pdcpl_buffer_expansion_computer)(
  pdcpl_buffer *buf, const void *pos, size_t write_size, void *data);

#endif  // PDCPL_MEMORY_H_
