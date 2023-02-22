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
class MiscTest : public ::testing::Test {
protected:
  // freezing point in Celsius and Fahrenheit
  static inline constexpr double freezing_c_ = 0;
  static inline constexpr double freezing_f_ = 32;
};

/**
 * Test that Fahrenheit to Celsius conversion and vice versa work as expected.
 */
TEST_F(MiscTest, TempTest)
{
  // 0 C* is same as 32 F*
  EXPECT_DOUBLE_EQ(freezing_c_, pdcpl_f2ctemp(freezing_f_));
  EXPECT_DOUBLE_EQ(freezing_f_, pdcpl_c2ftemp(freezing_c_));
}

}  // namespace
