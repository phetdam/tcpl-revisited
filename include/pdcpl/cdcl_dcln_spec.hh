/**
 * @file cdcl_dcln_spec.hh
 * @author Derek Huang
 * @brief C++ header for the C declaration specification
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_DCLN_SPEC_HH_
#define PDCPL_CDCL_DCLN_SPEC_HH_

#include <cstdint>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "pdcpl/cdcl_type_spec.hh"
#include "pdcpl/dllexport.h"
#include "pdcpl/warnings.h"

namespace pdcpl {

/**
 * C declaration specifier.
 *
 * Specifies both the storage type and the qualified type.
 */
class cdcl_dcl_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_dcl_spec() : storage_{cdcl_storage::invalid}, spec_{} {}

  /**
   * Ctor.
   *
   * Constructs a declaration specifier with automatic storage from the
   * qualified type specifier, copying the qualified type specifier.
   *
   * @param spec Qualified type specifier
   */
  cdcl_dcl_spec(const cdcl_qtype_spec& spec)
    : cdcl_dcl_spec{cdcl_storage::st_auto, spec}
  {}

  /**
   * Ctor.
   *
   * Constructs a declaration specifier with automatic storage from the
   * qualified type specifier, moving from the qualified type specifier.
   *
   * @param spec Qualified type specifier
   */
  cdcl_dcl_spec(cdcl_qtype_spec&& spec)
    : cdcl_dcl_spec{cdcl_storage::st_auto, spec}
  {}

  /**
   * Ctor.
   *
   * Constructs by copying the qualified type specifier.
   *
   * @param storage Storage type
   * @param spec Qualified type specifier
   */
  cdcl_dcl_spec(cdcl_storage storage, const cdcl_qtype_spec& spec)
    : storage_{storage}, spec_{spec}
  {}

  /**
   * Ctor.
   *
   * Constructs by moving from the qualified type specifier.
   *
   * @param storage Storage type
   * @param spec Qualified type specifier
   */
  cdcl_dcl_spec(cdcl_storage storage, cdcl_qtype_spec&& spec)
    : storage_{storage}, spec_{std::move(spec)}
  {}

  /**
   * Return the storage type of the declaration specifier.
   */
  auto storage() const noexcept { return storage_; }

  /**
   * Return const reference to the qualified type specifier.
   */
  const auto& spec() const noexcept { return spec_; }

  /**
   * Write the C declaration specifier to an output stream.
   *
   * @param out Output stream
   */
  auto& write(std::ostream& out) const
  {
    auto storage = cdcl_storage_printer(storage_);
    // only print if not automatic storage
    if (storage.size())
      out << storage << " ";
    out << spec_;
    return out;
  }

private:
  cdcl_storage storage_;
  cdcl_qtype_spec spec_;
};

/**
 * Write the C declaration specifier to an output stream.
 *
 * @param out Output stream
 * @param spec C declaration specifier to write
 */
inline auto& operator<<(std::ostream& out, const cdcl_dcl_spec& spec)
{
  return spec.write(out);
}

/**
 * C array specifier.
 */
class cdcl_array_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_array_spec(std::size_t size = 0U) : size_{size} {}

  /**
   * Return specified array size.
   */
  auto size() const noexcept { return size_; }

  /**
   * Write the array specifier to an output stream.
   *
   * @param out Output stream
   */
  auto& write(std::ostream& out) const
  {
    out << "array[";
    if (size_)
      out << size_;
    out << "]";
    return out;
  }

private:
  std::size_t size_;
};

/**
 * Write the C declaration array specifier to an output stream.
 *
 * @param out Output stream
 * @param spec C declaration array specifier
 */
inline auto& operator<<(std::ostream& out, const cdcl_array_spec& spec)
{
  return spec.write(out);
}

/**
 * C pointers specifier.
 *
 * Serves as a thin wrapper around a vector of `cdcl_qual` values, each one
 * corresponding to an implied additional pointer in the specifier.
 */
