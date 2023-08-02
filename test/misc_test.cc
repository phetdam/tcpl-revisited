/**
 * @file misc_test.cc
 * @author Derek Huang
 * @brief misc.h unit tests
 * @copyright MIT License
 */

#include <gtest/gtest.h>

#include "pdcpl/misc.h"

namespace {

/**
 * Misc functions test fixture class.
 */
class MiscTest : public ::testing::Test {};

/**
 * Test that Fahrenheit to Celsius conversion and vice versa work as expected.
 */
TEST_F(MiscTest, TempTest)
{
  // freezing point in Celsius and Fahrenheit
  constexpr double freezing_c = 0;
  constexpr double freezing_f = 32;
  // 0 C* is same as 32 F*
  EXPECT_DOUBLE_EQ(freezing_c, pdcpl_f2ctemp(freezing_f));
  EXPECT_DOUBLE_EQ(freezing_f, pdcpl_c2ftemp(freezing_c));
}

}  // namespace
