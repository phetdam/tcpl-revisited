/**
 * @file cdcl_dcln_spec_test.cc
 * @author Derek Huang
 * @brief cdcl_dcln_spec.hh unit tests
 * @copyright MIT License
 */

#include "pdcpl/cdcl_dcln_spec.hh"

#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace {

/**
 * Base test fixture class for all declaration specification tests.
 */
class CdclDclnSpecTest : public ::testing::Test {};

/**
 * Parameterized testing fixture for `cdcl_dcl_spec` tests.
 */
class CdclDclSpecParamTest
  : public CdclDclnSpecTest,
    public ::testing::WithParamInterface<
      std::pair<pdcpl::cdcl_dcl_spec, std::string> > {};

/**
 * Test that the `cdcl_dcl_spec` string representation is as expected.
 */
TEST_P(CdclDclSpecParamTest, ReprTest)
{
  EXPECT_EQ(GetParam().second, static_cast<std::string>(GetParam().first));
}

INSTANTIATE_TEST_SUITE_P(
  Base,
  CdclDclSpecParamTest,
  ::testing::Values(
    // extern const struct my_struct
    CdclDclSpecParamTest::ParamType{
      {
        pdcpl::cdcl_storage::st_extern,
        {pdcpl::cdcl_qual::qconst, {pdcpl::cdcl_type::gstruct, "my_struct"}}
      },
      pdcpl::cdcl_storage_printer(pdcpl::cdcl_storage::st_extern) + " " +
        pdcpl::cdcl_qual_printer(pdcpl::cdcl_qual::qconst) + " " +
        pdcpl::cdcl_type_printer(pdcpl::cdcl_type::gstruct) + " my_struct"
    },
    // static const volatile unsigned long
    CdclDclSpecParamTest::ParamType{
      {{pdcpl::cdcl_qual::qconst_volatile, pdcpl::cdcl_type::ulong}},
      pdcpl::cdcl_qual_printer(pdcpl::cdcl_qual::qconst_volatile) + " " +
        pdcpl::cdcl_type_printer(pdcpl::cdcl_type::ulong)
    },
    // static const typedef my_type
    CdclDclSpecParamTest::ParamType{
      {
        pdcpl::cdcl_storage::st_static,
        {pdcpl::cdcl_qual::qconst, {pdcpl::cdcl_type::gtype, "my_type"}}
      },
      pdcpl::cdcl_storage_printer(pdcpl::cdcl_storage::st_static) + " " +
        pdcpl::cdcl_qual_printer(pdcpl::cdcl_qual::qconst) + " " +
        pdcpl::cdcl_type_printer(pdcpl::cdcl_type::gtype) + " my_type"
    }
  )
);

}  // namespace
