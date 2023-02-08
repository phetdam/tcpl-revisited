/**
 * @file 1.9.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-9
 * @copyright MIT License
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Copy stdin to stdout but replace multiple blanks with a single blank.\n"
  "\n"
  "For example, if `echo \"i    say hello  goodbye\"' were piped into this\n"
  "program, the output printed on screen would be\n"
  "\n"
  "  i say hello goodbye"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // getchar() output + indicator if we saw a blank or not
  int c;
  bool got_blank = false;
  // loop for each character
  while ((c = getchar()) != EOF) {
    // if got a blank, set got_blank to true, otherwise set to false
    if (c == ' ') {
      // if got_blank is already true, don't print
      if (got_blank)
        continue;
      got_blank = true;
    }
    else
      got_blank = false;
    // print the character as usual
    putchar(c);
  }
  return EXIT_SUCCESS;
}
