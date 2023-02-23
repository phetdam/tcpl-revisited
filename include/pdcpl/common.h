/**
 * @file common.h
 * @author Derek Huang
 * @brief C/C++ header for common macros
 * @copyright MIT License
 */

#ifndef PDCPL_COMMON_H_
#define PDCPL_COMMON_H_

// extern "C" scoping macros
#ifdef __cplusplus
#define PDCPL_EXTERN_C_BEGIN extern "C" {
#define PDCPL_EXTERN_C_END }
#else
#define PDCPL_EXTERN_C_BEGIN
#define PDCPL_EXTERN_C_END
#endif  // __cplusplus

// stringification macros
#define PDCPL_STRINGIFY_I(x) #x
#define PDCPL_STRINGIFY(x) PDCPL_STRINGIFY_I(x)

// concatenation macros
#define PDCPL_CONCAT_I(x, y) x ## y
#define PDCPL_CONCAT(x, y) PDCPL_CONCAT_I(x, y)

// allow C++-like use of inline in C code
#ifdef __cplusplus
#define PDCPL_INLINE inline
#else
#define PDCPL_INLINE static inline
#endif  // __cplusplus

// macro to indicate extern inline, i.e. never static inline
#define PDCPL_EXTERN_INLINE inline

#endif  // PDCPL_COMMON_H_
