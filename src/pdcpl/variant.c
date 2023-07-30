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
