/**
 * @file sa.h
 * @author Derek Huang
 * @brief C macros for simple SAL-like source annotation
 * @copyright MIT License
 */

#ifndef PDCPL_SA_H_
#define PDCPL_SA_H_

/**
 * These annotation macros are inspired by Microsoft's Source Annotation
 * Language (SAL), which provides additional information through the use of
 * macros in function declarations. One benefit is the easy disambiguation of
 * whether a pointer argument is intended for input or output, and whether or
 * not the input/output pointer is allowed to be `NULL`.
 */

/**
 * Main annotation macro.
 *
 * Typical invocation is as follows.
 *
 * `PDCPL_SA(In)`
 *    Indicates a pointer argument is an input and must be non-`NULL`.
 *
 * `PDCPL_SA(Opt(In))`
 *    Indicates a pointer argument is an input but can be `NULL`.
 *
 * `PDCPL_SA(Out)`
 *    Indicates a pointer argument is an output and must be non-`NULL`.
 *
 * `PDCPL_SA(Opt(Out))`
 *    Indicates a pointer argument is an output but can be `NULL`.
 *
 * `PDCPL_SA(In_Out)`
 *    Indicates a pointer argument is input and output. Must be non-`NULL`.
 */
#define PDCPL_SA(...)

#endif  // PDCPL_SA_H_
