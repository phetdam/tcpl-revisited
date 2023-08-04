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
#include <cstring>
#include <string>
#include <tuple>

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
  // create a static char array from the contents of the string literal
  static char value[] = "hello";
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(string_ref)(&vt, value));
  EXPECT_STREQ(value, vt.data.s);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with owned void buffer.
 */
TEST_F(VariantTest, VoidTest)
{
  constexpr const char* value = "buffer text";
  const auto bufsiz = std::strlen(value) + 1;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(void)(&vt, value, bufsiz));
  EXPECT_STREQ(value, static_cast<const char*>(vt.data.v.b));
  EXPECT_EQ(bufsiz, vt.data.v.z);
  ASSERT_FALSE(pdcpl_variant_free(&vt));
}

/**
 * Test that variant works with unowned void buffer.
 */
TEST_F(VariantTest, VoidRefTest)
{
  // PDCPL_VARIANT_INIT(void_ref) value type is void*, not const void*, so we
  // again create a static char array from the contents of the string literal
  static char value[] = "unowned buffer text";
  const auto bufsiz = std::strlen(value) + 1;
  pdcpl_variant vt;
  ASSERT_FALSE(PDCPL_VARIANT_INIT(void_ref)(&vt, value, bufsiz));
  EXPECT_STREQ(value, (const char*) vt.data.v.b);
  EXPECT_EQ(bufsiz, vt.data.v.z);
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

/**
 * Test fixture for parametrized testing of `pdcpl_variant_shared_type`.
 */
class SharedTypeTest : public ::testing::TestWithParam<
  std::tuple<pdcpl_variant, pdcpl_variant, unsigned int>> {
public:
  /**
   * Create an input for `SharedTypeTest` parametrized tests.
   *
   * The returned `pdcpl_variant` structs do not have any data fields
   * initialized and are invalid for general use outside the associated tests.
   *
   * @param aflags Flags for first variant
   * @param bflags Flags for second variant
   * @param target Target flags; should only have a single type bit set
   */
  static ParamType
  CreateInput(unsigned int aflags, unsigned int bflags, unsigned int target)
  {
    pdcpl_variant va, vb;
    va.flags = aflags;
    vb.flags = bflags;
    return {va, vb, target};
  }
};

/**
 * Test that `pdcpl_variant_shared_type` works properly.
 */
TEST_P(SharedTypeTest, Test)
{
  const auto& [va, vb, expected] = GetParam();
  EXPECT_EQ(expected, pdcpl_variant_shared_type(&va, &vb));
}

INSTANTIATE_TEST_SUITE_P(
  VariantTest,
  SharedTypeTest,
  ::testing::Values(
    SharedTypeTest::CreateInput(pdcpl_variant_char, pdcpl_variant_int, 0U),
    SharedTypeTest::CreateInput(
      pdcpl_variant_uint, pdcpl_variant_uint, pdcpl_variant_uint
    ),
    SharedTypeTest::CreateInput(
      pdcpl_variant_string | pdcpl_variant_mem_own,
      pdcpl_variant_string | pdcpl_variant_mem_borrow,
      pdcpl_variant_string
    ),
    SharedTypeTest::CreateInput(
      pdcpl_variant_void | pdcpl_variant_mem_own,
      pdcpl_variant_string | pdcpl_variant_mem_borrow,
      0U
    ),
    SharedTypeTest::CreateInput(
      pdcpl_variant_string | pdcpl_variant_mem_borrow, pdcpl_variant_char, 0U
    )
  )
);

}  // namespace
