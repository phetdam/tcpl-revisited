/**
 * @file memory.h
 * @author Derek Huang
 * @brief C/C++ header for C memory utilities
 * @copyright MIT License
 */

#ifndef PDCPL_MEMORY_H_
#define PDCPL_MEMORY_H_

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Perform a binary operation on two pointers cast to a specific pointer type.
 *
 * @param type Type used in pointer cast, i.e. `p1`, `p2` cast to `type *`
 * @param p1 Pointer one
 * @param op Binary operator, e.g. `-`, `+`, `<`
 * @param p2 Pointer two
 */
#define PDCPL_TYPED_PTR_OP(type, p1, op, p2) ((type *) (p1) op (type *) (p2))

/**
 * Perform a binary operation on two pointers.
 *
 * Both are cast to `char *` for arithmetic operations to be in units of bytes.
 *
 * @param p1 Pointer one
 * @param op Binary operator, e.g. `-`, `+`, `<`
 * @param p2 Pointer two
 */
#define PDCPL_PTR_OP(p1, op, p2) PDCPL_TYPED_PTR_OP(char, p1, op, p2)

// helpers for evaluating expressions involving pointer address comparison
#define PDCPL_PTR_GT(p1, p2) PDCPL_PTR_OP(p1, >, p2)
#define PDCPL_PTR_GE(p1, p2) PDCPL_PTR_OP(p1, >=, p2)
#define PDCPL_PTR_LT(p1, p2) PDCPL_PTR_OP(p1, <, p2)
#define PDCPL_PTR_LE(p1, p2) PDCPL_PTR_OP(p1, <=, p2)

/**
 * Add or subtract a quantity to a pointer cast to a specific pointer type.
 *
 * @param type Type used in pointer cast, i.e. `p` cast to `type *`
 * @param p Pointer
 * @param op Binary operator, either `+` or `-`
 * @param value Integral value to add or subtract to pointer
 */
#define PDCPL_TYPED_PTR_SHIFT(type, p, op, value) ((type *) (p) op (value))

/**
 * Add or subtract a quantity to a pointer.
 *
 * Pointer is cast to `char *` for the shift to be in units of byyes.
 *
 * @param p Pointer
 * @param op Binary operator, either `+` or `-`
 * @param value Integral value to add or subtract to pointer
 */
#define PDCPL_PTR_SHIFT(p, op, value) PDCPL_TYPED_PTR_SHIFT(char, p, op, value)

/**
 * Index into a pointer like it is an array.
 *
 * Saves some typing of parentheses if `p` needs to be cast.
 *
 * @param p Pointer, can be cast to whatever type is necessary
 * @param i Index value
 */
#define PDCPL_INDEX(p, i) (p)[i]

/**
 * Index into a pointer like it is a `char` array.
 *
 * @param p Pointer to cast to `char *`
 * @param i Index value
 */
#define PDCPL_INDEX_CHAR(p, i) PDCPL_INDEX((char *) (p), i)

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
 * Check that a buffer is ready for use.
 *
 * Simply checks if data pointer is not `NULL` and if size is positive.
 *
 * @param buf Address to buffer
 */
PDCPL_INLINE bool
pdcpl_buffer_ready(const pdcpl_buffer *buf)
{
  return (buf->data && buf->size) ? true : false;
}

/**
 * Realloc the buffer's memory and update the buffer size.
 *
 * The buffer is allowed to have `NULL` data pointer.
 *
 * @param buf Address to buffer
 * @param new_size Number of bytes to allocate
 * @returns 0 on success, -EINVAL if `buf` is `NULL` or if `new_size` is zero,
 *  -ENOMEM if `realloc` fails due to lack of heap space
 */
PDCPL_INLINE int
pdcpl_buffer_realloc(pdcpl_buffer *buf, size_t new_size)
{
  // don't allow zero size reallocs
  if (!buf || !new_size)
    return -EINVAL;
  void *new_data = realloc(buf->data, new_size);
  if (!new_data)
    return -ENOMEM;
  buf->data = new_data;
  buf->size = new_size;
  return 0;
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
  buf->data = NULL;
  buf->size = 0;
  return 0;
}

/**
 * Copy the contents of `src` to an uninitialized buffer `dst`.
 *
 * The contents of `src->data` are copied to a new buffer at `dst->data` and
 * `dst->size` takes the value of `src->size` on success. `dst` should not
 * manage any buffer at all--if `dst->data` points to an existing buffer, it is
 * not freed, which would constitute a memory leak.
 *
 * @param src Address to source buffer
 * @param dst Address to uninitialized destination buffer
 * @returns 0 on success, -EINVAL if `src` or `dst` are `NULL` or if `src` is
 *  not a valid source buffer
 */
PDCPL_INLINE int
pdcpl_buffer_copy(const pdcpl_buffer *src, pdcpl_buffer *dst)
{
  // don't allow copying from an uninitialized buffer
  if (!src || !dst || (src && !pdcpl_buffer_ready(src)))
    return -EINVAL;
  // try to allocate new memory for dst
  void *data = malloc(src->size);
  if (!data)
    return -ENOMEM;
  // if successful, copy buffer contents, buffer pointer and size + exit
  memcpy(data, src->data, src->size);
  dst->data = data;
  dst->size = src->size;
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
PDCPL_PUBLIC
int
pdcpl_buffer_dynexpand_custom(
  pdcpl_buffer *buf,
  const void *pos,
  size_t write_size,
  pdcpl_buffer_expansion_computer compute_expansion,
  void *data);

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
PDCPL_PUBLIC
int
pdcpl_buffer_dynexpand_exact(
  pdcpl_buffer *buf, const void *pos, size_t write_size);

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
PDCPL_PUBLIC
int
pdcpl_buffer_dynexpand(pdcpl_buffer *buf, const void *pos, size_t write_size);

PDCPL_EXTERN_C_END

#endif  // PDCPL_MEMORY_H_
