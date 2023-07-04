/**
 * @file string_param_test.cc
 * @author Derek Huang
 * @brief string.(c|h) parametrized unit tests
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <cstdio>
#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture for parametrized testing of `pdcpl_next_tab_(stop|size)`.
 */
class TabStopStringTest : public ::testing::TestWithParam<
  std::tuple<std::size_t, unsigned int, std::size_t>> {};

INSTANTIATE_TEST_SUITE_P(
  TabStopParams,
  TabStopStringTest,
  ::testing::Values(
    TabStopStringTest::ParamType{17, 5, 20},
    TabStopStringTest::ParamType{30, 7, 35},
    TabStopStringTest::ParamType{15, 4, 16},
    TabStopStringTest::ParamType{20, 3, 21}
  )
);

/**
 * Test that `pdcpl_next_tab_(stop|size)` work as expected.
 */
TEST_P(TabStopStringTest, ParamTest)
{
  // current column position + tab size + next expected tab stop
  const auto cur_col = std::get<0>(GetParam());
  const auto tab_size = std::get<1>(GetParam());
  const auto next_tab_stop = std::get<2>(GetParam());
  // check position of next tab stop and next tab size
  EXPECT_EQ(next_tab_stop, pdcpl_next_tab_stop(cur_col, tab_size));
  EXPECT_EQ(next_tab_stop - cur_col, pdcpl_next_tab_size(cur_col, tab_size));
}

/**
 * Test fixture for parametrized testing of `pdcpl_htoj`.
 */
class HexConvertStringTest
  : public ::testing::TestWithParam<std::pair<std::string, std::intmax_t>> {};

INSTANTIATE_TEST_SUITE_P(
  HexConvertParams,
  HexConvertStringTest,
  ::testing::Values(
    HexConvertStringTest::ParamType{"0xdeadbeef", 0xdeadbeef},
    HexConvertStringTest::ParamType{"-0xAFE12", -0xafe12},
    HexConvertStringTest::ParamType{"-0XaE098FbD0", -0xae098fbd0},
    HexConvertStringTest::ParamType{"34343dae", 0x34343dae}
  )
);

/**
 * Test that `pdcpl_htoj` works as expected.
 */
TEST_P(HexConvertStringTest, ParamTest)
{
  // target intmax_t value to convert hex string to
  std::intmax_t value;
  ASSERT_FALSE(pdcpl_htoj(GetParam().first.c_str(), &value));
  EXPECT_EQ(GetParam().second, value);
}

/**
 * Test fixture for parametrized testing of `pdcpl_strsq`.
 */
class SqueezeStringTest : public ::testing::TestWithParam<
  std::tuple<std::string, std::string, std::string>> {};

INSTANTIATE_TEST_SUITE_P(
  SqueezeParams,
  SqueezeStringTest,
  ::testing::Values(
    SqueezeStringTest::ParamType{"totally", "holy", "tta"},
    SqueezeStringTest::ParamType{"normally", "", "normally"},
    SqueezeStringTest::ParamType{"Veronica", "acinoVer", ""}
  )
);

/**
 * Test that `pdcpl_strsq` works as expected.
 */
TEST_P(SqueezeStringTest, ParamTest)
{
  // must be freed after every successful call
  char *res;
  // input, chars to delete, expected result
  const auto& input = std::get<0>(GetParam());
  const auto& delete_chars = std::get<1>(GetParam());
  const auto& expected = std::get<2>(GetParam());
  // check equality and clean up
  ASSERT_FALSE(pdcpl_strsq(input.c_str(), &res, delete_chars.c_str()));
  EXPECT_EQ(expected, std::string{res});
  std::free(res);
}

}  // namespace
