/*
 * parser.y
 *
 * Author: Derek Huang
 * Summary: Bison input for pdcpl C declaration parser
 * Copyright: MIT License
 */

%{
  // on Windows, any header that transitively includes limits.h causes MSVC to
  // emit C4668, warning that __STDC_WANT_SECURE_LIB__ is not defined and has
  // been replaced with 0. corecrt.h defines __STDC_WANT_SECURE_LIB__ to 1
  #ifdef _WIN32
  #include <corecrt.h>
  #endif  // _WIN32

  #include <sstream>
  #include <string>

  #include "dcl_parser_impl.h"
%}

/* C++ LR parser using variants handling complete symbols with error reporting.
 *
 * Location tracking is enabled and as recommended by Bison documentation, the
 * parser's parse() function takes the pdcpl_bcdp dcl_parser as a parameter.
 *
 * Requiring Bison 3.2 stops unnecessary stack.hh generation. For Bison 3.6+,
 * it is better for parse.error to have the value of detailed. Lookahead
 * correction enabled for more accurate error reporting of location. The
 * api.location.file %define is used to prevent location.hh generation.
 *
 * TODO: In the future, we may need to share the location type. Some decision
 * needs to be made on how it will be namespaced and what the include path is.
 */
%require "3.2"
%language "c++"
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose
%define parse.lac full
%define parse.trace
%locations
%define api.location.file none
/* parser class is yy::dcl_parser, not the usual yy::parser */
%define api.parser.class { dcl_parser }
%param { pdcpl::dcl_parser_impl& parser }

/* Token definitions */
%token STAR "*"
%token LPAREN "("
%token RPAREN ")"
%token LANGLE "["
%token RANGLE "]"
%token COMMA ","
%token SEMICOLON ";"
/* don't need digits' numerical value, so semantic type is std::string */
%token <std::string> DIGITS
%token <std::string> IDEN
/* storage class specification */
%token <std::string> STORAGE_SPEC
/* type specifiers */
%token T_VOID "void"
%token T_CHAR "char"
%token T_INT "int"
%token T_DOUBLE "double"
%token T_FLOAT "float"
%token T_ENUM "enum"
%token T_STRUCT "struct"
/* type length specifiers */
%token L_SHORT "short"
%token L_LONG "long"
/* sign specifiers */
%token S_SIGNED "signed"
%token S_UNSIGNED "unsigned"
/* type qualifiers */
%token Q_CONST "const"
%token Q_VOLATILE "volatile"
/* variadic args specifier */
%token T_VARIADIC "..."

%%

/* Input rule */
input:
  %empty
| input stmt

/* Statement rule.
 *
 * Currently this only supports C declarations.
 */
stmt:
  ";"
| decln

/* C declaration rule.
 *
 * We follow a modified version of the declaration specification in Appendix A,
 * section 8 of The C Programming Language, as we do not allow definitions.
 * That is, the init_declr (init-declarator) rule does not allow initializers.
 * The declaration rule alow only allows one declaration specifier.
 */
decln:
  decl_spec init_dclrs ";"

/* C declaration specifier rule. */
decl_spec:
  storage_spec qual_type_spec

/* C storage class specifier rule.
 *
 * If empty, then the storage class is assumed to be auto. Note that unlike as
 * in The C Programming Language, we don't use typedef as a specifier.
 */
storage_spec:
  %empty
| STORAGE_SPEC

/* C qualified type specifier rule.
 *
 * Like with actual C, we can have const T, T const, or just T for a type T.
 */
qual_type_spec:
  type_spec
| type_qual type_spec
| type_spec type_qual

/* C type qualifier rule */
type_qual:
  "const"
| "volatile"

/* C type specifier rule.
 *
 * As in actual C, "unsigned", "unsigned int" can be parsed to unsigned int,
 * and "long", "long int", and "signed long int" can be parsed to long int.
 */
type_spec:
  "void"
| "char"
| sign_spec "char"
| sign_spec implied_int
| length_spec implied_int
| sign_spec length_spec implied_int
| "double"
| "long" "double"
| "float"
| "struct" IDEN
| "enum" IDEN
| IDEN

implied_int:
  %empty
| "int"

/* C sign specifier */
sign_spec:
  "signed"
| "unsigned"

/* C length specifier.
 *
 * For simplicity, long long is not supported.
 */
length_spec:
  "short"
| "long"

/* C init declarators rule */
init_dclrs:
  init_dclr
| init_dclrs "," init_dclr

/* C init declarator.
 *
 * For simplicity, this does not support initialization statements.
 */
init_dclr:
  dclr

/* C declarator.
 *
 * This handles are the pointer specifiers before parsing direct declarator.
 */
dclr:
  ptr_specs dir_declr

/* C pointer specifiers */
ptr_specs:
  %empty
| ptr_specs ptr_spec

/* C pointer specifier */
ptr_spec:
  "*" type_qual

/* C direct declarator.
 *
 * Note that this recurses into the dclr rule when extra parentheses are seen.
 */
dir_declr:
  IDEN
| "(" dclr ")"
| dir_declr array_spec
| dir_declr "(" maybe_param_specs ")"

/* C array specifier.
 *
 * This has been simplified so that instead of a constant expression, only
 * digits can be used as the argument between the brackets.
 */
array_spec:
  "[" "]"
| "[" DIGITS "]"

/* C optional parameter specifiers.
 *
 * Supports empty or void for no parameters, a normal list of parameter
 * specifiers, and a list of parameter specifiers with variadic arguments.
 */
maybe_param_specs:
  %empty
| param_specs
| param_specs "," "..."

/* C parameter specifiers */
param_specs:
  param_spec
| param_specs "," param_spec

/* C parameter specifier.
 *
 * TODO: Need to define something like direct-abstract-declarator as The C
 * Programming Language book has, i.e. a `dir_declr` and `dclr` that don't
 * resolve into an identifier, as otherwise we can't support the full syntax.
 */
param_spec:
  decl_spec
| decl_spec dclr

%%

namespace yy {

/**
 * User-defined error handler.
 *
 * The parse driver's last error member is updated with location + error.
 *
 * @param loc Bison error location
 * @param msg Bison exception error message
 */
void dcl_parser::error(const location& loc, const std::string& msg)
{
  std::stringstream ss;
  ss << loc << ": " << msg;
  parser.last_error_ = ss.str();
}

}  // namespace yy
