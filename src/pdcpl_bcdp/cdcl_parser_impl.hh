/**
 * @file cdcl_parser_impl.hh
 * @author Derek Huang
 * @brief C++ parser implementation for simplified C declarations
 * @copyright MIT License
 */

#ifndef PDCPL_BCDP_CDCL_PARSER_IMPL_HH_
#define PDCPL_BCDP_CDCL_PARSER_IMPL_HH_

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "pdcpl/cdcl_dcln_spec.hh"
#include "pdcpl/cdcl_type_spec.hh"
#include "pdcpl/warnings.h"

// #include "dcl_parser_dcln.hh"

/**
 * Forward declaration to satisfy the `yy::cdcl_parser` class definition.
 *
 * The `yy::cdcl_parser` takes a `pdcpl::cdcl_parser_impl&` as a parameter.
 */
namespace pdcpl { class cdcl_parser_impl; }  // namespace pdcpl

/**
 * The C++ header generated by Bison defining the parsing class.
 *
 * We need this in order to use `YY_DECL`, and so this must be included first.
 *
 * MSVC always emits C4065 (switch with default but no case labels) when this
 * header is included in a translation unit, so we disable when including.
 *
 * Any required headers should be included before this Bison-generated header.
 */
PDCPL_MSVC_WARNING_DISABLE(4065)
#include "parser.yy.hh"
PDCPL_MSVC_WARNING_ENABLE()

/**
 * `YY_DECL` function declaration return type.
 *
 * This is defined separately so the `yylex` friend declaration can use it.
 */
#define PDCPL_BCDP_YYLEX_RETURN yy::cdcl_parser::symbol_type

/**
 * `YY_DECL` function name.
 *
 * Usually this can be left as `yylex` unless the Flex name prefix is changed.
 */
#define PDCPL_BCDP_YYLEX yylex

/**
 * `YY_DECL` function declaration arguments.
 *
 * Should be a comma-separated list of function arguments.
 */
#define PDCPL_BCDP_YYLEX_ARGS pdcpl::cdcl_parser_impl& parser

/**
 * Macro declaring `yylex` in the format the Bison parser expects.
 *
 * The corresponding parser handles complete symbols and uses variant values.
 */
#define YY_DECL PDCPL_BCDP_YYLEX_RETURN PDCPL_BCDP_YYLEX(PDCPL_BCDP_YYLEX_ARGS)

/**
 * `yylex` declaration compatible with C++ Bison parser.
 *
 * There is no need to make this `extern "C"` since the generated Flex lexer
 * is being compiled as C++, not as straight C code.
 */
YY_DECL;

namespace pdcpl {

/**
 * Parser driver implementation class for parsing C declarations.
 */
class cdcl_parser_impl {
public:
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

  // allow lexer to access to the parse driver members to update location +
  // error. we use (::PDCPL_BCDP_YYLEX) to tell compiler PDCPL_BCDP_YYLEX is in
  // the global namespace, not in the current enclosing pdcpl namespace
  friend PDCPL_BCDP_YYLEX_RETURN (::PDCPL_BCDP_YYLEX)(PDCPL_BCDP_YYLEX_ARGS);
  // allow parser to access parse driver members to update location + error
  friend class yy::cdcl_parser;

  /**
   * Return last location of the parser.
   */
  const auto& location() const noexcept { return location_; }

  /**
   * Return last error encountered during parsing.
   */
  const auto& last_error() const noexcept { return last_error_; }

  /**
   * Return ordered vector of declarations.
   *
   * The declarations are ordered by the order in which they were parsed.
   */
  const auto& results() const noexcept { return results_; }

  /**
   * Return map for looking up a declaration given its identifier.
   *
   * The returned index can be used with `results()` to get the declaration.
   */
  const auto& result_indices() const noexcept { return result_indicies_; }

  /**
   * Insert a new declaration.
   *
   * @param dcl_spec Declaration specifier, e.g. storage and qualified type
   * @param init_dclr Init declarator, currently only supports declarator
   */
  void insert(const cdcl_dcl_spec& dcl_spec, const cdcl_init_dclr& init_dclr)
  {
    // currently only support declarations
    if (!std::holds_alternative<cdcl_dclr>(init_dclr)) {
      last_error_ = "init_dclr only support C declarators";
      throw yy::cdcl_parser::syntax_error{location_, last_error_};
    }
    // create cdcl_dcln C declaration from dcl_spec and dclr
    cdcl_dcln dcln{dcl_spec, std::get<cdcl_dclr>(init_dclr)};
    // must have identifer and must not be duplicate declaration
    const auto& iden = dcln.iden();
    if (iden.empty()) {
      last_error_ = "dcln is missing identifier";
      throw yy::cdcl_parser::syntax_error{location_, last_error_};
    }
    if (result_indicies_.find(iden) != result_indicies_.end()) {
      last_error_ = "identifier " + iden + " redeclared";
      throw yy::cdcl_parser::syntax_error{location_, last_error_};
    }
    // otherwise, we can insert the declaration + update iden -> index map
    results_.push_back(std::move(dcln));
    result_indicies_[iden] = results_.size() - 1;
  }

  /**
   * Insert new declarations from multiple declarators.
   *
   * @param dcl_spec Declaration specifier, e.g. storage and qualified type
   * @param init_dclrs Init declarators, currently only supports declarator
   */
  void insert(const cdcl_dcl_spec& dcl_spec, const cdcl_init_dclrs& init_dclrs)
  {
    for (const auto& init_dclr : init_dclrs)
      insert(dcl_spec, init_dclr);
  }

  /**
   * Return number of parsed declarations.
   */
  auto n_results() const noexcept { return results_.size(); }

  /**
   * Return `true` if the results have a declaration with the given identifier.
   *
   * @param iden Identifier to find matching C declaration for
   */
  auto results_contain(const std::string& iden) const
  {
    return result_indicies_.find(iden) != result_indicies_.end();
  }

  /**
   * Look up a declaration object with the given identifier.
   *
   * An exception will be thrown if no matching declaration is found. Use the
   * `results_contain()` member to check if a matching declaration exists.
   *
   * @param iden Identifier to find matching C declaration for
   */
  const auto& result(const std::string& iden) const
  {
    return results_[result_indicies_.at(iden)];
  }

  /**
   * Look up a declaration object via its position in the results vector.
   *
   * An exception will be thrown by the vector if `idx` is out of bounds.
   *
   * @param idx Index of a C declaration object in `results()`
   */
  const cdcl_dcln& result(std::size_t idx) const
  {
    return results_.at(idx);
  }

private:
  yy::location location_;
  std::string last_error_;
  std::vector<cdcl_dcln> results_;
  std::unordered_map<std::string, std::size_t> result_indicies_;

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

}  // namespace pdcpl

#endif  // PDCPL_BCDP_CDCL_PARSER_IMPL_HH_