class cdcl_ptrs_spec {
public:
  using container_type = std::vector<cdcl_qual>;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_ptrs_spec() : specs_{} {}

  /**
   * Ctor.
   *
   * Constructs from an initializer list of cv-qualifiers.
   *
   * @param specs Pointer cv-qualifiers
   */
  cdcl_ptrs_spec(const std::initializer_list<cdcl_qual>& specs) : specs_{specs} {}

  /**
   * Ctor.
   *
   * Constructs by copy from a vector of cv-qualifiers.
   *
   * @param specs Pointer cv-qualifiers
   */
  cdcl_ptrs_spec(const container_type& specs) : specs_{specs} {}

  /**
   * Ctor.
   *
   * Constructs by move from a vector of cv-qualifiers.
   *
   * @param specs Pointer cv-qualifiers
   */
  cdcl_ptrs_spec(container_type&& specs) : specs_{std::move(specs)} {}

  /**
   * Return const reference to vector of pointer cv-qualifiers.
   */
  const auto& specs() const noexcept { return specs_; }

  /**
   * Return iterator to the beginning of the vector.
   */
  auto begin() noexcept { return specs_.begin(); }

  /**
   * Return const iterator to the beginning of the vector.
   */
  auto begin() const noexcept { return specs_.begin(); }

  /**
   * Return iterator to the end of the vector.
   */
  auto end() noexcept { return specs_.end(); }

  /**
   * Return const iterator to the end of the vector.
   */
  auto end() const noexcept { return specs_.end(); }

  /**
   * Append a pointer's cv-qualifier to the vector.
   *
   * @param qual Pointer cv-qualifier
   */
  void append(cdcl_qual qual) { specs_.push_back(qual); }

  /**
   * Return the `i`th pointer cv-qualifier.
   */
  auto operator[](std::size_t i) const noexcept { return specs_[i]; };

  /**
   * Return the number of points in the specifier.
   */
  auto size() const noexcept { return specs_.size(); }

private:
  container_type specs_;
};

// forward declaration for cdcl_dclr, the C declaration declarator, as it is
// used via shared pointer in the cdcl_param_spec parameter specifier.
class cdcl_dclr;

/**
 * C function parameter specifier.
 */
class PDCPL_BCDP_PUBLIC cdcl_param_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_param_spec() = default;

  /**
   * Ctor.
   *
   * Constructs by copy from a qualified type specifier with no declarator.
   *
   * @param spec Qualified type specifier
   */
  cdcl_param_spec(const cdcl_qtype_spec& spec);

  /**
   * Ctor.
   *
   * Constructs by move from a qualified type specifier with no declarator.
   *
   * @param spec Qualified type specifier
   */
  cdcl_param_spec(cdcl_qtype_spec&& spec);

  /**
   * Ctor.
   *
   * Constructs by copy from a qualified type specifier and a declarator.
   *
   * @param spec Qualified type specifier
   * @param dclr C [abstract] [direct] declarator
   */
  cdcl_param_spec(const cdcl_qtype_spec& spec, const cdcl_dclr& dclr);

  /**
   * Ctor.
   *
   * Constructs by move from a qualified type specifier and a declarator.
   *
   * @param spec Qualified type specifier
   * @param dclr C [abstract] [direct] declarator
   */
  cdcl_param_spec(cdcl_qtype_spec&& spec, cdcl_dclr&& dclr);

  /**
   * Ctor.
   *
   * Constructs by move from a qualified type specifier and a declarator.
   *
   * @param spec Qualified type specifier
   * @param dclr C [abstract] [direct] declarator
   */
  cdcl_param_spec(cdcl_qtype_spec&& spec, std::unique_ptr<cdcl_dclr>&& dclr);

  /**
   * Return the parameter specifier's qualified type specifier.
   */
  auto spec() const noexcept { return spec_; }

  /**
   * Return a const reference to C declarator shared pointer.
   */
  const std::shared_ptr<cdcl_dclr>& dclr() const noexcept;

  /**
   * Write the function parameter specifier to an output stream.
   *
   * @param out Output stream
   */
  std::ostream& write(std::ostream& out) const;

