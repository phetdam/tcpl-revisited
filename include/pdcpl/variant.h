/**
 * @file variant.h
 * @author Derek Huang
 * @brief C header for a simple variant type
 * @copyright MIT License
 */

#ifndef PDCPL_VARIANT_H_
#define PDCPL_VARIANT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * `pdcpl_variant` field constants.
 *
 * The memory ownership flags indicate whether the `pdcpl_variant_clear` should
 * be responsible for freeing memory buffers required by some types.
 */
enum {
  // available types
  pdcpl_variant_char = 0x1,       // char
  pdcpl_variant_int = 0x2,        // int
  pdcpl_variant_uint = 0x4,       // unsigned int
  pdcpl_variant_size = 0x8,       // size_t
  pdcpl_variant_double = 0x10,    // double
  pdcpl_variant_float = 0x20,     // float
  pdcpl_variant_string = 0x40,    // char *
  pdcpl_variant_void = 0x80,      // void *
  // memory ownership flags
  pdcpl_variant_mem_own = 0x10000,      // memory owned
  pdcpl_variant_mem_borrow = 0x20000    // memory borrowed
};

/**
 * Variant type.
 *
 * The `data` union describes the possible values that can be stored and the
 * `flags` member holds type and ownership information.
 *
 * @todo In the future, this may allow pointer to another `pdcpl_variant`.
 */
typedef struct {
  unsigned int flags;
  union {
    char c;
    int i;
    unsigned int u;
    size_t z;
    double d;
    float f;
    char *s;
    // for void * buffers, need to know the buffer size too
    struct buffer {
      void *b;
      size_t z;
    } v;
  } data;
} pdcpl_variant;

/**
 * Macro expanding into the name for a `pdcpl_variant` init function.
 *
 * @param suffix Suffix of the function, e.g. `uint`, `string`
 */
#define PDCPL_VARIANT_INIT(suffix) \
  PDCPL_CONCAT(pdcpl_variant_init_, suffix)

/**
 * Macro defining the `pdcpl_variant` init function declaration.
 *
 * @param suffix Suffix of the function
 * @param type Type the value the variant will be initialized with
 * @returns 0 on success, negative `errno` value on failure
 */
#define PDCPL_VARIANT_INIT_DECL(suffix, type) \
  int \
  PDCPL_VARIANT_INIT(suffix)(pdcpl_variant *vt, type val)

/**
 * Macro defining an extended `pdcpl_variant` init function declaration.
 *
 * @param suffix Suffix of the function
 * @param type Type the value the variant will be initialized with
 * @param ... Additional argument declarations
 * @returns 0 on success, negative `errno` value on failure
 */
#define PDCPL_VARIANT_INIT_DECL_EX(suffix, type, ...) \
  int \
  PDCPL_VARIANT_INIT(suffix)(pdcpl_variant *vt, type val, __VA_ARGS__)

/**
 * Initialize a `pdcpl_variant` with a `char`.
 *
 * @param vt Variant to initialize
 * @param val `char` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(char, char);

/**
 * Initialize a `pdcpl_variant` with an `int`.
 *
 * @param vt Variant to initialize
 * @param val `int` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(int, int);

/**
 * Initialize a `pdcpl_variant` with an `unsigned int`.
 *
 * @param vt Variant to initialize
 * @param val `unsigned int` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(uint, unsigned int);

/**
 * Initialize a `pdcpl_variant` with a `size_t`.
 *
 * @param vt Variant to initialize
 * @param val `size_t` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(size, size_t);

/**
 * Initialize a `pdcpl_variant` with a `double.
 *
 * @param vt Variant to initialize
 * @param val `double` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(double, double);

/**
 * Initialize a `pdcpl_variant` with a `float`.
 *
 * @param vt Variant to initialize
 * @param val `float` to initialize with
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(float, float);

/**
 * Initialize a `pdcpl_variant` with a string.
 *
 * String contents are copied to a new memory buffer and owned.
 *
 * @param vt Variant to initialize
 * @param val `const char *` to initialize with, can be `NULL`
 * @returns 0 on success, -EINVAL if `vt` is `NULL`, -ENOMEM is `malloc` fails
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(string, const char *);

/**
 * Initialize a `pdcpl_variant` with a string.
 *
 * The string pointer refers to the existing string buffer and is not owned.
 *
 * @param vt Variant to initialize
 * @param val `char *` to point to, can be `NULL`
 * @returns 0 on success, -EINVAL if `vt` is `NULL`
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(string_ref, char *);

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
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL_EX(void, const void *, size_t size);

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
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL_EX(void_ref, void *, size_t size);

/**
 * Check if two variants have the same type.
 *
 * Memory ownership flags are not relevant and are ignored.
 *
 * @param va First variant
 * @param vb Second variant
 * @returns Nonzero value if variants are the same type, otherwise zero
 */
PDCPL_INLINE unsigned int
pdcpl_variant_shared_type(const pdcpl_variant *va, const pdcpl_variant *vb)
{
  return va->flags & vb->flags & ~(pdcpl_variant_mem_own | pdcpl_variant_mem_borrow);
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
PDCPL_PUBLIC
int
pdcpl_variant_compare(const pdcpl_variant* va, const pdcpl_variant *vb);

/**
 * Typedef for `pdcpl_variant` free function.
 *
 * The functions should return 0 on success, negative `errno` value on failure
 */
typedef int (*pdcpl_variant_free_function)(pdcpl_variant *);

/**
 * Free a `pdcpl_variant`.
 *
 * @param vt Variant to free
 * @returns 0 on success, -EINVAL if `vt` is `NULL` or if flags indicate memory
 *  ownership but no type flag has been set to indicate how to free
 */
PDCPL_PUBLIC
int
pdcpl_variant_free(pdcpl_variant *vt);

PDCPL_EXTERN_C_END

#endif  // PDCPL_VARIANT_H_
