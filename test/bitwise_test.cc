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
TEST_F(BitwiseTest, BitCountTest)
{
  EXPECT_EQ(6, pdcpl_bitcount(0b101011101));
  EXPECT_EQ(0, pdcpl_bitcount(0));
  EXPECT_EQ(4, pdcpl_bitcount(0b011101000));
}

/**
 * Test that `pdcpl_bitget_r` works as expected.
 */
TEST_F(BitwiseTest, SetBitsTest)
{
  unsigned int value;
  ASSERT_FALSE(pdcpl_getbits(0b110101001, &value, 5, 4));
  EXPECT_EQ(0b1010, value);
}

}  // namespace