private:
  PDCPL_MSVC_WARNING_DISABLE(4251)
  cdcl_qtype_spec spec_;
  std::shared_ptr<cdcl_dclr> dclr_;
  PDCPL_MSVC_WARNING_ENABLE()
};

/**
 * Write the C declaration function parameter specifier to an output stream.
 *
 * @param out Output stream
 * @param spec C declaration function parameter specifier
 */
inline auto& operator<<(std::ostream& out, const cdcl_param_spec& spec)
{
  return spec.write(out);
}

/**
 * C function parameters specifier.
 *
 * Thin wrapper around a vector of `cdcl_param_spec` values and indicates
 * if the function being described is a variadic function.
 */
class cdcl_params_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_params_spec() = default;

  /**
   * Ctor.
   *
   * Constructs from an initializer list of function parameter specifiers.
   *
   * @param specs Function parameter specifiers
   * @param variadic `true` if function accepts varags, `false` otherwise
   */
  cdcl_params_spec(
    const std::initializer_list<cdcl_param_spec>& specs, bool variadic = false)
    : specs_{specs}, variadic_{variadic}
  {}

  /**
   * Ctor.
   *
   * Constructs by copy from a vector function parameter specifiers.
   *
   * @param specs Function parameter specifiers
   * @param variadic `true` if function accepts varags, `false` otherwise
   */
  cdcl_params_spec(
    const std::vector<cdcl_param_spec>& specs, bool variadic = false)
    : specs_{specs}, variadic_{variadic}
  {}

  /**
   * Ctor.
   *
   * Constructs by move from a vector function parameter specifiers.
   *
   * @param specs Function parameter specifiers
   * @param variadic `true` if function accepts varags, `false` otherwise
   */
  cdcl_params_spec(
    std::vector<cdcl_param_spec>&& specs, bool variadic = false)
    : specs_{std::move(specs)}, variadic_{variadic}
  {}

  /**
   * Return const reference to the vector of function parameter specifiers.
   */
  const auto& specs() const noexcept { return specs_; }

  /**
   * Return `true` if function is variadic, `false` otherwise.
   */
  auto variadic() const noexcept { return variadic_; }

  /**
   * Mark the function as being variadic or not being variadic.
   *
   * @param value Truth value
   * @returns Previous value of `variadic()`
   */
  auto variadic(bool value) noexcept
  {
    auto old_value = variadic_;
    variadic_ = value;
    return old_value;
  }

  /**
   * Return iterator to the beginning of the vector.
   */
  auto begin() noexcept { return specs_.begin(); }

  /**
   * Return const iterator to the beginning of the vector.
   */
  auto begin() const noexcept { return specs_.begin(); }

  /**
   * Return iterator to the end of the vector.
   */
  auto end() noexcept { return specs_.end(); }

  /**
   * Return const iterator to the end of the vector.
   */
  auto end() const noexcept { return specs_.end(); }

  /**
   * Return const reference to the `i`th function parameter specifier.
   */
  const auto& operator[](std::size_t i) const noexcept { return specs_[i]; }

  /**
   * Append a function parameter specifier by copy to the vector.
   *
   * @param spec Function parameter specifier
   */
  void append(const cdcl_param_spec& spec) { specs_.push_back(spec); }

  /**
   * Append a function parameter specifier by move to the vector.
   *
   * @param spec Function parameter specifier
   */
  void append(cdcl_param_spec&& spec) { specs_.push_back(std::move(spec)); }

  /**
   * Return number of function parameter specifiers.
   */
  auto size() const noexcept { return specs_.size(); }

