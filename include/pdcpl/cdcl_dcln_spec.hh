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
 * Serves as a thin wrapper around a vector of `cdcl_qual` values.
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

  cdcl_ptrs_spec(const std::initializer_list<cdcl_qual>& specs) : specs_{specs} {}

  cdcl_ptrs_spec(const container_type& specs) : specs_{specs} {}

  cdcl_ptrs_spec(container_type&& specs) : specs_{std::move(specs)} {}

  const auto& specs() const noexcept { return specs_; }

  auto begin() noexcept { return specs_.begin(); }

  auto begin() const noexcept { return specs_.begin(); }

  auto end() noexcept { return specs_.end(); }

  auto end() const noexcept { return specs_.end(); }

  void append(cdcl_qual qual) { specs_.push_back(qual); }

  auto operator[](std::size_t i) const noexcept { return specs_[i]; };

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
 */
class cdcl_params_spec {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_params_spec() = default;

  cdcl_params_spec(
    const std::vector<cdcl_param_spec>& specs, bool variadic = false)
    : specs_{specs}, variadic_{variadic}
  {}

  cdcl_params_spec(
    const std::initializer_list<cdcl_param_spec>& specs, bool variadic = false)
    : specs_{specs}, variadic_{variadic}
  {}

  cdcl_params_spec(
    std::vector<cdcl_param_spec>&& specs, bool variadic = false)
    : specs_{std::move(specs)}, variadic_{variadic}
  {}

  const auto& specs() const noexcept { return specs_; }

  auto variadic() const noexcept { return variadic_; }

  auto variadic(bool value) noexcept
  {
    auto old_value = variadic_;
    variadic_ = value;
    return old_value;
  }

  auto begin() noexcept { return specs_.begin(); }

  auto begin() const noexcept { return specs_.begin(); }

  auto end() noexcept { return specs_.end(); }

  auto end() const noexcept { return specs_.end(); }

  const auto& operator[](std::size_t i) const noexcept { return specs_[i]; }

  void append(const cdcl_param_spec& spec) { specs_.push_back(spec); }

  void append(cdcl_param_spec&& spec) { specs_.push_back(std::move(spec)); }

  auto size() const noexcept { return specs_.size(); }

private:
  std::vector<cdcl_param_spec> specs_;
  bool variadic_;
};

/**
 * `std::variant` specialization for a declarator specifier.
 *
 * The variant can hold an array specifier, pointers specifier, or the
 * parameters specifier used to indicate a function.
 */
using cdcl_dclr_spec_variant = std::variant<
  cdcl_array_spec, cdcl_ptrs_spec, cdcl_params_spec
>;

// MSVC emits C4251 for cdcl_dclr_spec since STL types are technically not
// marked for DLL export, but can be ignored when inheriting an STL type.
PDCPL_MSVC_WARNING_DISABLE(4251)
/**
 * C declarator specifier.
 */
class PDCPL_BCDP_PUBLIC cdcl_dclr_spec : public cdcl_dclr_spec_variant {
PDCPL_MSVC_WARNING_ENABLE()
public:
  using variant_type = cdcl_dclr_spec_variant;
  using variant_type::variant_type;

  /**
   * Variant printer.
   *
   * Each `operator()` overload returns a string representation for each type.
   */
  struct printer {
    auto operator()(const cdcl_array_spec& spec) const
    {
      std::stringstream ss;
      ss << spec << " of";
      return ss.str();
    }

