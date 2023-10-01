/**
 * @file dcl_parser.cc
 * @author Derek Huang
 * @brief C++ parser for simplified C declarations
 * @copyright MIT License
 */

#include "pdcpl/dcl_parser.hh"

#include "dcl_parser_impl.hh"

namespace pdcpl {

/**
 * Ctor.
 *
 * Simply default-constructs a new `dcl_parser_impl`.
 */
dcl_parser::dcl_parser() : impl_{new dcl_parser_impl} {}

/**
 * Dtor.
 */
dcl_parser::~dcl_parser() = default;

/**
 * Parse the specified input file.
 *
 * @param input_file File to read input from, empty or "-" for `stdin`
 * @param trace_lexer `true` to enable lexer tracing
 * @param trace_parser `true` to enable parser tracing
 * @returns `true` on success, `false` on failure
 */
bool dcl_parser::parse(
  const std::string& input_file, bool trace_lexer, bool trace_parser)
{
  return impl_->parse(input_file, trace_lexer, trace_parser);
}

/**
 * Return last error encountered during parsing.
 */
const std::string& dcl_parser::last_error() const noexcept
{
  return impl_->last_error();
}

}  // namespace pdcpl
