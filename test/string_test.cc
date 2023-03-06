/**
 * @file string_test.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <string>

#include <gtest/gtest.h>

#include "pdcpl/cpp/common.h"

namespace {

/**
 * Define a pair of static inline const[expr] hex string and value.
 *
 * The corresponding value will be named `[name]val_` and is `constexpr` while
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
 * Define a pair of static inline const[expr] nonnegative hex string and value.
 *
 * The corresponding value will be named `[name]val_` and is `constexpr` while
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
 * Define static inline const[expr] strings for testing string squeezing.
 *
 * The strings are `PDCPL_CXX20_CONSTEXPR`, with the original string bound to
 * `name`, `delete_chars` being bound to `[name]ds_`, and the expected
 * `squeezed` being bound to `[name]sq_`.
 *
 * @param name Base name for the strings
 * @param orig Original string
 * @param delete_chars String with chars to remove from original string
 * @param squeezed Expected squeezed string, i.e. w/o chars from `delete_chars`
 */
#define PDCPL_SQUEEZE_STRING_TEST_PARAMS(name, orig, delete_chars, squeezed) \
  static inline PDCPL_CXX_20_CONSTEXPR std::string name{orig}; \
  static inline PDCPL_CXX_20_CONSTEXPR std::string \
  PDCPL_CONCAT(name, ds_){delete_chars}; \
  static inline PDCPL_CXX_20_CONSTEXPR std::string \
  PDCPL_CONCAT(name, sq_){squeezed}

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
  static inline PDCPL_CXX_20_CONSTEXPR std::size_t wc_chars_ = wc_string_.size();
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

  // strings to squeeze, chars to delete, and their squeezed values
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_1_, "totally", "holy", "tta");
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_2_, "normally", "", "normally");
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_3_, "Veronica", "acinoVer", "");
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
  // string is on heap, clean up
  std::free(act_rev);
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

/**
 * Test that `pdcpl_strsqueeze` works as expected.
 */
TEST_F(StringTest, SqueezeTest)
{
  // must be freed after every successful call
  char *res;
  // normal case
  ASSERT_FALSE(pdcpl_strsq(strsq_1_.c_str(), &res, strsq_1_ds_.c_str()));
  EXPECT_EQ(strsq_1_sq_, std::string{res});
  std::free(res);
  // no chars to strip out
  ASSERT_FALSE(pdcpl_strsq(strsq_2_.c_str(), &res, strsq_2_ds_.c_str()));
  EXPECT_EQ(strsq_2_sq_, std::string{res});
  std::free(res);
  // strip all chars out
  ASSERT_FALSE(pdcpl_strsq(strsq_3_.c_str(), &res, strsq_3_ds_.c_str()));
  EXPECT_EQ(strsq_3_sq_, std::string{res});
  std::free(res);
}

/**
 * Test that `pdcpl_tolower` works as expected.
 */
TEST_F(StringTest, ToLowerTest)
{
  std::string orig_string{"STOP the SHOUTING!"};
  std::string lower_string = orig_string;
  // std::tolower
  std::for_each(
    lower_string.begin(),
    lower_string.end(),
    [](auto c) { return static_cast<decltype(c)>(std::tolower(c)); }
  );
  // pdcpl_tolower
  std::for_each(
    orig_string.begin(),
    orig_string.end(),
    [](auto c) { return pdcpl_tolower(c); }
  );
  EXPECT_EQ(lower_string, orig_string);
}

/**
 * Test that `pdcpl_strexpand` works as expected.
 */
TEST_F(StringTest, StringExpandTest)
{
  std::string orig{"-hello a-z0-9A-Zb-d what's up-"};
  // broken into chunks so each range is more easily delineated
  std::string expanded{
    "-hello "
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "bcd"
    " what's up-"
  };
  // expected string and number of chars written
  char* res;
  std::size_t res_size;
  ASSERT_FALSE(pdcpl_strexpand(orig.c_str(), &res, &res_size));
  // contents and reported length should match
  EXPECT_EQ(expanded, std::string{res});
  EXPECT_EQ(expanded.size(), res_size);
  // string is on heap, clean up
  std::free(res);
}

/**
 * Test that `pdcpl_htoj` works as expected.
 */
TEST_F(StringTest, IntToCharConvertTest)
{
  // int to convert and expected result
  int x = -282813239;
  std::string x_str{std::to_string(x)};
  // pdcpl_jtoa result and result length
  char *res;
  std::size_t res_size;
  ASSERT_FALSE(pdcpl_jtoa(x, &res, &res_size));
  // compare + clean up
  EXPECT_EQ(x_str, std::string{res});
  EXPECT_EQ(x_str.size(), res_size);
  std::free(res);
}

/**
 * Test that `pdcpl_strfind` works as expected.
 */
TEST_F(StringTest, StringFindTest)
{
  // original string, string to search for
  std::string s{"hello there was a man"};
  std::string ss{"re was"};
  // expected index location + actual index location
  auto exp_loc = s.find(ss);
  std::size_t act_loc;
  ASSERT_FALSE(pdcpl_strfind(s.c_str(), ss.c_str(), &act_loc));
  // exp_loc and act_loc should match
  EXPECT_EQ(exp_loc, act_loc);
  // now we try with a string that is not in s, act_loc should be SIZE_MAX
  ASSERT_FALSE(pdcpl_strfind(s.c_str(), "unfindable", &act_loc));
  EXPECT_EQ(SIZE_MAX, act_loc);
}

/**
 * Test that `pdcpl_strrfind` works as expected.
 */
TEST_F(StringTest, StringRevFindTest)
{
  // original string, string to search for
  std::string s{"something to search in to search from the right"};
  std::string ss{"search "};
  // expected index location + actual index location
  auto exp_loc = s.rfind(ss);
  std::size_t act_loc;
  ASSERT_FALSE(pdcpl_strrfind(s.c_str(), ss.c_str(), &act_loc));
  // exp_loc and act_loc should match
  EXPECT_EQ(exp_loc, act_loc);
  // now we try with a string that is not in s, act_loc should be SIZE_MAX
  ASSERT_FALSE(pdcpl_strrfind(s.c_str(), "unfindable", &act_loc));
  EXPECT_EQ(SIZE_MAX, act_loc);
}
}  // namespace
