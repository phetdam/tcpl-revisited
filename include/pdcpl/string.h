/**
 * @file string.h
 * @author Derek Huang
 * @brief C/C++ header for string helpers
 * @copyright MIT License
 */

#ifndef PDCPL_STRING_H_
#define PDCPL_STRING_H_

#include <stdio.h>

/**
 * Return the string associated with the escaped character.
 *
 * If the incoming character is not an escape character, return `NULL`. `EOF`
 * is also handled here and treated as an escape character.
 *
 * @param c Incoming character
 */
static inline const char *
pdcpl_stresc(int c)
{
  switch (c) {
    case '\a':
      return "\\a";
    case '\b':
      return "\\b";
// on Windows, \e is not recognized as an escape character
#ifndef _WIN32
    case '\e':
      return "\\e";
#endif  // _WIN32
    case '\f':
      return "\\f";
    case '\n':
      return "\\n";
    case '\r':
      return "\\r";
    case '\t':
      return "\\t";
    case '\v':
      return "\\v";
    case '\\':
      return "\\\\";
    case '\'':
      return "\\\'";
    case '\"':
      return "\\\"";
    case EOF:
      return "EOF";
    default:
      break;
  }
  return NULL;
}

#endif  // PDCPL_STRING_H_
