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
  #include <stdexcept>
  #include <string>
  #include <utility>
  #include <vector>

  #include "cdcl_parser_impl.hh"
  #include "pdcpl/cdcl_type_spec.hh"

  namespace pdcpl {

  }  // namespace pdcpl
%}

/* C++ LR parser using variants handling complete symbols with error reporting.
 *
 * Location tracking is enabled and as recommended by Bison documentation, the
 * parser's parse() function takes the pdcpl_bcdp cdcl_parser as a parameter.
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
/* parser class is yy::cdcl_parser, not the usual yy::parser */
%define api.parser.class { cdcl_parser }
%param { pdcpl::cdcl_parser_impl& parser }

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
/* storage class specifiers */
%token ST_AUTO "auto"
%token ST_EXTERN "extern"
%token ST_REGISTER "register"
%token ST_STATIC "static"
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

/* Nonterminal token definitions */
/* %nterm <std::vector<pdcpl::dcl_parser_dclr>> init_dclrs */
/* %nterm <pdcpl::dcl_parser_dclr> init_dclr */
%nterm <pdcpl::cdcl_type_spec> type_spec
%nterm <pdcpl::cdcl_qual> type_qual
%nterm <pdcpl::cdcl_qtype_spec> qual_type_spec
%nterm <pdcpl::cdcl_storage> storage_spec

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
| dcln

/* C declaration rule.
 *
 * We follow a modified version of the declaration specification in Appendix A,
 * section 8 of The C Programming Language, as we do not allow definitions.
 * That is, the init_declr (init-declarator) rule does not allow initializers.
 * The declaration rule alow only allows one declaration specifier.
 */
dcln:
  decl_spec init_dclrs ";"  /* { parser.insert($1, $2); } */

/* C declaration specifier rule. */
decl_spec:
  storage_spec qual_type_spec

/* C storage class specifier rule.
 *
 * If empty, then the storage class is assumed to be auto. Note that unlike as
 * in The C Programming Language, we don't use typedef as a specifier.
 */
storage_spec:
  %empty        { $$ = pdcpl::cdcl_storage::st_auto; }
| "auto"        { $$ = pdcpl::cdcl_storage::st_auto; }
| "extern"      { $$ = pdcpl::cdcl_storage::st_extern; }
| "register"    { $$ = pdcpl::cdcl_storage::st_register; }
| "static"      { $$ = pdcpl::cdcl_storage::st_static; }

/* C qualified type specifier rule.
 *
 * Like with actual C, we can have const T, T const, or just T for a type T.
 */
qual_type_spec:
  type_spec              { $$ = {$1}; }
| type_qual type_spec    { $$ = {$1, $2}; }
| type_spec type_qual    { $$ = {$2, $1}; }

/* C type qualifier rule */
type_qual:
  "const"       { $$ = pdcpl::cdcl_qual::qconst; }
| "volatile"    { $$ = pdcpl::cdcl_qual::qvolatile; }

/* C type specifier rule.
 *
 * As in actual C, "unsigned", "unsigned int" can be parsed to unsigned int,
 * and "long", "long int", and "signed long int" can be parsed to long int.
 */
type_spec:
  "void"                            { $$ = pdcpl::cdcl_type::gvoid; }
| "char"                            { $$ = pdcpl::cdcl_type::gchar; }
| "signed" "char"                   { $$ = pdcpl::cdcl_type::schar; }
| "unsigned" "char"                 { $$ = pdcpl::cdcl_type::uchar; }
| "signed" implied_int              { $$ = pdcpl::cdcl_type::sint; }
| "unsigned" implied_int            { $$ = pdcpl::cdcl_type::uint; }
| "short" implied_int               { $$ = pdcpl::cdcl_type::sshort; }
| "long" implied_int                { $$ = pdcpl::cdcl_type::slong; }
| "signed" "short" implied_int      { $$ = pdcpl::cdcl_type::sshort; }
| "unsigned" "short" implied_int    { $$ = pdcpl::cdcl_type::ushort; }
| "signed" "long" implied_int       { $$ = pdcpl::cdcl_type::slong; }
| "unsigned" "long" implied_int     { $$ = pdcpl::cdcl_type::ulong; }
| "int"                             { $$ = pdcpl::cdcl_type::sint; }
| "double"                          { $$ = pdcpl::cdcl_type::gdouble; }
| "long" "double"                   { $$ = pdcpl::cdcl_type::gldouble; }
| "float"                           { $$ = pdcpl::cdcl_type::gfloat; }
| "struct" IDEN                     { $$ = {pdcpl::cdcl_type::gstruct, $2}; }
| "enum" IDEN                       { $$ = {pdcpl::cdcl_type::genum, $2}; }
| IDEN                              { $$ = {pdcpl::cdcl_type::gtype, $1}; }

/* "Implied" int.
 *
 * Supports the C practice of defaulting to int as a type.
 */
implied_int:
  %empty
| "int"

/* C init declarators rule */
init_dclrs:
  init_dclr                 /* { $$ = {$1}; } */
| init_dclrs "," init_dclr  /* { $$.emplace_back($3); } */

/* C init declarator.
 *
 * For simplicity, this does not support initialization statements.
 */
init_dclr:
  dclr  /* { $$ = std::move($1); } */

/* C declarator.
 *
 * This handles the pointer specifiers before parsing direct declarator.
 */
dclr:
  maybe_ptr_specs dir_declr

/* C optional pointer specifiers */
maybe_ptr_specs:
  %empty
| ptr_specs

/* C pointer specifiers */
ptr_specs:
  ptr_spec
| ptr_specs ptr_spec

/* C pointer specifier */
ptr_spec:
  "*"
| "*" type_qual

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
 * Supports an empty parameter list for pre-ANSI C unspecified parameters.
 */
maybe_param_specs:
  %empty
| param_specs

/* C parameter specifiers.
 *
 * Supports variadic arguments.
 */
param_specs:
  param_spec
| param_specs "," param_spec
| param_specs "," "..."

/* C parameter specifier.
 *
 * Each parameter specifier is a qualified type specifier with or without a
 * concrete or abstract declarator. Abstract declarators have no identifier.
 */
param_spec:
  qual_type_spec
| qual_type_spec dclr
| qual_type_spec a_dclr

/* C abstract declarator.
 *
 * The main difference from the [concrete] declarator is that the abstract
 * declarator does not contain an identifier.
 */
a_dclr:
  ptr_specs
| maybe_ptr_specs a_dir_dclr

/* C direct abstract declarator.
 *
 * Similar to the direct declarator, but again does not resolve to identifier.
 */
a_dir_dclr:
  "(" a_dclr ")"
| array_spec
| a_dir_dclr array_spec
| "(" maybe_param_specs ")"
| a_dir_dclr "(" maybe_param_specs ")"

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
void cdcl_parser::error(const location& loc, const std::string& msg)
{
  std::stringstream ss;
  ss << loc << ": " << msg;
  parser.last_error_ = ss.str();
}

}  // namespace yy