private:
  std::vector<cdcl_param_spec> specs_;
  bool variadic_;
};

/**
 * `std::variant` specialization for a C declarator specifier.
 *
 * The variant can hold an array specifier, pointers specifier, or the
 * parameters specifier used to indicate a function.
 */
using cdcl_dclr_spec_variant = std::variant<
  cdcl_array_spec, cdcl_ptrs_spec, cdcl_params_spec
>;

/**
 * C declarator specifier.
 *
 * Thin wrapper over the `cdcl_dclr_spec_variant` variant specialization used
 * to represent a specifier for a C declarator, e.g. an array specifier,
 * pointers specifier, or function parameters specifier.
 */
class cdcl_dclr_spec : public cdcl_dclr_spec_variant {
public:
  using variant_type = cdcl_dclr_spec_variant;
  using variant_type::variant_type;

  /**
   * Variant printer.
   *
   * Each `operator()` overload returns a string representation for each type.
   */
  struct printer {
    /**
     * Return string representation for an array specifier.
     *
     * @param spec Array specifier
     */
    auto operator()(const cdcl_array_spec& spec) const
    {
      std::stringstream ss;
      ss << spec << " of";
      return ss.str();
    }

    /**
     * Return string representation for a pointers specifier.
     *
     * @param specs Pointers specifier
     */
    auto operator()(const cdcl_ptrs_spec& specs) const
    {
      std::stringstream ss;
      for (auto it = specs.begin(); it != specs.end(); it++) {
        if (std::distance(specs.begin(), it))
          ss << " ";
        switch (*it) {
          case cdcl_qual::qnone:
            ss << "pointer";
            break;
          case cdcl_qual::qconst:
            ss << "const pointer";
            break;
          case cdcl_qual::qvolatile:
            ss << "volatile pointer";
            break;
          case cdcl_qual::qconst_volatile:
            ss << "const volatile pointer";
            break;
          default:
            throw std::runtime_error{"invalid pointer specification"};
        }
        ss << " to";
      }
      return ss.str();
    }

    /**
     * Return string representation for a function parameters specifier.
     *
     * @note Only member that is exported from a DLL when compiling on Windows
     *  since it is the only non-inline `cdcl_dclr_spec::printer` member.
     *
     * @param specs Function parameters specifier
     */
    PDCPL_BCDP_PUBLIC
    std::string operator()(const cdcl_params_spec& specs) const;
  };
};

/**
 * C [abstract] [direct] declarator.
 *
 * The same type is used to handle the `dclr` declarator, `dir_dclr`
 * direct-declarator, `a_dclr` abstract declarator, and `a_dir_dclr` abstract
 * direct-declarator structures specified in the C grammar.
 */
class cdcl_dclr {
public:
  using container_type = std::vector<cdcl_dclr_spec>;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions. Can also be used to represent
   * one of the abstract declarators that do not have an identifier.
   */
  cdcl_dclr() : iden_{}, specs_{} {}

  /**
   * Ctor.
   *
   * @param iden Declarator identifier, can be empty for abstract declarators
   */
  cdcl_dclr(const std::string& iden) : iden_{iden}, specs_{} {}

  /**
   * Return const reference to the identifier.
   *
   * This is empty for abstract [direct] declarators.
   */
  const auto& iden() const noexcept { return iden_; }

  /**
   * Return const reference to the vector of declarator specifiers.
   */
  const auto& specs() const noexcept { return specs_; }

  /**
   * Return iterator to the beginning of the declarator specifier vector.
   */
  auto begin() noexcept { return specs_.begin(); }

  /**
   * Return const iterator to the beginning of the declarator specifier vector.
   */
  auto begin() const noexcept { return specs_.begin(); }

  /**
   * Return iterator to the end of the declarator specifier vector.
   */
  auto end() noexcept { return specs_.end(); }

