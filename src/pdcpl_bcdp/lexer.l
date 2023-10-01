/*
 * lexer.l
 *
 * Author: Derek Huang
 * Summary: Flex input for pdcpl C declaration lexer
 * Copyright: MIT License
 */

/*
 * Lexer will not be used interactively, but in order to work with C++ Bison
 * parser, is non-reentrant. No input() or yyunput() functions required as well
 * since non-interactive. Debug enabled to allow tracing.
 */
%option noinput nounput never-interactive debug

/*
 * Additional includes at top of file to make MSVC stop emitting warnings.
 *
 * Include `cstdint` to prevent Flex from defining its own `stdint.h` macros,
 * which causes MSVC to spit many warnings about macro redefinition.
 *
 * NOTE: In case any header transitively includes `limits.h` and MSVC emits the
 * warning that the macro `__STDC_WANT_SECURE_LIB__` is not defined and has
 * been replaced with 0, first include `corecrt.h`, which defines it to 1.
 */
%top{
  #ifdef _WIN32
  #include <cstdint>
  #endif  // _WIN32
}

%{
  #include <cerrno>
  #include <cstdio>
  #include <cstring>
  #include <string>

  #include "dcl_parser_impl.h"

  /**
   * User-defined action run after token match but before rule action.
   *
   * We use this to advance the location reference's end position columns by
   * the length of the matched token for correct location tracking.
   *
   * @note Must be a complete semicolon-terminated statement or braced block.
   */
  #define YY_USER_ACTION loc.columns(yyleng);

  namespace pdcpl {

  /**
   * Throw `yy::dcl_parser::syntax_error` given location and bad token.
   *
   * @param loc Error location
   * @param token Unknown token text
   */
  [[noreturn]]
  void unknown_dcl_token_throw(const yy::location& loc, const std::string& token)
  {
    throw yy::dcl_parser::syntax_error{loc, "Unrecognized token '" + token + "'"};
  }

  }  // namespace pdcpl
%}

/* Start conditions */

/* C block comment */
%x C_COMMENT
/* C block comment end condition test */
%x C_COMMENT_END
/* C++ line comment */
%x CXX_COMMENT

/* Token definitions */

/* Numbers */
DIGIT       [0-9]
DIGITS      {DIGIT}+
/* Letters */
LETTER      [a-zA-Z]
LETTERS     {LETTER}+
/* Alphanumerics */
ALNUM       ({DIGIT}|{LETTER})
ALNUMS      {ALNUM}+
/* Identifier */
IDEN        ({LETTER}|_)({ALNUM}|_)+
/*
 * Treat blanks and newlines separately for accurate location tracking. This is
 * because newlines require that the current column is reset and the current
 * line number is incremented. Location is stepped for both.
 */
BLANKS      [ \t\r]+
NEWLINES    \n+

/* Rules */
%%

%{
  // location setup code to run before scanning. PDCPL_BDCL_YYLEX is a friend
  // of the bdcl::dcl_parser_impl class so we can update location_ directly
  auto& loc = parser.location_;
  // move start position onto previous end position
  loc.step();
%}

  /* Ignore C block comments */
"/*"                   BEGIN(C_COMMENT);
  /* Possible end of C block comment */
<C_COMMENT>"*"         BEGIN(C_COMMENT_END);
<C_COMMENT>[^*\n]*
  /* Extra blank lines allowed in C block comment */
<C_COMMENT>\n          loc.lines(yyleng); loc.step();
  /* Matched end of C block comment */
<C_COMMENT_END>"/"     loc.step(); BEGIN(INITIAL);
<C_COMMENT_END>[^/]    BEGIN(C_COMMENT);
  /* Ignore C++ line comments */
"//"                   BEGIN(CXX_COMMENT);
<CXX_COMMENT>[^\n]*
<CXX_COMMENT>\n        loc.lines(yyleng); loc.step(); BEGIN(INITIAL);
  /* Ignore blanks */
{BLANKS}               loc.step();
  /* Ignore newlines */
{NEWLINES}             loc.lines(yyleng); loc.step();
  /* Asterisks */
