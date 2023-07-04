/**
 * @file features.h
 * @author Derek Huang
 * @brief C/C++ header for platform feature detection
 * @copyright MIT License
 */

#ifndef PDCPL_FEATURE_H_
#define PDCPL_FEATURE_H_

// test for POSIX features
#ifdef _POSIX_C_SOURCE
// we have some subset of POSIX features
#define PDCPL_POSIX
// if we have POSIX.1-1990 and ISO C (1990) features
#if _POSIX_C_SOURCE >= 1
#define PDCPL_POSIX_1
#endif  // !_POSIX_C_SOURCE < 1
// if we have POSIX.2-1992 features
#if _POSIX_C_SOURCE >= 2
#define PDCPL_POSIX_2
#endif  // !_POSIX_C_SOURCE < 2
#if _POSIX_C_SOURCE >= 199309L
// if we have POSIX.1b (real-time extensions) features
#define PDCPL_POSIX_1_B
#endif  // !_POSIX_C_SOURCE < 199309L
// if we have POSIX.1c (threads) features
#if _POSIX_C_SOURCE >= 199506L
#define PDCPL_POSIX_1_C
#endif  // !_POSIX_C_SOURCE < 199506L
// if we have POSIX.1-2001 features (no XSI extension)
#if _POSIX_C_SOURCE >= 200112L
#define PDCPL_POSIX_1_2001
#endif  // !_POSIX_C_SOURCE < 200112L
// if wehave POSIX.1-2008 features (no XSI extension)
#if _POSIX_C_SOURCE >= 200809L
#define PDCPL_POSIX_1_2008
#endif  // !_POSIX_C_SOURCE < 200809L
#endif  // _POSIX_C_SOURCE

// test for GNU features
#ifdef _GNU_SOURCE
#define PDCPL_GNU
#endif  // _GNU_SOURCE

#endif  // PDCPL_FEATURE_H_