  /**
   * Return const iterator to the end of the declarator specifier vector.
   */
  auto end() const noexcept { return specs_.end(); }

  /**
   * Return const reference to the `i`th declarator specifier.
   */
  const auto& operator[](std::size_t i) const noexcept { return specs_[i]; }

  /**
   * Append declarator specifier by copy to the vector of specifiers.
   *
   * @param spec C declarator specifier
   */
  void append(const cdcl_dclr_spec& spec) { specs_.push_back(spec); }

  /**
   * Append declarator specifier by move to the vector of specifiers.
   *
   * @param spec C declarator specifier
   */
  void append(cdcl_dclr_spec&& spec) { specs_.push_back(std::move(spec)); }

  /**
   * Prepend declarator specifier by copy to the vector of specifiers.
   *
   * @param spec C declarator specifier
   */
  void prepend(const cdcl_dclr_spec& spec)
  {
    specs_.insert(specs_.begin(), spec);
  }

  /**
   * Prepend declarator specifier by move to the vector of specifiers.
   *
   * @param spec C declarator specifier
   */
  void prepend(cdcl_dclr_spec&& spec)
  {
    specs_.insert(specs_.begin(), std::move(spec));
  }

  /**
   * Write the declarator to an output stream.
   *
   * @param out Output stream
   */
  auto& write(std::ostream& out) const
  {
    // only print identifier + colon if provided
    if (iden_.size()) {
      out << iden_ << ":";
      // separating space only then printed if there are specifiers
      if (specs_.size())
        out << " ";
    }
    // visit each specifier
    for (auto it = specs_.begin(); it != specs_.end(); it++) {
      if (std::distance(specs_.begin(), it))
        out << " ";
      out << std::visit(cdcl_dclr_spec::printer{}, *it);
    }
    return out;
  }

  /**
   * Return the declarator's string representation.
   *
   * Contains what would be written into an output stream using `write`.
   */
  operator std::string() const
  {
    std::stringstream ss;
    write(ss);
    return ss.str();
  }

private:
  std::string iden_;
  container_type specs_;
};

/**
 * Write the C declarator to an output stream.
 *
 * @param out Output stream
 * @param dclr C declarator
 */
inline auto& operator<<(std::ostream& out, const cdcl_dclr& dclr)
{
  return dclr.write(out);
}

/**
 * C init declarator.
 *
 * @note In actual C programs, the init declarator includes initializers.
 */
class cdcl_init_dclr : public std::variant<cdcl_dclr> {
public:
  using variant_type = std::variant<cdcl_dclr>;
  using variant_type::variant_type;

  /**
   * Variant printer.
   *
   * Each `operator()` overload returns a string representation for each type.
   */
  struct printer {
    /**
     * Return string representation for a declarator.
     *
     * @param dclr Declarator
     */
    auto operator()(const cdcl_dclr& dclr) const
    {
      return static_cast<std::string>(dclr);
    }
  };
};

/**
 * Write the C declaration init declarator to an output stream.
 *
 * @param out Output stream
 * @param init_dclr C declaration init declarator
 */
inline auto& operator<<(std::ostream& out, const cdcl_init_dclr& init_dclr)
{
  out << std::visit(cdcl_init_dclr::printer{}, init_dclr);
  return out;
}

/**
 * C init declarators.
 *
 * Thin wrapper around a `cdcl_init_dclr` vector.
 */
class cdcl_init_dclrs {
public:
  using container_type = std::vector<cdcl_init_dclr>;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_init_dclrs() : init_dclrs_{} {}

  /**
   * Ctor.
   *
   * Constructs from an initializer list of init declarators.
   *
   * @param init_dclrs Init declarators
   */
  cdcl_init_dclrs(const std::initializer_list<cdcl_init_dclr>& init_dclrs)
    : init_dclrs_{init_dclrs}
  {}

