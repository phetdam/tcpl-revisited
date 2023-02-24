/**
 * @file string_test.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <algorithm>
#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "pdcpl/cpp/common.h"

namespace {

/**
 * Define a pair of static inline const hex string and value.
 *
 * The corresponding value will be named `[name]_val_` and is `constexpr` while
 * the string will be `PDCPL_CXX20_CONSTEXPR`.
 *
 * @param name Name to bind string to
 * @param value Hex value
 */
#define PDCPL_HEX_STRING_TEST_PAIR(name, value) \
  static inline PDCPL_CXX_20_CONSTEXPR std::string name{ \
    PDCPL_STRINGIFY(value) \
  }; \
  static inline constexpr std::intmax_t PDCPL_CONCAT(name, val_) = value

/**
 * Define a pair of static inline const nonnegative hex string and value.
 *
 * The corresponding value will be named `[name]_val_` and is `constexpr` while
 * the string will be `PDCPL_CXX20_CONSTEXPR`. Since a hex value without a
 * prefix is accepted, due to hex literal rules it must not be negative.
 *
 * @param name Name to bind string to
 * @param nn_value Non-negative hex value with the 0x or 0X prefix
 */
#define PDCPL_HEX_STRING_TEST_PAIR_NO_PREFIX(name, nn_value) \
  static inline PDCPL_CXX_20_CONSTEXPR std::string name{ \
    PDCPL_STRINGIFY(nn_value) \
  }; \
  static inline constexpr std::intmax_t \
  PDCPL_CONCAT(name, val_) = PDCPL_CONCAT(0x, nn_value)

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

  // string to count words, chars, lines for
  static inline PDCPL_CXX_20_CONSTEXPR std::string wc_string_{
    "A really long\n"
    "sequence of words over many\n"
    "long lines\n"
    "\n"
    "Also a bad\n"
    "haiku"
  };
  // number of words, chars, lines
  static inline constexpr std::size_t wc_words_ = 14;
// in C++20, use constexpr size() for this
#if __cplusplus >= PDCPL_CXX_20
  static inline constexpr std::size_t wc_chars_ = wc_string_.size() - 1;
#else
  static inline constexpr std::size_t wc_chars_ = 70;
#endif  // __cplusplus < PDCPL_CXX20
  static inline constexpr std::size_t wc_lines_ = 6;

  // string to reverse
  static inline PDCPL_CXX_20_CONSTEXPR std::string rev_string_{
    "hello nice to meet you"
  };

  // strings to convert to integer hex values and their corresponding values
  PDCPL_HEX_STRING_TEST_PAIR(hex_beef_, 0xdeadbeef);
  PDCPL_HEX_STRING_TEST_PAIR(hex_neg_, -0xAFE12);
  PDCPL_HEX_STRING_TEST_PAIR(hex_mess_, -0XaE098FbD0);
  PDCPL_HEX_STRING_TEST_PAIR_NO_PREFIX(hex_nop_, 34343dae);
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

/**
 * Test that `pdcpl_strwc` works as expected.
 */
TEST_F(StringTest, WordCountTest)
{
  pdcpl_wcresults res;
  // if something goes wrong, the test will abort
  ASSERT_FALSE(pdcpl_strwc(wc_string_.c_str(), &res));
  EXPECT_EQ(res.nc, wc_chars_);
  EXPECT_EQ(res.nl, wc_lines_);
  EXPECT_EQ(res.nw, wc_words_);
}

/**
 * Test that `pdcpl_strrev` works as expected.
 */
TEST_F(StringTest, StringReverseTest)
{
  // actual reversed string + its size
  char *act_rev;
  std::size_t n_act_rev;
  ASSERT_FALSE(pdcpl_strrev(rev_string_.c_str(), &act_rev, &n_act_rev));
  // expected reversed string
  std::string exp_rev{rev_string_.rbegin(), rev_string_.rend()};
  // contents/size should match
  EXPECT_EQ(exp_rev, std::string{act_rev});
  EXPECT_EQ(exp_rev.size(), n_act_rev);
}

/**
 * Test that `pdcpl_htoj` works as expected.
 */
TEST_F(StringTest, HexConvertTest)
{
  intmax_t value;
  // 0xdeadbeef
  ASSERT_FALSE(pdcpl_htoj(hex_beef_.c_str(), &value));
  EXPECT_EQ(hex_beef_val_, value);
  // -0xafe12
  ASSERT_FALSE(pdcpl_htoj(hex_neg_.c_str(), &value));
  EXPECT_EQ(hex_neg_val_, value);
  // -0xae098fbd0
  ASSERT_FALSE(pdcpl_htoj(hex_mess_.c_str(), &value));
  EXPECT_EQ(hex_mess_val_, value);
  // 0x34343dae
  ASSERT_FALSE(pdcpl_htoj(hex_nop_.c_str(), &value));
  EXPECT_EQ(hex_nop_val_, value);
}

}  // namespace
