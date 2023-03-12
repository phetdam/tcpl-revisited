/**
 * @file 5.13.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 5-13
 * @copyright MIT License
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#define PDCPL_HAS_PROGRAM_OPTIONS
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

static unsigned int lines_target = 10;

/**
 * Action to get the number of trailing lines to print.
 */
static
PDCPL_CLIOPT_ACTION(lines_action)
{
  PDCPL_CLIOPT_ACTION_ARGI_GUARD
  int n = atoi(argv[argi + 1]);
  if (!n)
    return PDCPL_CLIOPT_ERROR_CANT_CONVERT;
  if (n < 0)
    return PDCPL_CLIOPT_ERROR_EXPECTED_POSITIVE;
  lines_target = n;
  return PDCPL_CLIOPT_PARSE_OK;
}

PDCPL_PROGRAM_USAGE_DEF
(
  "A minimal tail clone reading only from stdin.\n"
  "\n"
  "Any line with size less than SIZE_MAX can be consumed properly. The\n"
  "program keeps a rolling window of the last few lines through a block of\n"
  "char *; these are rolled and the unneeded string freed as necessary.\n"
  "\n"
  "Although from an algorithmic analysis perspective a linked list modeling\n"
  "a queue is more efficient, the contiguous block is simpler. It is also\n"
  "possible the contiguous storage is more cache friendly and so faster.\n"
  "\n"
  "Note that input with a trailing newline will count as having an extra\n"
  "empty line. Some additional logic could be added to handle this case."
)

PDCPL_PROGRAM_OPTIONS_DEF
{
  {
    "-n", "--lines",
    "Number of trailing lines to print, default 10",
    1,
    lines_action,
    NULL
  },
  PDCPL_PROGRAM_OPTIONS_END
};

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // lines read + block of lines_target char * we use to store the lines
  size_t n_read = 0;
  char **lines = malloc(lines_target * sizeof *lines);
  if (!lines) {
    PDCPL_PRINT_ERROR("ENOMEM\n");
    return EXIT_FAILURE;
  }
  // get lines, filling the lines buffer + exit on error
  int status;
  for (; !feof(stdin) && n_read < lines_target; n_read++) {
    if ((status = pdcpl_getline(stdin, lines + n_read, NULL))) {
      PDCPL_PRINT_ERROR_EX("%s\n", strerror(status));
      return EXIT_FAILURE;
    }
  }
  // now buffer is full, so rotate for each read (no-op if done reading)
  char *cur_line;
  while (!feof(stdin)) {
    if ((status = pdcpl_getline(stdin, &cur_line, NULL))) {
      PDCPL_PRINT_ERROR_EX("%s\n", strerror(status));
      return EXIT_FAILURE;
    }
    // free unneeded line, rotate lines, insert new line
    free(lines[0]);
    for (size_t i = 1; i < lines_target; i++)
      lines[i - 1] = lines[i];
    lines[lines_target - 1] = cur_line;
  }
  // done, so print + free all the lines before freeing the char * block. since
  // n_read can be less than lines_target, just use the minimum
  size_t n_write = (n_read < lines_target) ? n_read : lines_target;
  for (size_t i = 0; i < n_write; i++) {
    printf("%s\n", lines[i]);
    free(lines[i]);
  }
  free(lines);
  return EXIT_SUCCESS;
}