  /**
   * Ctor.
   *
   * Constructs by copy from another vector of init declarators.
   *
   * @param init_dclrs Init declarators
   */
  cdcl_init_dclrs(const container_type& init_dclrs) : init_dclrs_{init_dclrs} {}

  /**
   * Ctor.
   *
   * Constructs by move from another vector of init declarators.
   *
   * @param init_dclrs Init declarators
   */
  cdcl_init_dclrs(container_type&& init_dclrs)
    : init_dclrs_{std::move(init_dclrs)}
  {}

  /**
   * Return const reference to the vector of init declarators.
   */
  const auto& init_dclrs() const noexcept { return init_dclrs_; }

  /**
   * Return iterator to the beginning of the init declarators vector.
   */
  auto begin() noexcept { return init_dclrs_.begin(); }

  /**
   * Return const iterator to the beginning of the init declarators vector.
   */
  auto begin() const noexcept { return init_dclrs_.begin(); }

  /**
   * Return iterator to the end of the init declarators vector.
   */
  auto end() noexcept { return init_dclrs_.end(); }

  /**
   * Return const iterator to the end of the init declarators vector.
   */
  auto end() const noexcept { return init_dclrs_.end(); }

  /**
   * Return const reference to the `i`th init declarators vector.
   */
  const auto& operator[](std::size_t i) const noexcept
  {
    return init_dclrs_[i];
  }

  /**
   * Append init declarator by copy to the init declarators vectors.
   *
   * @param init_dclr Init declarator to append
   */
  void append(const cdcl_init_dclr& init_dclr)
  {
    init_dclrs_.push_back(init_dclr);
  }

  /**
   * Append init declarator by move to the init declarators vectors.
   *
   * @param init_dclr Init declarator to append
   */
  void append(cdcl_init_dclr&& init_dclr)
  {
    init_dclrs_.push_back(std::move(init_dclr));
  }

  /**
   * Return size of the init declarators vector.
   */
  auto size() const noexcept { return init_dclrs_.size(); }

private:
  container_type init_dclrs_;
};

/**
 * C declaration.
 *
 * This consists of a declaration specifier and a declarator.
 */
class cdcl_dcln {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_dcln() : dcl_spec_{}, dclr_{} {}

  /**
   * Ctor.
   *
   * Constructs by copy from a declaration specifier and a declarator.
   *
   * @param dcl_spec Declaration specifier
   * @param dclr [Abstract] [direct] declarator
   */
  cdcl_dcln(const cdcl_dcl_spec& dcl_spec, const cdcl_dclr& dclr)
    : dcl_spec_{dcl_spec}, dclr_{dclr}
  {}

  /**
   * Ctor.
   *
   * Constructs by move from a declaration specifier and a declarator.
   *
   * @param dcl_spec Declaration specifier
   * @param dclr [Abstract] [direct] declarator
   */
  cdcl_dcln(cdcl_dcl_spec&& dcl_spec, cdcl_dclr&& dclr)
    : dcl_spec_{std::move(dcl_spec)}, dclr_{std::move(dclr)}
  {}

  /**
   * Return const reference to the declaration specifier.
   */
  const auto& dcl_spec() const noexcept { return dcl_spec_; }

  /**
   * Return const reference to the declarator.
   */
  const auto& dclr() const noexcept { return dclr_; }

  /**
   * Write the C declaration to an output stream.
   *
   * @param out Output stream
   */
  auto& write(std::ostream& out) const
  {
    out << dclr_ << " " << dcl_spec_;
    return out;
  }

private:
  cdcl_dcl_spec dcl_spec_;
  cdcl_dclr dclr_;
};

/**
 * Write the C declaration to an output stream.
 *
 * @param out Output stream
 * @param dcln C declaration
 */
inline auto& operator<<(std::ostream& out, const cdcl_dcln& dcln)
{
  return dcln.write(out);
}

}  // namespace pdcpl

#endif  // PDCPL_CDCL_DCLN_SPEC_HH_
