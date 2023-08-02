/**
 * @file variant.c
 * @author Derek Huang
 * @brief C source for a simple variant type
 * @copyright MIT License
 */

#include "pdcpl/variant.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/math.h"

/**
 * Macro templating the body of `pdcpl_variant` scalar init functions.
 *
 * The resulting function returns 0 on success, -EINVAL if `vt` is `NULL`.
 *
 * @param suffix Suffix of the function
 * @param type_ Scalar type
 * @param field Name of the relevant `data` field
 */
#define PDCPL_VARIANT_SCALAR_INIT_DEF(suffix, type_, field) \
  PDCPL_VARIANT_INIT_DECL(suffix, type_) \
  { \
    if (!vt) \
      return -EINVAL; \
    vt->flags = PDCPL_CONCAT(pdcpl_variant_, suffix); \
    vt->data.field = val; \
    return 0; \
  } \
  /* struct forward decl trick to force use of semicolon */ \
  struct PDCPL_VARIANT_INIT(PDCPL_CONCAT(suffix, _struct))

PDCPL_VARIANT_SCALAR_INIT_DEF(char, char, c);
PDCPL_VARIANT_SCALAR_INIT_DEF(int, int, i);
PDCPL_VARIANT_SCALAR_INIT_DEF(uint, unsigned int, u);
PDCPL_VARIANT_SCALAR_INIT_DEF(size, size_t, z);
PDCPL_VARIANT_SCALAR_INIT_DEF(double, double, d);
PDCPL_VARIANT_SCALAR_INIT_DEF(float, float, f);

/**
 * Initialize a `pdcpl_variant` with a string.
 *
 * String contents are copied to a new memory buffer and owned.
 *
 * @param vt Variant to initialize
 * @param val `const char *` to initialize with, can be `NULL`
 * @returns 0 on success, -EINVAL if `vt` is `NULL`, -ENOMEM is `malloc` fails
 */
PDCPL_VARIANT_INIT_DECL(string, const char *)
{
  // variant pointer cannot be NULL
  if (!vt)
    return -EINVAL;
  // set type and ownership. allow incoming string to be NULL
  vt->flags = pdcpl_variant_string | pdcpl_variant_mem_own;
  if (!val)
    return 0;
  // otherwise, malloc. don't forget + 1 for null terminator
  vt->data.s = malloc(strlen(val) + 1);
  // -ENOMEM if malloc fails
  if (!vt->data.s)
    return -ENOMEM;
  // otherwise copy
  strcpy(vt->data.s, val);
  return 0;
}

/**
 * Initialize a `pdcpl_variant` with a string.
 *
 * The string pointer refers to the existing string buffer and is not owned.
 *
 * @param vt Variant to initialize
 * @param val `char *` to copy, can be `NULL`
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_VARIANT_INIT_DECL(string_ref, char *)
{
  if (!vt)
    return -EINVAL;
  vt->flags = pdcpl_variant_string | pdcpl_variant_mem_borrow;
  vt->data.s = val;
  return 0;
}

/**
 * Initialize a `pdcpl_variant` with an arbitrary data buffer.
 *
 * Buffer contents are copied to a new memory buffer and owned.
 *
 * @param vt Variant to initialize
 * @param val Buffer to initialize with
 * @param size Number of bytes in the buffer
 * @returns 0 on success, -EINVAL if `vt` or `buf` are `NULL` or if `size` is
 *  zero, -ENOMEM if `malloc` fails when allocating buffer
 */
PDCPL_VARIANT_INIT_DECL_EX(void, const void *, size_t size)
{
  if (!vt || !val || !size)
    return -EINVAL;
  vt->flags = pdcpl_variant_void | pdcpl_variant_mem_own;
  // -ENOMEM if malloc fails
  vt->data.v.b = malloc(size);
  if (!vt->data.v.b)
    return -ENOMEM;
  // otherwise copy the buffer and size
  memcpy(vt->data.v.b, val, size);
  vt->data.v.z = size;
  return 0;
}

