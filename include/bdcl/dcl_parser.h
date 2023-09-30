/**
 * @file dcl_parser.h
 * @author Derek Huang
 * @brief C++ parser for simplified C declarations
 * @copyright MIT License
 */

#ifndef BDCL_DCL_PARSER_H_
#define BDCL_DCL_PARSER_H_

#include <memory>
#include <string>

#include "bdcl/dllexport.h"
#include "pdcpl/warnings.h"

namespace bdcl {

// forward declaration for the implementation class
class dcl_parser_impl;

/**
 * Parse driver class for parsing C declarations.
 *
 * PIMPL is used to keep the Flex + Bison generated implementation internal to
 * provide a stable ABI, which is more important on Windows than Linux.
 */
class BDCL_PUBLIC dcl_parser {
public:
  /**
   * Ctor.
   */
  dcl_parser();

  /**
   * Dtor.
   */
  ~dcl_parser();

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
  bool parse(const std::string& input_file, bool trace_lexer, bool trace_parser);

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

  /**
   * Return last error encountered during parsing.
   */
  const std::string& last_error() const noexcept;

private:
  // MSVC emits C4251 since STL types are not exported. not our problem however
PDCPL_MSVC_WARNING_DISABLE(4251)
  std::unique_ptr<dcl_parser_impl> impl_;
PDCPL_MSVC_WARNING_ENABLE()
};

}  // namespace bdcl

#endif  // BDCL_DCL_PARSER_H_
