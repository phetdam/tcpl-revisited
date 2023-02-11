/**
 * @file 1.12.c
 * @author Derek Huang
 * @brief The C Programming Language execise 1-12
 * @copyright MIT License
 */

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints stdin to stdout one word per line, ignoring extra whitespace.\n"
  "\n"
  "For example, if `printf \"hello from  \\n   the other\\nside  \\n\\n\\n'\"\n"
  "were piped into this program, the resulting output would be\n"
  "\n"
  "  hello\n"
  "  from\n"
  "  the\n"
  "  other\n"
  "  side"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // getchar() output, number of words counted
  int c;
  size_t n_words = 0;
  // word in/out indicator and indicator for if we just exited a word. this
  // allows us to not print newlines for each whitespace.
  bool in_word = false, just_exited = false;
  while ((c = getchar()) != EOF) {
    if (!isspace(c))
      putchar(c);
    PDCPL_WC_CHECK_WORD(c, in_word, n_words);
    // after macro call, in_word is either true or false. if true (in word),
    // we set just_exited to false. if false and we just exited, print newline
    // and then update just_exited so we don't print extra
    if (in_word)
      just_exited = false;
    // no need to test !in_word, in_word is false already in this branch
    else if (!just_exited) {
      putchar('\n');
      just_exited = true;
    }
  }
  return EXIT_SUCCESS;
}
