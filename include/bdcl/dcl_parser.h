/**
 * @file dcl_parser.h
 * @author Derek Huang
 * @brief C++ parser for simplified C declarations
 * @copyright MIT License
 */

#ifndef BDCL_DCL_PARSER_H_
#define BDCL_DCL_PARSER_H_

#include <string>

#include "pdcpl/warnings.h"

/**
 * Forward declaration to satisfy the `yy::dcl_parser` class definition.
 *
 * The `yy::dcl_parser` takes a `bdcl::dcl_parser&` as a parameter.
 */
namespace bdcl { class dcl_parser; }  // namespace bdcl

/**
 * The C++ header generated by Bison defining the parsing class.
 *
 * We need this in order to use `YY_DECL`, and so this must be included first.
 *
 * MSVC always emits C4065 (switch with default but no case labels) when this
 * header is included in a translation unit, so we disable when including.
 */
PDCPL_MSVC_WARNING_DISABLE(4065)
#include "bdcl/parser.yy.h"
PDCPL_MSVC_WARNING_ENABLE()

/**
 * `YY_DECL` function declaration return type.
 *
 * This is defined separately so the `yylex` friend declaration can use it.
 */
#define BDCL_YYLEX_RETURN yy::dcl_parser::symbol_type

/**
 * `YY_DECL` function name.
 *
 * Usually this can be left as `yylex` unless the Flex name prefix is changed.
 */
#define BDCL_YYLEX yylex

/**
 * `YY_DECL` function declaration arguments.
 *
 * Should be a comma-separated list of function arguments.
 */
#define BDCL_YYLEX_ARGS bdcl::dcl_parser& parser

/**
 * Macro declaring `yylex` in the format the Bison parser expects.
 *
 * The corresponding parser handles complete symbols and uses variant values.
 */
#define YY_DECL BDCL_YYLEX_RETURN BDCL_YYLEX(BDCL_YYLEX_ARGS)

/**
 * `yylex` declaration compatible with C++ Bison parser.
 *
 * There is no need to make this `extern "C"` since the generated Flex lexer
 * is being compiled as C++, not as straight C code.
 */
YY_DECL;

namespace bdcl {

/**
 * Parser driver class for parsing C declarations.
 */
class dcl_parser {
public:

  /**
   * Parse input from `stdin`.
   *
   * @param enable_trace `true` to enable lexer and parser tracing
   * @returns `true` on success, `false` on failure
   */
  bool parse(bool enable_trace = false)
  {
    return parse("", enable_trace);
  }

  /**
   * Parse the specified input file.
   *
   * @param input_file File to read input from, empty or "-" for `stdin`
   * @param enable_trace `true` to enable lexer and parser tracing
   * @returns `true` on success, `false` on failure
   */
  bool parse(const std::string& input_file, bool enable_trace = false)
  {
    return parse(input_file, enable_trace, enable_trace);
  }

  /**
   * Parse the specified input file.
   *
   * @param input_file File to read input from, empty or "-" for `stdin`
   * @param trace_lexer `true` to enable lexer tracing
   * @param trace_parser `true` to enable parser tracing
   * @returns `true` on success, `false` on failure
   */
  bool parse(const std::string& input_file, bool trace_lexer, bool trace_parser)
  {
    // initialize the Bison parser location for location tracking
    location_.initialize(&input_file);
    // perform Flex lexer setup, create Bison parser, set debug level, parse
    if (!lex_setup(input_file, trace_lexer))
      return false;
    yy::dcl_parser parser{*this};
    parser.set_debug_level(trace_parser);
    auto status = parser.parse();
    // perform Flex lexer cleanup + return
    if (!lex_cleanup(input_file))
      return false;
    // last_error_ should already have been set if parsing is failing
    return !status;
  }

  /**
   * Parse input from `stdin`.
   *
   * @param enable_trace `true` to enable lexer and parser tracing
   * @returns `true` on success, `false` on failure
   */
  auto operator()(bool enable_trace = false)
  {
    return parse(enable_trace);
  }

  /**
   * Parse the specified input file.
   *
   * @param input_file File to read input from, empty or "-" for `stdin`
   * @param enable_trace `true` to enable lexer and parser tracing
   * @returns `true` on success, `false` on failure
   */
  auto operator()(const std::string& input_file, bool enable_trace = false)
  {
    return parse(input_file, enable_trace);
  }

  /**
   * Parse the specified input file.
   *
   * @param input_file File to read input from, empty or "-" for `stdin`
   * @param trace_lexer `true` to enable lexer tracing
   * @param trace_parser `true` to enable parser tracing
   * @returns `true` on success, `false` on failure
   */
  auto operator()(
    const std::string& input_file, bool trace_lexer, bool trace_parser)
  {
    return parse(input_file, trace_lexer, trace_parser);
  }

  // allow lexer to access to the parse driver members to update location +
  // error. we use (::BDCL_YYLEX) to tell compiler BDCL_YYLEX is in the global
  // namespace, not in the current enclosing bdcl namespace
  friend BDCL_YYLEX_RETURN (::BDCL_YYLEX)(BDCL_YYLEX_ARGS);
  // allow parser to access parse driver members to update location + error
  friend class yy::dcl_parser;

  /**
   * Return last location of the parser.
   */
  const auto& location() const noexcept { return location_; }

  /**
   * Return last error encountered during parsing.
   */
  const auto& last_error() const noexcept { return last_error_; }

private:
  yy::location location_;
  std::string last_error_;

  /**
   * Perform setup for the Flex lexer.
   *
   * @param input_file Input file to read. If empty or "-", `stdin` is used.
   * @param enable_debug `true` to turn on lexer tracing, default `false`
   * @returns `true` on success, `false` on failure and sets `last_error_`
   */
  bool lex_setup(const std::string& input_file, bool enable_tracing) noexcept;

  /**
   * Perform cleanup for the Flex lexer.
   *
   * Currently, all this does is close `yyin` unless `yyin` is `stdin`.
   *
   * @param input_file Input file passed to `lex_setup`. Used in error reporting.
   * @returns `true` on success, `false` on failure and sets `last_error_`
   */
  bool lex_cleanup(const std::string& input_file) noexcept;
};

}  // namespace bdcl

#endif  // BDCL_DCL_PARSER_H_
