/**
 * @file 1.16.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-16
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints the longest line read from stdin and its length to stdout.\n"
  "\n"
  "Lines up to SIZE_MAX - 1 in length can be read as we need space for the\n"
  "null terminator. However, if a line longer than SIZE_MAX - 1 is in fact\n"
  "encountered, the function used to read the lines, pdcpl_getline, will\n"
  "correctly return -ERANGE and clean up after itself.\n"
  "\n"
  "To provide an example of program output, if the top-level CMakeLists.txt\n"
  "was used as input to this program, the output would be\n"
  "\n"
  "  # set some system information variables used for the version info. note "
    "the\n"
  "  75 chars"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // size of current line read, size of max line read
  size_t cur_len, max_len = 0;
  // buffer for current and max line
  char *cur_line, *max_line = NULL;
  // keep going until we reach EOF
  do {
    // read line from stdin, setting buffer address and line length
    if (pdcpl_getline(stdin, &cur_line, &cur_len))
      return EXIT_FAILURE;
    // if line is longer than max line, instead of copying, exchange pointers
    // and make cur_line NULL so free() is a no-op. then, update max_len
    if (cur_len > max_len) {
      max_line = cur_line;
      cur_line = NULL;
      max_len = cur_len;
    }
    // free current line, otherwise we leak (no-op if cur_line NULL)
    free(cur_line);
  }
  while (!feof(stdin));
  // print out the max line (max_line NULL otherwise), free out of habit
  if (max_line)
    printf("%s\n%zu chars\n", max_line, max_len);
  free(max_line);
  return EXIT_SUCCESS;
}
