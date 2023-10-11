/**
 * @file cdcl_dcln_spec.hh
 * @author Derek Huang
 * @brief C++ header for the C declaration specification
 * @copyright MIT License
 */

#ifndef PDCPL_CDCL_DCLN_SPEC_HH_
#define PDCPL_CDCL_DCLN_SPEC_HH_

#include <utility>

#include "pdcpl/cdcl_type_spec.hh"

namespace pdcpl {

/**
 * C declaration specifier.
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

}  // namespace pdcpl

#endif  // PDCPL_CDCL_DCLN_SPEC_HH_
