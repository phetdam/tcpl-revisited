/**
 * @file 1.10.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-10
 * @copyright MIT License
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Copy stdin to stdout but escape tabs, backspaces, and backslashes.\n"
  "\n"
  "That is, tabs are replaced with \\t, backspaces are replaced with \\b,\n"
  "and backslashes are replaced with \\\\. For example, if one piped\n"
  "`printf \"hello\\tto the  \\b.\\\\hack\\\\slash\\n\"' into this program,\n"
  "the output would be printed verbatim except for the newline, i.e.\n"
  "\n"
  "  hello\\tto the  \\b.\\\\hack\\\\slash"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_OPTS();
  // getchar() output
  int c;
  // loop for each character
  while ((c = getchar()) != EOF) {
    // print escaped string for tab, backspace, backslash
    if (c == '\t' || c == '\b' || c == '\\')
      printf("%s", pdcpl_stresc(c));
    else
      putchar(c);
  }
  return EXIT_SUCCESS;
}
