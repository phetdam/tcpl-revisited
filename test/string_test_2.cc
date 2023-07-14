/**
 * @file string_test_2.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests 2
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <cstdio>
#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

#include "pdcpl/warnings.h"

namespace {

/**
 * Test fixture for parametrized testing of `pdcpl_print[p]wtd`.
 */
class PrintWidthTest : public ::testing::TestWithParam<
  std::tuple<std::ptrdiff_t, unsigned short, unsigned short>> {
public:
  /**
   * Return an input for `PrintWidthTest` parametrized tests.
   *
   * @param value Input value
   * @param padding Print width padding
   * @returns Tuple of input value, print width padding, and padded print width
   */
  static ParamType CreateInput(std::ptrdiff_t value, unsigned short padding)
  {
    return {
      value,
      padding,
      // cast entire expression as 2 * padding is promoted to int so MSVC warns
      static_cast<unsigned short>(std::to_string(value).size() + 2 * padding)
    };
  }
};

/**
 * Test that `pdcpl_printpwtd` and `pdcpl_printwtd` work as expected.
 */
TEST_P(PrintWidthTest, ParamTest)
{
  // input, padding, padded print width
  const auto [value, padding, padded_width] = GetParam();
  // padded and non-padded widths (-2 * padding) should match up
  EXPECT_EQ(padded_width, pdcpl_printpwtd(value, padding));
  EXPECT_EQ(padded_width - 2 * padding, pdcpl_printwtd(value));
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  PrintWidthTest,
  ::testing::Values(
    PrintWidthTest::CreateInput(123513272, 2),
    PrintWidthTest::CreateInput(-1991823, 3),
    PrintWidthTest::CreateInput(8787822, 6)
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_next_tab_(stop|size)`.
 */
class TabStopTest : public ::testing::TestWithParam<
  std::tuple<std::size_t, unsigned int, std::size_t>> {};

/**
 * Test that `pdcpl_next_tab_(stop|size)` work as expected.
 */
TEST_P(TabStopTest, ParamTest)
{
  // current column position + tab size + next expected tab stop
  const auto [cur_col, tab_size, next_tab_stop] = GetParam();
  // check position of next tab stop and next tab size
  EXPECT_EQ(next_tab_stop, pdcpl_next_tab_stop(cur_col, tab_size));
  EXPECT_EQ(next_tab_stop - cur_col, pdcpl_next_tab_size(cur_col, tab_size));
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  TabStopTest,
  ::testing::Values(
    TabStopTest::ParamType{17, 5, 20},
    TabStopTest::ParamType{30, 7, 35},
    TabStopTest::ParamType{15, 4, 16},
    TabStopTest::ParamType{20, 3, 21}
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_htoj`.
 */
class HexConvertTest : public ::testing::TestWithParam<
  std::pair<std::string, std::intmax_t>> {};

/**
 * Test that `pdcpl_htoj` works as expected.
 */
TEST_P(HexConvertTest, ParamTest)
{
  // target intmax_t value to convert hex string to
  std::intmax_t value;
  ASSERT_FALSE(pdcpl_htoj(GetParam().first.c_str(), &value));
  EXPECT_EQ(GetParam().second, value);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  HexConvertTest,
  ::testing::Values(
    HexConvertTest::ParamType{"0xdeadbeef", 0xdeadbeef},
    HexConvertTest::ParamType{"-0xAFE12", -0xafe12},
    HexConvertTest::ParamType{"-0XaE098FbD0", -0xae098fbd0},
    HexConvertTest::ParamType{"34343dae", 0x34343dae}
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_strsq`.
 */
class SqueezeTest : public ::testing::TestWithParam<
  std::tuple<std::string, std::string, std::string>> {};

/**
 * Test that `pdcpl_strsq` works as expected.
 */
TEST_P(SqueezeTest, ParamTest)
{
  // must be freed after every successful call
  char *res;
  // input, chars to delete, expected result
  const auto& [input, delete_chars, expected] = GetParam();
  // check equality and clean up
  ASSERT_FALSE(pdcpl_strsq(input.c_str(), &res, delete_chars.c_str()));
  EXPECT_EQ(expected, std::string{res});
  std::free(res);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  SqueezeTest,
  ::testing::Values(
    SqueezeTest::ParamType{"totally", "holy", "tta"},
    SqueezeTest::ParamType{"normally", "", "normally"},
    SqueezeTest::ParamType{"Veronica", "acinoVer", ""}
  )
);

}  // namespace
