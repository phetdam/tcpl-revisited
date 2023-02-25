/**
 * @file bitwise_test.cc
 * @author Derek Huang
 * @brief bitwise.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/bitwise.h"

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture class for bitwise tests.
 */
class BitwiseTest : public ::testing::Test {};

/**
 * Test that `pdcpl_bitcount` works as expected.
 */
TEST_F(BitwiseTest, BitcountTest)
{
  EXPECT_EQ(6, pdcpl_bitcount(0b101011101));
  EXPECT_EQ(0, pdcpl_bitcount(0));
  EXPECT_EQ(4, pdcpl_bitcount(0b011101000));
}

}  // namespace
