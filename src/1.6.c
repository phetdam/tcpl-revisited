/**
 * @file 1.6.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-6
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Verifies that getchar() != EOF is either 0 or 1.\n"
  "\n"
  "Reads characters from stdin until EOF is detected and prints the value of\n"
  "getchar() != EOF for each character, including the final EOF.\n"
  "\n"
  "Note that echo appends an extra newline to its output, so if one piped the\n"
  "output of `echo hello' to this program, one would get output like\n"
  "\n"
  "  getchar() != EOF (1) 'h'\n"
  "  getchar() != EOF (1) 'e'\n"
  "  getchar() != EOF (1) 'l'\n"
  "  getchar() != EOF (1) 'l'\n"
  "  getchar() != EOF (1) 'o'\n"
  "  getchar() != EOF (1) '\\n'\n"
  "  getchar() != EOF (0) 'EOF'"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // char value (or EOF), value of pseudo getchar() != EOF
  int c, neof;
  // escaped string representation of c if it is an escape char or EOF
  const char *c_esc;
  do {
    printf("getchar() != EOF (%d) ", neof = ((c = getchar()) != EOF));
    // MSVC complains about assignment in conditional, so we add an explicit
    // != NULL. for GCC, the extra parentheses are enough to make it happy.
    if ((c_esc = pdcpl_stresc(c)) != NULL)
      printf("'%s'\n", c_esc);
    else
      printf("'%c'\n", c);
  }
  while (neof);
  return EXIT_SUCCESS;
}
