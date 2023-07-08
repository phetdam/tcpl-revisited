/**
 * @file 1.19.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-19
 * @copyright MIT License
 */

#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints input line from stdin in reverse to stdout.\n"
  "\n"
  "Lines up to SIZE_MAX - 1 in length can be read as we need space for the\n"
  "null terminator. If a line longer than SIZE_MAX - 1 is read, the\n"
  "pdcpl_getline function used to read the line will correctly return\n"
  "-ERANGE and clean up after itself before program exit.\n"
  "\n"
  "To provide a glimpse of possible program output, if the top-level\n"
  "CMakeLists.txt was used as input, one of lines printed would be\n"
  "\n"
  "  )edulcni/}RID_ECRUOS_TNERRUC_EKAMC{$(seirotcerid_edulcni"
)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // buffers for current and reversed current lines + status
  char *line = NULL, *line_r = NULL;
  int status;
  // go line by line. if line is NULL, no more input
  while (status = pdcpl_getline(stdin, &line, NULL), line) {
    // error if getting or reversing the lines fail
    if (status || pdcpl_strrev(line, &line_r, NULL))
      return EXIT_FAILURE;
    // no more need for the original, print the reversed one + free
    free(line);
    printf("%s\n", line_r);
    free(line_r);
  }
  return EXIT_SUCCESS;
}
