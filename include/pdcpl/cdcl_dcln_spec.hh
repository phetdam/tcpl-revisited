/**
 * @file cdcl_dcln_spec.hh
 * @author Derek Huang
 * @brief C++ header for the C declaration specification
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_DCLN_SPEC_HH_
#define PDCPL_CDCL_DCLN_SPEC_HH_

#include <cstdint>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "pdcpl/cdcl_type_spec.hh"

namespace pdcpl {

/**
 * C declaration specifier.
 *
 * Specifies both the storage type and the qualified type.
 */
class cdcl_dcl_spec {
public:
  cdcl_dcl_spec() : storage_{cdcl_storage::invalid}, spec_{} {}

  cdcl_dcl_spec(const cdcl_qtype_spec& spec)
    : cdcl_dcl_spec{cdcl_storage::st_auto, spec}
  {}

  cdcl_dcl_spec(cdcl_qtype_spec&& spec)
    : cdcl_dcl_spec{cdcl_storage::st_auto, spec}
  {}

  cdcl_dcl_spec(cdcl_storage storage, const cdcl_qtype_spec& spec)
    : storage_{storage}, spec_{spec}
  {}

  cdcl_dcl_spec(cdcl_storage storage, cdcl_qtype_spec&& spec)
    : storage_{storage}, spec_{std::move(spec)}
  {}

  auto storage() const noexcept { return storage_; }
  const auto& spec() const noexcept { return spec_; }

private:
  cdcl_storage storage_;
  cdcl_qtype_spec spec_;
};

/**
 * C array specifier.
 */
class cdcl_array_spec {
public:
  cdcl_array_spec(std::size_t size = 0U) : size_{size} {}

  auto size() const noexcept { return size_; }

private:
  std::size_t size_;
};

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

  cdcl_ptrs_spec() : specs_{} {}

  cdcl_ptrs_spec(const std::initializer_list<cdcl_qual>& specs) : specs_{specs} {}

  cdcl_ptrs_spec(const container_type& specs) : specs_{specs} {}

  cdcl_ptrs_spec(container_type&& specs) : specs_{specs} {}

  const auto& specs() const noexcept { return specs_; }

  auto begin() noexcept { return specs_.begin(); }

  auto begin() const noexcept { return specs_.begin(); }

  auto end() noexcept { return specs_.end(); }

  auto end() const noexcept { return specs_.end(); }

  void append(cdcl_qual qual) { specs_.push_back(qual); }

  auto operator[](std::size_t i) const noexcept { return specs_[i]; };

private:
  container_type specs_;
};

/**
 * Variant for a declarator specifier.
 */
class cdcl_dclr_spec : public std::variant<cdcl_array_spec, cdcl_ptrs_spec> {
public:
  using variant_type = std::variant<cdcl_array_spec, cdcl_ptrs_spec>;
  using variant_type::variant_type;

  /**
   * Variant printer.
   *
   * Each `operator()` overload returns a string representation for each type.
   */
  struct printer {
    auto operator()(const cdcl_array_spec& spec)
    {
      std::string size_spec = (!spec.size()) ? "" : std::to_string(spec.size());
      return "array[" + size_spec + "] of ";
    }

    auto operator()(const cdcl_ptrs_spec& specs)
    {
      std::stringstream ss;
      for (const auto spec : specs) {
        switch (spec) {
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
        ss << " to ";
      }
      return ss.str();
    }
  };
};

/**
 * C [direct] declarator.
 *
 * The same type is used to handle both the `dclr` and `dir_dclr` declarator
 * and direct-declarator structures specified in the C grammar.
 */
class cdcl_dclr {
public:
  using container_type = std::vector<cdcl_qual>;
  using iterator = container_type::iterator;
  using const_iterator = container_type::const_iterator;

  cdcl_dclr() : iden_{}, specs_{} {}

  cdcl_dclr(const std::string& iden) : iden_{iden}, specs_{} {}

  const auto& iden() const noexcept { return iden_; }

  const auto& specs() const noexcept { return specs_; }

  auto begin() noexcept { return specs_.begin(); }

  auto begin() const noexcept { return specs_.begin(); }

  auto end() noexcept { return specs_.end(); }

  auto end() const noexcept { return specs_.end(); }

  void append(const cdcl_dclr_spec& spec) { specs_.push_back(spec); }

  void append(cdcl_dclr_spec&& spec) { specs_.push_back(spec); }

  void prepend(const cdcl_dclr_spec& spec) { specs_.insert(specs_.begin(), spec); }

  void prepend(cdcl_dclr_spec&& spec) { specs_.insert(specs_.begin(), spec); }

  auto& write(std::ostream& out) const
  {
    auto iden = this->iden();
    out << ((iden.empty()) ? "[?]" : iden) << ": ";
    for (const auto& spec : specs())
      out << std::visit(cdcl_dclr_spec::printer{}, spec);
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
  std::vector<cdcl_dclr_spec> specs_;
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
    auto operator()(const cdcl_dclr& dclr)
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

  cdcl_init_dclrs() : init_dclrs_{} {}

  cdcl_init_dclrs(const std::initializer_list<cdcl_init_dclr>& init_dclrs)
    : init_dclrs_{init_dclrs}
  {}

  cdcl_init_dclrs(const container_type& init_dclrs) : init_dclrs_{init_dclrs} {}

  cdcl_init_dclrs(container_type&& init_dclrs) : init_dclrs_{init_dclrs} {}

  const auto& init_dclrs() const noexcept { return init_dclrs_; }

  auto begin() noexcept { return init_dclrs_.begin(); }

  auto begin() const noexcept { return init_dclrs_.begin(); }

  auto end() noexcept { return init_dclrs_.end(); }

  auto end() const noexcept { return init_dclrs_.end(); }

  void append(const cdcl_init_dclr& init_dclr)
  {
    init_dclrs_.push_back(init_dclr);
  }

  void append(cdcl_init_dclr&& init_dclr)
  {
    init_dclrs_.push_back(init_dclr);
  }

private:
  container_type init_dclrs_;
};

}  // namespace pdcpl

#endif  // PDCPL_CDCL_DCLN_SPEC_HH_
