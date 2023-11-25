/**
 * @file cdcl_type_spec_test.cc
 * @author Derek Huang
 * @brief cdcl_type_spec.hh unit tests
 * @copyright MIT License
 */

#include "pdcpl/cdcl_type_spec.hh"

#include <string>
#include <utility>

#include <gtest/gtest.h>

namespace {

/**
 * Base test fixture class for all C declaration type specification tests.
 */
class DclTypeSpecTest : public ::testing::Test {};

/**
 * Parameterized testing fixture for C declaration type specifier tests.
 *
 * @note We could use virtual functions to dispatch from a base writable type
 *  that provides the `operator std::string const` and requires implementation
 *  of the `std::ostream& write(std::ostream&) const`. There would also be a
 *  non-template `auto& operator<<(std::ostream&, const writeable&)` as well.
 */
class CdclTypeSpecParamTest
  : public DclTypeSpecTest,
    public ::testing::WithParamInterface<
      std::pair<pdcpl::cdcl_type_spec, std::string>>
{};

/**
 * Test that the `cdcl_type_spec` string representation is as expected.
 */
TEST_P(CdclTypeSpecParamTest, ReprTest)
{
  std::string repr = GetParam().first;
  EXPECT_EQ(GetParam().second, repr);
}

INSTANTIATE_TEST_SUITE_P(
  BaseInstance,
  CdclTypeSpecParamTest,
  ::testing::Values(
    // unsigned long
    CdclTypeSpecParamTest::ParamType{
      pdcpl::cdcl_type::ulong,
      pdcpl::cdcl_type_printer(pdcpl::cdcl_type::ulong)
    },
    // enum cool_enum
    CdclTypeSpecParamTest::ParamType{
      {pdcpl::cdcl_type::genum, "cool_enum"},
      pdcpl::cdcl_type_printer(pdcpl::cdcl_type::genum) + " cool_enum"
    },
    // typedef my_type
    CdclTypeSpecParamTest::ParamType{
      {pdcpl::cdcl_type::gtype, "my_type"},
      pdcpl::cdcl_type_printer(pdcpl::cdcl_type::gtype) + " my_type"
    }
  )
);

/**
 * Parameterized testing fixture for C declaration qualified type spec tests.
 */
class CdclQualTypeSpecParamTest
  : public DclTypeSpecTest,
    public ::testing::WithParamInterface<
      std::pair<pdcpl::cdcl_qtype_spec, std::string>>
{};

/**
 * Test that the `cdcl_qtype_spec` string representation is as expected.
 */
TEST_P(CdclQualTypeSpecParamTest, ReprTest)
{
  std::string repr = GetParam().first;
  EXPECT_EQ(GetParam().second, repr);
}

INSTANTIATE_TEST_SUITE_P(
  BaseInstance,
  CdclQualTypeSpecParamTest,
  ::testing::Values(
    // struct my_struct
    CdclQualTypeSpecParamTest::ParamType{
      {{pdcpl::cdcl_type::gstruct, "my_struct"}},
      pdcpl::cdcl_type_printer(pdcpl::cdcl_type::gstruct) + " my_struct"
    },
    // const volatile unsigned short
    CdclQualTypeSpecParamTest::ParamType{
      {pdcpl::cdcl_qual::qconst_volatile, pdcpl::cdcl_type::sshort},
      pdcpl::cdcl_qual_printer(pdcpl::cdcl_qual::qconst_volatile) + " " +
        pdcpl::cdcl_type_printer(pdcpl::cdcl_type::sshort)
    },
    // const typedef my_type
    CdclQualTypeSpecParamTest::ParamType{
      {pdcpl::cdcl_qual::qconst, {pdcpl::cdcl_type::gtype, "my_type"}},
      pdcpl::cdcl_qual_printer(pdcpl::cdcl_qual::qconst) + " " +
        pdcpl::cdcl_type_printer(pdcpl::cdcl_type::gtype) + " my_type"
    }
  )
);

}  // namespace