/**
 * Initialize a `pdcpl_variant` with an arbitrary data buffer.
 *
 * The variant data pointer refers to the existing data buffer and is not owned.
 *
 * @param vt Variant to initialize
 * @param val Buffer to point to
 * @param size Number of bytes in the buffer
 * @returns 0 on success, -EINVAL if `vt`, `buf` are `NULL` or if `size` is 0
 */
PDCPL_VARIANT_INIT_DECL_EX(void_ref, void *, size_t size)
{
  if (!vt || !val || !size)
    return -EINVAL;
  vt->flags = pdcpl_variant_void | pdcpl_variant_mem_borrow;
  vt->data.v.b = val;
  vt->data.v.z = size;
  return 0;
}

/**
 * Comparison function for two variants.
 *
 * Determines which of the two sorts lexicographically before the other. If the
 * two variants have different intrinsic types, they are considered tied.
 *
 * @param va First variant
 * @param vb Second variant
 * @returns If `va`, `vb` have the same intrinsic type, < 0 when `va` sorts
 *  before `vb`, > 0 when `vb` sorts before `va`, 0 if `va` and `vb` are tied,
 *  if they have different intrinsic types, or on error.
 */
int
pdcpl_variant_compare(const pdcpl_variant* va, const pdcpl_variant *vb)
{
  // if any pointer is NULL we just treat as tie
  if (!va || !vb)
    return 0;
  // attempt to get shared type. if unequal types, this is zero
  unsigned int vtype = pdcpl_variant_shared_type(va, vb);
  if (!vtype)
    return 0;
  // otherwise, go down the list of types
  switch (vtype) {
    case pdcpl_variant_char:
      return (int) va->data.c - (int) vb->data.c;
    case pdcpl_variant_int:
      return va->data.i - vb->data.i;
    // avoid sign problems with unsigned types by manually comparing
    case pdcpl_variant_uint:
      if (va->data.u < vb->data.u)
        return -1;
      if (va->data.u > vb->data.u)
        return 1;
      return 0;
    case pdcpl_variant_size:
      if (va->data.z < vb->data.z)
        return -1;
      if (va->data.z > vb->data.z)
        return 1;
      return 0;
    // use epsilon-based comparison functions for floats
    case pdcpl_variant_double:
      if (pdcpl_double_lt(va->data.d, vb->data.d))
        return -1;
      if (pdcpl_double_gt(va->data.d, vb->data.d))
        return 1;
      return 0;
    case pdcpl_variant_float:
      if (pdcpl_float_lt(va->data.f, vb->data.f))
        return -1;
      if (pdcpl_float_gt(va->data.f, vb->data.f))
        return 1;
      return 0;
    // use strcmp for strings
    case pdcpl_variant_string:
      return strcmp(va->data.s, vb->data.s);
    case pdcpl_variant_void:
      // if sizes are different, shorter buffer goes first
      if (va->data.v.z < vb->data.v.z)
        return -1;
      if (va->data.v.z > vb->data.v.z)
        return 1;
      // use memcmp for buffers if sizes are the same
      return memcmp(va->data.v.b, vb->data.v.b, va->data.v.z);
  }
  // unknown enum value, so just return zero
  return 0;
}

/**
 * Free a `pdcpl_variant`.
 *
 * @param vt Variant to free
 * @returns 0 on success, -EINVAL if `vt` is `NULL` or if flags indicate memory
 *  ownership but no type flag has been set to indicate how to free
 */
int
pdcpl_variant_free(pdcpl_variant *vt)
{
  if (!vt)
    return -EINVAL;
  // only need to free memory if it is owned
  if (vt->flags & pdcpl_variant_mem_own) {
    // free owned string
    if (vt->flags & pdcpl_variant_string) {
      free(vt->data.s);
      goto success;
    }
    // free void * buffer
    else if (vt->flags & pdcpl_variant_void) {
      free(vt->data.v.b);
      goto success;
    }
    // error, so return -EINVAL
    return -EINVAL;
  }
  // otherwise, can just return
success:
  return 0;
}
