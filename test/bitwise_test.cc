/**
 * @file bitwise_test.cc
 * @author Derek Huang
 * @brief bitwise.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/bitwise.h"

#include <climits>
#include <type_traits>

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
 * Test that `pdcpl_getbits` works as expected.
 */
TEST_F(BitwiseTest, GetBitsTest)
{
  unsigned int value;
  ASSERT_FALSE(pdcpl_getbits(0b110101001, &value, 5, 4));
  EXPECT_EQ(0b1010, value);
}

/**
 * Test that `pdcpl_setbits` works as expected.
 */
TEST_F(BitwiseTest, SetBitsTest)
{
  unsigned int value;
  ASSERT_FALSE(pdcpl_setbits(0b1010100010101, &value, 6, 4, 0b0101001101));
  EXPECT_EQ(0b1010101101101, value);
}

/**
 * Test that `pdcpl_invbits` works as expected.
 */
TEST_F(BitwiseTest, InvBitsTest)
{
  unsigned int value;
  ASSERT_FALSE(pdcpl_invbits(0b010110101, &value, 5, 4));
  EXPECT_EQ(0b010001001, value);
}

/**
 * Test that `pdcpl_rrotbits` works as expected.
 */
TEST_F(BitwiseTest, RRotBitsTest)
{
  auto value = pdcpl_rrotbits(0b1011101, 5);
  // bits to shift rotated bits left by to put them at most significant bits.
  // this lets us avoid explicitly relying on the size of unsigned int.
  auto shift = static_cast<unsigned short>(CHAR_BIT * sizeof(value) - 5);
  EXPECT_EQ((0b11101 << shift) + 0b10, value);
}

}  // namespace