"*"                    return yy::dcl_parser::make_STAR(loc);
  /* Parentheses */
"("                    return yy::dcl_parser::make_LPAREN(loc);
")"                    return yy::dcl_parser::make_RPAREN(loc);
  /* Angle brackets */
"["                    return yy::dcl_parser::make_LANGLE(loc);
"]"                    return yy::dcl_parser::make_RANGLE(loc);
  /* Comma */
","                    return yy::dcl_parser::make_COMMA(loc);
  /* Semicolon */
";"                    return yy::dcl_parser::make_SEMICOLON(loc);
  /* Digits */
{DIGITS}               return yy::dcl_parser::make_DIGITS(yytext, loc);
  /* Storage specifiers */
"auto"                 return yy::dcl_parser::make_STORAGE_SPEC(yytext, loc);
"extern"               return yy::dcl_parser::make_STORAGE_SPEC(yytext, loc);
"register"             return yy::dcl_parser::make_STORAGE_SPEC(yytext, loc);
"static"               return yy::dcl_parser::make_STORAGE_SPEC(yytext, loc);
  /* Type specifiers */
"void"                 return yy::dcl_parser::make_T_VOID(loc);
"char"                 return yy::dcl_parser::make_T_CHAR(loc);
"int"                  return yy::dcl_parser::make_T_INT(loc);
"double"               return yy::dcl_parser::make_T_DOUBLE(loc);
"float"                return yy::dcl_parser::make_T_FLOAT(loc);
"struct"               return yy::dcl_parser::make_T_STRUCT(loc);
"enum"                 return yy::dcl_parser::make_T_ENUM(loc);
  /* Length specifiers */
"short"                return yy::dcl_parser::make_L_SHORT(loc);
"long"                 return yy::dcl_parser::make_L_LONG(loc);
  /* Sign specifiers */
"signed"               return yy::dcl_parser::make_S_SIGNED(loc);
"unsigned"             return yy::dcl_parser::make_S_UNSIGNED(loc);
  /* Variadic args specifier */
"..."                  return yy::dcl_parser::make_T_VARIADIC(loc);
  /* Identifier */
{IDEN}                 return yy::dcl_parser::make_IDEN(yytext, loc);
  /* Unknown token, so throw. Bison will catch the error. */
.                      pdcpl::unknown_dcl_token_throw(loc, yytext);
  /* With Bison locations turned on make_YYEOF needs to be explicitly used */
<<EOF>>                return yy::dcl_parser::make_YYEOF(loc);

%%

namespace pdcpl {

/**
 * Perform setup for the Flex lexer.
 *
 * @param input_file Input file to read. If empty or "-", `stdin` is used.
 * @param enable_debug `true` to turn on lexer tracing, default `false`
 * @returns `true` on success, `false` on failure and sets `last_error_`
 */
bool dcl_parser_impl::lex_setup(
  const std::string& input_file, bool enable_tracing) noexcept
{
  // enable/disable debugging. yy_flex_debug is not even exported in the
  // Flex-generated lexer.yy.h header, so must set yy_flex_debug here.
  yy_flex_debug = enable_tracing;
  // empty file or "-" to read from stdin, latter is POSIX style
  if (input_file.empty() || input_file == "-")
    yyin = stdin;
  // otherwise, attempt to read from file. handle error
  else if ((yyin = std::fopen(input_file.c_str(), "r")) == nullptr) {
    last_error_ =
      "Error opening " + input_file + ": " + std::string{std::strerror(errno)};
    return false;
  }
  return true;
}

/**
 * Perform cleanup for the Flex lexer.
 *
 * Currently, all this does is close `yyin` unless `yyin` is `stdin`.
 *
 * @param input_file Input file passed to `lex_setup`. Used in error reporting.
 * @returns `true` on success, `false` on failure and sets `last_error_`
 */
bool dcl_parser_impl::lex_cleanup(const std::string& input_file) noexcept
{
  if (yyin != stdin && std::fclose(yyin)) {
    last_error_ =
      "Error closing " + input_file + ": " + std::string{std::strerror(errno)};
    return false;
  }
  return true;
}

}  // namespace pdcpl
