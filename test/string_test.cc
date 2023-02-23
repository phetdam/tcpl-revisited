/**
 * @file string_test.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <cstdint>

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture class for string tests.
 */
class StringTest : public ::testing::Test {
protected:
  // signed int and number of columns needed to fit this int
  static inline constexpr int col_int_ = 123513272;
  static inline constexpr unsigned short col_width_ = 9;
  // padding and number of columns needed to fit the int with said padding
  static inline constexpr unsigned short col_padding_ = 2;
  static inline constexpr unsigned short col_width_padded_{
    col_width_ + 2 * col_padding_
  };

  // current column position + tab size + next expected tab stop
  static inline constexpr std::size_t cur_col_ = 17;
  static inline constexpr unsigned int tab_size_ = 5;
  static inline constexpr std::size_t next_tab_stop_ = 20;
};

/**
 * Test that `pdcpl_printpwtd` and `pdcpl_printwtd` work as expected.
 */
TEST_F(StringTest, PrintWidthTest)
{
  EXPECT_EQ(col_width_padded_, pdcpl_printpwtd(col_int_, col_padding_));
  EXPECT_EQ(col_width_, pdcpl_printwtd(col_int_));
}

/**
 * Test that `pdcpl_next_tab_stop` and `pdcpl_next_tab_size` work as expected.
 */
TEST_F(StringTest, TabStopTest)
{
  EXPECT_EQ(next_tab_stop_, pdcpl_next_tab_stop(cur_col_, tab_size_));
  EXPECT_EQ(next_tab_stop_ - cur_col_, pdcpl_next_tab_size(cur_col_, tab_size_));
}

}  // namespace