    auto operator()(const cdcl_ptrs_spec& specs) const
    {
      std::stringstream ss;
      // for (const auto spec : specs) {
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
   * Required for use in Bison semantic actions.
   */
  cdcl_dclr() : iden_{}, specs_{} {}

  cdcl_dclr(const std::string& iden) : iden_{iden}, specs_{} {}

  const auto& iden() const noexcept { return iden_; }

  const auto& specs() const noexcept { return specs_; }

  auto begin() noexcept { return specs_.begin(); }

  auto begin() const noexcept { return specs_.begin(); }

  auto end() noexcept { return specs_.end(); }

  auto end() const noexcept { return specs_.end(); }

  const auto& operator[](std::size_t i) const noexcept { return specs_[i]; }

  void append(const cdcl_dclr_spec& spec) { specs_.push_back(spec); }

  void append(cdcl_dclr_spec&& spec) { specs_.push_back(std::move(spec)); }

  void prepend(const cdcl_dclr_spec& spec)
  {
    specs_.insert(specs_.begin(), spec);
  }

  void prepend(cdcl_dclr_spec&& spec)
  {
    specs_.insert(specs_.begin(), std::move(spec));
  }

  auto& write(std::ostream& out) const
  {
    if (iden_.size())
      out << iden_ << ": ";
    for (auto it = specs_.begin(); it != specs_.end(); it++) {
      if (std::distance(specs_.begin(), it))
        out << " ";
      out << std::visit(cdcl_dclr_spec::printer{}, *it);
    }
    return out;
  }

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
 * `operator<<` overload for `cdcl_dclr` to enable streaming.
 */
inline auto& operator<<(std::ostream& out, const cdcl_dclr& dclr)
{
  return dclr.write(out);
}

/**
 * C init declarator.
 */
class cdcl_init_dclr : public std::variant<cdcl_dclr> {
public:
  using variant_type = std::variant<cdcl_dclr>;
  using variant_type::variant_type;

  struct printer {
    auto operator()(const cdcl_dclr& dclr) const
    {
      return static_cast<std::string>(dclr);
    }
  };
};

/**
 * `operator<<` overload for `cdcl_init_dclr` to enable streaming.
 */
inline auto& operator<<(std::ostream& out, const cdcl_init_dclr& init_dclr)
{
  out << std::visit(cdcl_init_dclr::printer{}, init_dclr);
  return out;
}

/**
 * C init declarators.
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

  cdcl_init_dclrs(const std::initializer_list<cdcl_init_dclr>& init_dclrs)
    : init_dclrs_{init_dclrs}
  {}

  cdcl_init_dclrs(const container_type& init_dclrs) : init_dclrs_{init_dclrs} {}

  cdcl_init_dclrs(container_type&& init_dclrs)
    : init_dclrs_{std::move(init_dclrs)}
  {}

  const auto& init_dclrs() const noexcept { return init_dclrs_; }

  auto begin() noexcept { return init_dclrs_.begin(); }

  auto begin() const noexcept { return init_dclrs_.begin(); }

  auto end() noexcept { return init_dclrs_.end(); }

  auto end() const noexcept { return init_dclrs_.end(); }

  const auto& operator[](std::size_t i) const noexcept
  {
    return init_dclrs_[i];
  }

  void append(const cdcl_init_dclr& init_dclr)
  {
    init_dclrs_.push_back(init_dclr);
  }

  void append(cdcl_init_dclr&& init_dclr)
  {
    init_dclrs_.push_back(init_dclr);
  }

  auto size() const noexcept { return init_dclrs_.size(); }

private:
  container_type init_dclrs_;
};

/**
 * C declaration.
 */
class cdcl_dcln {
public:
  /**
   * Default ctor.
   *
   * Required for use in Bison semantic actions.
   */
  cdcl_dcln() : dcl_spec_{}, dclr_{} {}

  cdcl_dcln(const cdcl_dcl_spec& dcl_spec, const cdcl_dclr& dclr)
    : dcl_spec_{dcl_spec}, dclr_{dclr}
  {}

  cdcl_dcln(cdcl_dcl_spec&& dcl_spec, cdcl_dclr&& dclr)
    : dcl_spec_{std::move(dcl_spec)}, dclr_{std::move(dclr)}
  {}

  const auto& dcl_spec() const noexcept { return dcl_spec_; }

  const auto& dclr() const noexcept { return dclr_; }

  auto& write(std::ostream& out) const
  {
    out << dclr_ << " " << dcl_spec_;
    return out;
  }

private:
  cdcl_dcl_spec dcl_spec_;
  cdcl_dclr dclr_;
};

inline auto& operator<<(std::ostream& out, const cdcl_dcln& dcln)
{
  return dcln.write(out);
}

}  // namespace pdcpl

#endif  // PDCPL_CDCL_DCLN_SPEC_HH_
