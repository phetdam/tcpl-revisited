/**
 * @file cdcl_dcln_spec.cc
 * @author Derek Huang
 * @brief C++ source for the C declaration specification
 * @copyright MIT License
 */

#include "pdcpl/cdcl_dcln_spec.hh"

#include <ostream>
#include <memory>
#include <sstream>
#include <variant>

namespace pdcpl {

// cdcl_param_spec functions involving cdcl_dclr member dclr_ cannot be inline
// due to cdcl_dclr only being forward declared at time of use

cdcl_param_spec::cdcl_param_spec(const cdcl_qtype_spec& spec)
  : spec_{spec}, dclr_{}
{}

cdcl_param_spec::cdcl_param_spec(cdcl_qtype_spec&& spec)
  : spec_{spec}, dclr_{}
{}

cdcl_param_spec::cdcl_param_spec(
  const cdcl_qtype_spec& spec, const cdcl_dclr& dclr)
  : spec_{spec}, dclr_{std::make_unique<cdcl_dclr>(dclr)}
{}

cdcl_param_spec::cdcl_param_spec(cdcl_qtype_spec&& spec, cdcl_dclr&& dclr)
  : spec_{spec}, dclr_{std::make_unique<cdcl_dclr>(dclr)}
{}

cdcl_param_spec::cdcl_param_spec(
  cdcl_qtype_spec&& spec, std::unique_ptr<cdcl_dclr>&& dclr)
  : spec_{spec}, dclr_{std::move(dclr)}
{}

const std::shared_ptr<cdcl_dclr>& cdcl_param_spec::dclr() const noexcept
{
  return dclr_;
}

std::ostream& cdcl_param_spec::write(std::ostream& out) const
{
  // write declarator if any
  if (dclr_)
    out << *dclr_;
  // write qualified type specifier
  out << spec_;
  return out;
}

// cdcl_dclr_spec::printer operator() cannot be inline as a cdcl_dclr member
// needs to be accessed and at time of class definition cdcl_dclr is incomplete

std::string cdcl_dclr_spec::printer::operator()(
  const cdcl_params_spec& specs) const
{
  std::stringstream ss;
  ss << "function (";
  for (auto it = specs.begin(); it != specs.end(); it++) {
    // write param specifier
    ss << *it;
    // add separating comma if necessary
    if (std::distance(it, specs.end()) > 1)
      ss << ", ";
  }
  if (specs.variadic())
    ss << ", ...";
  ss << ") returning ";
  return ss.str();
}

}  // namespace
