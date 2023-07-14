/**
 * @file string_test_2.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests 2
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

#include "pdcpl/warnings.h"

namespace {

/**
 * Test fixture for parametrized testing of `pdcpl_jtoa`.
 */
class IntToCharConvertTest
  : public ::testing::TestWithParam<std::pair<int, std::string>> {
public:
  /**
   * Return an input for `IntToCharConvertTest` parametrized tests.
   *
   * @param x Input integer
   * @returns Pair of `x` and resulting string
   */
  static ParamType CreateInput(int x)
  {
    return {x, std::to_string(x)};
  }
};

/**
 * Test that `pdcpl_jtoa` works as expected.
 */
TEST_P(IntToCharConvertTest, Test)
{
  // int to convert and expected result
  const auto& [x, x_str] = GetParam();
  // pdcpl_jtoa result and result length
  char *res;
  std::size_t res_size;
  ASSERT_FALSE(pdcpl_jtoa(x, &res, &res_size));
  // compare + clean up
  EXPECT_EQ(x_str, std::string{res});
  EXPECT_EQ(x_str.size(), res_size);
  std::free(res);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  IntToCharConvertTest,
  ::testing::Values(
    IntToCharConvertTest::CreateInput(-282813239),
    IntToCharConvertTest::CreateInput(12312372),
    IntToCharConvertTest::CreateInput(-99101)
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_strfind`.
 */
class StringFindTest
  : public ::testing::TestWithParam<
      std::tuple<std::string, std::string, std::size_t>> {
public:
  /**
   * Return an input for `StringFindTest` parametrized tests.
   *
   * @param s Input string
   * @param ss String to search for
   * @returns Tuple of `s`, `ss`, and expected index location
   */
  static ParamType CreateInput(const std::string& s, const std::string& ss)
  {
    return {s, ss, s.find(ss)};
  }
};

/**
 * Test that `pdcpl_strfind` works as expected.
 */
TEST_P(StringFindTest, Test)
{
  // original string, string to search for, expected index location
  const auto& [s, ss, exp_loc] = GetParam();
  // get actual index location + compare
  std::size_t act_loc;
  ASSERT_FALSE(pdcpl_strfind(s.c_str(), ss.c_str(), &act_loc));
  EXPECT_EQ(exp_loc, act_loc);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  StringFindTest,
  ::testing::Values(
    StringFindTest::CreateInput("hello there was a man", "re was"),
    StringFindTest::CreateInput("master chief", "chief"),
    // string not in the input string, index is SIZE_MAX
    StringFindTest::CreateInput("no findable strings here", "oops"),
    StringFindTest::CreateInput("no string findable", "unfindable")
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_strrfind`.
 *
 * @todo May later remove inheritance from `StringTest`.
 */
class StringRevFindTest
  : public ::testing::TestWithParam<
      std::tuple<std::string, std::string, std::size_t>> {
public:
  /**
   * Return an input for `StringRevFindTest` parametrized tests.
   *
   * @param s Input string
   * @param ss String to search for
   * @returns Tuple of `s`, `ss`, and expected index location
   */
  static ParamType CreateInput(const std::string& s, const std::string& ss)
  {
    return {s, ss, s.rfind(ss)};
  }
};

/**
 * Test that `pdcpl_strrfind` works as expected.
 */
TEST_P(StringRevFindTest, Test)
{
  // original string, string to search for, expected index location
  const auto& [s, ss, exp_loc] = GetParam();
  // get actual index location + compare
  std::size_t act_loc;
  ASSERT_FALSE(pdcpl_strrfind(s.c_str(), ss.c_str(), &act_loc));
  EXPECT_EQ(exp_loc, act_loc);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  StringRevFindTest,
  ::testing::Values(
    StringRevFindTest::CreateInput("to search by right search", "search"),
    StringRevFindTest::CreateInput("not really searching correctly", "really"),
    StringRevFindTest::CreateInput("hello sweet world of sweetness", "sweet"),
    // string not in the input string, index is SIZE_MAX
    StringRevFindTest::CreateInput("nothing to find here", "string"),
    StringRevFindTest::CreateInput("string with lots of characters", "b")
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_concat`.
 */
class ConcatTest
  : public ::testing::TestWithParam<
      std::tuple<std::string, std::string, std::string>> {
public:
  /**
   * Return an input for `ConcatTest` parametrized tests.
   *
   * @param s1 First string
   * @param s2 Second string
   * @returns Tuple of `s1`, `s2`, and `s1 + s2`
   */
  static ParamType CreateInput(const std::string& s1, const std::string& s2)
  {
    return {s1, s2, s1 + s2};
  }
};

/**
 * Test that `pdcpl_concat` works as expected.
 */
TEST_P(ConcatTest, Test)
{
  // strings to concatenate + expected result
  const auto& [s1, s2, expected] = GetParam();
  // actual result + length
  char *res;
  std::size_t res_size;
  ASSERT_FALSE(pdcpl_strcat(s1.c_str(), s2.c_str(), &res, &res_size));
  // compare + free
  EXPECT_EQ(expected, std::string{res});
  EXPECT_EQ(expected.size(), res_size);
  std::free(res);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  ConcatTest,
  ::testing::Values(
    ConcatTest::CreateInput("first string", " second string"),
    ConcatTest::CreateInput("hello", " world"),
    ConcatTest::CreateInput("", "the result"),
    ConcatTest::CreateInput("the result", "")
  )
);

}  // namespace
