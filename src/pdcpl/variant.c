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
    vt->type = PDCPL_CONCAT(pdcpl_variant_, suffix); \
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
 * The variant will copy the contents of the string to a new buffer in memory.
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
  // set type. allow incoming string to be NULL
  vt->type = pdcpl_variant_string;
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
 * Initialize a `pdcpl_variant` with an arbitrary data buffer.
 *
 * @todo `pdcpl_variant_free` will free the buffer, maybe not a good idea
 *
 * @param vt Variant to initialize
 * @param val Buffer to initialize with
 * @param size Number of bytes in the buffer
 * @returns 0 on success, -EINVAL if `vt` or `buf` are `NULL` or if `size` is
 *  zero,  -ENOMEM if `malloc` fails when allocating buffer
 */
PDCPL_VARIANT_INIT_DECL_EX(void, void *, size_t size)
{
  if (!vt || !val || !size)
    return -EINVAL;
  vt->type = pdcpl_variant_void;
  vt->data.v_b = val;
  vt->data.v_z = size;
  return 0;
}

/**
 * Free a `pdcpl_variant` containing a scalar.
 */
static int
pdcpl_variant_free_scalar(pdcpl_variant * vt)
{
  (void) vt;  // unused
  return 0;
}

/**
 * Free a `pdcpl_variant` containing a string.
 */
static int
pdcpl_variant_free_string(pdcpl_variant *vt)
{
  free(vt->data.s);
  return 0;
}

/**
 * Free a `pdcpl_variant` containing a `void *` buffer.
 */
static int
pdcpl_variant_free_void(pdcpl_variant *vt)
{
  free(vt->data.v_b);
  return 0;
}

/**
 * Free a `pdcpl_variant`.
 *
 * The relevant free functions are indexed from an array.
 *
 * @todo May change implementation to assume scalar if not string or `void *`.
 *
 * @param vt Variant to free
 * @returns 0 on success
 */
int
pdcpl_variant_free(pdcpl_variant *vt)
{
  if (!vt)
    return -EINVAL;
  // free function array
  static const pdcpl_variant_free_function free_funcs[] = {
    pdcpl_variant_free_scalar,  // char
    pdcpl_variant_free_scalar,  // int
    pdcpl_variant_free_scalar,  // unsigned int
    pdcpl_variant_free_scalar,  // size_t
    pdcpl_variant_free_scalar,  // double
    pdcpl_variant_free_scalar,  // float
    pdcpl_variant_free_string,  // char *
    pdcpl_variant_free_void     // void *
  };
  // use enum to index into array
  return free_funcs[vt->type](vt);
}
