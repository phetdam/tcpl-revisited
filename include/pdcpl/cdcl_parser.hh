/**
 * @file cdcl_parser.h
 * @author Derek Huang
 * @brief C++ parser for simplified C declarations
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_PARSER_H_
#define PDCPL_CDCL_PARSER_H_

#include <filesystem>
#include <memory>
#include <string>

#include "pdcpl/dllexport.h"
#include "pdcpl/warnings.h"

namespace pdcpl {

// forward declaration for the implementation class
class cdcl_parser_impl;

/**
 * Parse driver class for parsing C declarations.
 *
 * PIMPL is used to keep the Flex + Bison generated implementation internal to
 * provide a stable ABI, which is more important on Windows than Linux.
 */
class PDCPL_BCDP_PUBLIC cdcl_parser {
public:
  /**
   * Ctor.
   */
  cdcl_parser();

  /**
   * Dtor.
   */
  ~cdcl_parser();

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
  bool parse(const std::filesystem::path& input_file, bool enable_trace = false)
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
  bool parse(
    const std::filesystem::path& input_file,
    bool trace_lexer,
    bool trace_parser);

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
  auto operator()(
    const std::filesystem::path& input_file, bool enable_trace = false)
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
    const std::filesystem::path& input_file,
    bool trace_lexer,
    bool trace_parser)
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
  std::unique_ptr<cdcl_parser_impl> impl_;
PDCPL_MSVC_WARNING_ENABLE()
};

}  // namespace pdcpl

#endif  // PDCPL_CDCL_PARSER_H_
