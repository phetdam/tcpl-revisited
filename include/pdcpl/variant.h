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

#include "pdcpl/common.h"
#include "pdcpl/dllexport.h"

PDCPL_EXTERN_C_BEGIN

/**
 * Enum indicating the data containing within the `pdcpl_variant.`
 */
typedef enum {
  pdcpl_variant_char,      // char
  pdcpl_variant_int,       // int
  pdcpl_variant_uint,      // unsigned int
  pdcpl_variant_size,      // size_t
  pdcpl_variant_double,    // double
  pdcpl_variant_float,     // float
  pdcpl_variant_string,    // char *
  pdcpl_variant_void       // void *
} pdcpl_variant_type;

/**
 * Variant type.
 *
 * @todo In the future, this may allow pointer to another `pdcpl_variant`.
 */
typedef struct {
  pdcpl_variant_type type;
  union {
    char c;
    int i;
    unsigned int u;
    size_t z;
    double d;
    float f;
    char *s;
    // for void * buffers, need to know the buffer size too
    struct {
      void *v_b;
      size_t v_z;
    };
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
 * The variant will copy the contents of the string to a new buffer in memory.
 *
 * @param vt Variant to initialize
 * @param val `const char *` to initialize with, can be `NULL`
 * @returns 0 on success, -EINVAL if `vt` is `NULL`, -ENOMEM is `malloc` fails
 */
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL(string, const char *);

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
PDCPL_PUBLIC
PDCPL_VARIANT_INIT_DECL_EX(void, void *, size_t size);

/**
 * Typedef for `pdcpl_variant` free function.
 *
 * The functions should return 0 on success, negative `errno` value on failure
 */
typedef int (*pdcpl_variant_free_function)(pdcpl_variant *);

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
PDCPL_PUBLIC
int
pdcpl_variant_free(pdcpl_variant *vt);

PDCPL_EXTERN_C_END

#endif  // PDCPL_VARIANT_H_
