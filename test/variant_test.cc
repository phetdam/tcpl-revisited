/**
 * @file variant_test.cc
 * @author Derek Huang
 * @brief variant.(c|h) unit test
 * @copyright MIT License
 */

#include "pdcpl/variant.h"

#include <gtest/gtest.h>

#include <cerrno>
#include <cstdint>
#include <string>

namespace {

/**
 * Main test fixture for variant tests.
 */
class VariantTest : public ::testing::Test {};

/**
 * Test that variant works with char.
 */
TEST_F(VariantTest, CharTest)
{
  constexpr char value = 'c';
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(char)(&vt, value));
  EXPECT_EQ(value, vt.data.c);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with int.
 */
TEST_F(VariantTest, IntTest)
{
  constexpr int value = 15;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(int)(&vt, value));
  EXPECT_EQ(value, vt.data.i);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with unsigned int.
 */
TEST_F(VariantTest, UintTest)
{
  constexpr unsigned int value = 166U;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(uint)(&vt, value));
  EXPECT_EQ(value, vt.data.u);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with `size_t`.
 */
TEST_F(VariantTest, SizeTest)
{
  constexpr std::size_t value = 300U;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(size)(&vt, value));
  EXPECT_EQ(value, vt.data.z);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with unsigned int.
 */
TEST_F(VariantTest, DoubleTest)
{
  constexpr double value = 15.6;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(double)(&vt, value));
  EXPECT_EQ(value, vt.data.d);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with unsigned int.
 */
TEST_F(VariantTest, FloatTest)
{
  constexpr float value = -45.6f;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(float)(&vt, value));
  EXPECT_EQ(value, vt.data.f);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with owned string.
 */
TEST_F(VariantTest, StringTest)
{
  constexpr const char* value = "October";
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(string)(&vt, value));
  EXPECT_STREQ(value, vt.data.s);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with unowned string.
 */
TEST_F(VariantTest, StringRefTest)
{
  // PDCPL_VARIANT_INIT(string_ref) value type is char*, not const char*, so we
  // create a static char array with the contains of the string literal
  static char value[] = "hello";
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(string_ref)(&vt, value));
  EXPECT_STREQ(value, vt.data.s);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that `pdcpl_variant_free` errors approriately.
 */
TEST_F(VariantTest, FreeErrorTest)
{
  // purposefully crafted variant "owning" memory
  pdcpl_variant vt;
  vt.flags = pdcpl_variant_float | pdcpl_variant_mem_own;
  // errors on NULL
  EXPECT_EQ(-EINVAL, pdcpl_variant_free(nullptr));
  // errors if type owning memory is not one of those known to own memory
  EXPECT_EQ(-EINVAL, pdcpl_variant_free(&vt));
}

}  // namespace
