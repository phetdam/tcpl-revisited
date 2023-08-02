/**
 * @file math_test.cc
 * @author Derek Huang
 * @brief math.h unit tests
 * @copyright MIT License
 */

#include "pdcpl/math.h"

#include <limits>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture for `pdcpl_double_near` parametrized testing.
 */
class DoubleNearTest : public ::testing::TestWithParam<
  std::tuple<double, double, double, bool>> {};

/**
 * Test that `pdcpl_double_near` works as expected.
 */
TEST_P(DoubleNearTest, Test)
{
  // first, second, eps, expected truth value
  const auto [a, b, eps, expected] = GetParam();
  EXPECT_EQ(expected, pdcpl_double_near(a, b, eps));
}

INSTANTIATE_TEST_SUITE_P(
  MathTest,
  DoubleNearTest,
  ::testing::Values(
    DoubleNearTest::ParamType{1.4, 1.5, 0.2, true},
    DoubleNearTest::ParamType{1.4, 1.6, 1e-03, false},
    DoubleNearTest::ParamType{5., 1., 5., true},
    DoubleNearTest::ParamType{
      1.2 + 10 * std::numeric_limits<double>::epsilon(),
      1.2,
      8 * std::numeric_limits<double>::epsilon(),
      false
    },
    DoubleNearTest::ParamType{16.1, 16.0999, 1e-02, true}
  )
);

}  // namespace
