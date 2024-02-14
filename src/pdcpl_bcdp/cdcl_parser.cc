/**
 * @file cdcl_parser.cc
 * @author Derek Huang
 * @brief C++ parser for simplified C declarations
 * @copyright MIT License
 */

#include "pdcpl/cdcl_parser.hh"

#include <cstddef>
#include <filesystem>
#include <string>

#include "cdcl_parser_impl.hh"

namespace pdcpl {

/**
 * Ctor.
 *
 * Simply default-constructs a new `cdcl_parser_impl`.
 */
cdcl_parser::cdcl_parser() : impl_{new cdcl_parser_impl} {}

/**
 * Dtor.
 */
cdcl_parser::~cdcl_parser() = default;

/**
 * Parse the specified input file.
 *
 * @param input_file File to read input from, empty or "-" for `stdin`
 * @param trace_lexer `true` to enable lexer tracing
 * @param trace_parser `true` to enable parser tracing
 * @returns `true` on success, `false` on failure
 */
bool cdcl_parser::parse(
  const std::filesystem::path& input_file, bool trace_lexer, bool trace_parser)
{
  return impl_->parse(input_file, trace_lexer, trace_parser);
}

/**
 * Return last error encountered during parsing.
 */
const std::string& cdcl_parser::last_error() const noexcept
{
  return impl_->last_error();
}

/**
 * Return ordered vector of declarations.
 *
 * The declarations are ordered by the order in which they were parsed.
 */
const cdcl_parser::results_type& cdcl_parser::results() const noexcept
{
  return impl_->results();
}

/**
 * Return number of parsed declarations.
 */
std::size_t cdcl_parser::n_results() const noexcept
{
  return impl_->n_results();
}

/**
 * Return `true` if the results have a declaration with the given identifier.
 *
 * @param iden Identifier to find matching C declaration for
 */
bool cdcl_parser::results_contain(const std::string& iden) const
{
  return impl_->results_contain(iden);
}

}  // namespace pdcpl
