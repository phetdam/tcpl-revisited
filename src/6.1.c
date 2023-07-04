/**
 * @file 6.1.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 6-1
 * @copyright MIT License
 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Print input from stdin one word at a time.\n"
  "\n"
  "Here a \"word\" is any space-delimited token readable from the stream.\n"
  "\n"
  "To provide an example of sample output, if the top-level LICENSE file was\n"
  "used as input and the output piped to `head', we would see printed\n"
  "\n"
  "  MIT\n"
  "  License\n"
  "  Copyright\n"
  "  (c)\n"
  "  2023\n"
  "  Derek\n"
  "  Huang\n"
  "  Permission\n"
  "  is\n"
  "  hereby"
)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // buffer to hold word, length of word, status
  char *word;
  size_t word_len;
  int status;
  // loop until we are out of words, in which case word_len is zero
  while (status = pdcpl_getword(stdin, &word, &word_len), word_len) {
    // handle any errors
    if (status) {
      fprintf(stderr, "%s: error: %s\n", PDCPL_PROGRAM_NAME, strerror(-status));
      return EXIT_FAILURE;
    }
    // otherwise, just print on new line and free
    printf("%s\n", word);
    free(word);
  }
  return EXIT_SUCCESS;
}
