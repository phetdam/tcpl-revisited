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
 * Test fixture class for string tests.
 */
class StringTest : public ::testing::Test {};

/**
 * Test that `pdcpl_printpwtd` and `pdcpl_printwtd` work as expected.
 */
TEST_F(StringTest, PrintWidthTest)
{
  // signed int and number of columns needed to fit this int
  static constexpr int col_int = 123513272;
  static constexpr unsigned short col_width = 9;
  // padding and number of columns needed to fit the int with said padding
  static constexpr unsigned short col_padding = 2;
  static constexpr unsigned short col_width_padded = col_width + 2 * col_padding;
  // padded and non-padded widths should match up
  EXPECT_EQ(col_width_padded, pdcpl_printpwtd(col_int, col_padding));
  EXPECT_EQ(col_width, pdcpl_printwtd(col_int));
}

/**
 * Test that `pdcpl_next_tab_stop` and `pdcpl_next_tab_size` work as expected.
 */
TEST_F(StringTest, TabStopTest)
{
  // current column position + tab size + next expected tab stop
  static constexpr std::size_t cur_col = 17;
  static constexpr unsigned int tab_size = 5;
  static constexpr std::size_t next_tab_stop = 20;
  // check position of next tab stop and next tab size
  EXPECT_EQ(next_tab_stop, pdcpl_next_tab_stop(cur_col, tab_size));
  EXPECT_EQ(next_tab_stop - cur_col, pdcpl_next_tab_size(cur_col, tab_size));
}

/**
 * Test that `pdcpl_strwc` works as expected.
 */
TEST_F(StringTest, WordCountTest)
{
  // string to count words, chars, lines for
  static PDCPL_CONSTEXPR_20 std::string wc_string{
    "A really long\n"
    "sequence of words over many\n"
    "long lines\n"
    "\n"
    "Also a bad\n"
    "haiku"
  };
  // number of words, chars, lines
  static constexpr std::size_t wc_words = 14;
  static PDCPL_CONSTEXPR_20 std::size_t wc_chars = wc_string.size();
  static constexpr std::size_t wc_lines = 6;
  // word count results
  pdcpl_wcresults res;
  // if something goes wrong, the test will abort
  ASSERT_FALSE(pdcpl_strwc(wc_string.c_str(), &res));
  EXPECT_EQ(res.nc, wc_chars);
  EXPECT_EQ(res.nl, wc_lines);
  EXPECT_EQ(res.nw, wc_words);
}

/**
 * Test that `pdcpl_strrev` works as expected.
 */
TEST_F(StringTest, StringReverseTest)
{
  // string to reverse
  static PDCPL_CONSTEXPR_20 std::string rev_string{"hello nice to meet you"};
  // actual reversed string + its size
  char *act_rev;
  std::size_t n_act_rev;
  ASSERT_FALSE(pdcpl_strrev(rev_string.c_str(), &act_rev, &n_act_rev));
  // expected reversed string
  std::string exp_rev{rev_string.rbegin(), rev_string.rend()};
  // contents/size should match
  EXPECT_EQ(exp_rev, std::string{act_rev});
  EXPECT_EQ(exp_rev.size(), n_act_rev);
  // string is on heap, clean up
  std::free(act_rev);
}

/**
 * Define a pair of static const[expr] hex string and value.
 *
 * The corresponding value will be named `[name]_val` and is `constexpr` while
 * the string will be `PDCPL_CONSTEXPR_20`.
 *
 * @param name Name to bind string to
 * @param value Hex value
 */
#define PDCPL_HEX_STRING_TEST_PAIR(name, value) \
  static PDCPL_CONSTEXPR_20 std::string name{PDCPL_STRINGIFY(value)}; \
  static constexpr std::intmax_t PDCPL_CONCAT(name, _val) = value

/**
 * Define a pair of static const[expr] nonnegative hex string and value.
 *
 * The corresponding value will be named `[name]_val` and is `constexpr` while
 * the string will be `PDCPL_CONSTEXPR_20`. Since a hex value without a prefix
 * is accepted, due to hex literal rules it must not be negative.
 *
 * @param name Name to bind string to
 * @param nn_value Non-negative hex value with the 0x or 0X prefix
 */
#define PDCPL_HEX_STRING_TEST_PAIR_NO_PREFIX(name, nn_value) \
  static PDCPL_CONSTEXPR_20 std::string name{PDCPL_STRINGIFY(nn_value)}; \
  static constexpr std::intmax_t \
  PDCPL_CONCAT(name, _val) = PDCPL_CONCAT(0x, nn_value)

/**
 * Test that `pdcpl_htoj` works as expected.
 */
TEST_F(StringTest, HexConvertTest)
{
  // strings to convert to integer hex values and their corresponding values
  PDCPL_HEX_STRING_TEST_PAIR(hex_beef, 0xdeadbeef);
  PDCPL_HEX_STRING_TEST_PAIR(hex_neg, -0xAFE12);
  PDCPL_HEX_STRING_TEST_PAIR(hex_mess, -0XaE098FbD0);
  PDCPL_HEX_STRING_TEST_PAIR_NO_PREFIX(hex_nop, 34343dae);
  // target intmax_t value to convert hex string to
  intmax_t value;
  // 0xdeadbeef
  ASSERT_FALSE(pdcpl_htoj(hex_beef.c_str(), &value));
  EXPECT_EQ(hex_beef_val, value);
  // -0xafe12
  ASSERT_FALSE(pdcpl_htoj(hex_neg.c_str(), &value));
  EXPECT_EQ(hex_neg_val, value);
  // -0xae098fbd0
  ASSERT_FALSE(pdcpl_htoj(hex_mess.c_str(), &value));
  EXPECT_EQ(hex_mess_val, value);
  // 0x34343dae
  ASSERT_FALSE(pdcpl_htoj(hex_nop.c_str(), &value));
  EXPECT_EQ(hex_nop_val, value);
}

/**
 * Define static const[expr] strings for testing string squeezing.
 *
 * The strings are `PDCPL_CONSTEXPR_20`, with the original string bound to
 * `name`, `delete_chars` being bound to `[name]_ds`, and the expected
 * `squeezed` being bound to `[name]_sq`.
 *
 * @param name Base name for the strings
 * @param orig Original string
 * @param delete_chars String with chars to remove from original string
 * @param squeezed Expected squeezed string, i.e. w/o chars from `delete_chars`
 */
#define PDCPL_SQUEEZE_STRING_TEST_PARAMS(name, orig, delete_chars, squeezed) \
  static PDCPL_CONSTEXPR_20 std::string name{orig}; \
  static PDCPL_CONSTEXPR_20 std::string PDCPL_CONCAT(name, _ds){delete_chars}; \
  static PDCPL_CONSTEXPR_20 std::string PDCPL_CONCAT(name, _sq){squeezed}

/**
 * Test that `pdcpl_strsqueeze` works as expected.
 */
TEST_F(StringTest, SqueezeTest)
{
  // strings to squeeze, chars to delete, and their squeezed values
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_1, "totally", "holy", "tta");
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_2, "normally", "", "normally");
  PDCPL_SQUEEZE_STRING_TEST_PARAMS(strsq_3, "Veronica", "acinoVer", "");
  // must be freed after every successful call
  char *res;
  // normal case
  ASSERT_FALSE(pdcpl_strsq(strsq_1.c_str(), &res, strsq_1_ds.c_str()));
  EXPECT_EQ(strsq_1_sq, std::string{res});
  std::free(res);
  // no chars to strip out
  ASSERT_FALSE(pdcpl_strsq(strsq_2.c_str(), &res, strsq_2_ds.c_str()));
  EXPECT_EQ(strsq_2_sq, std::string{res});
  std::free(res);
  // strip all chars out
  ASSERT_FALSE(pdcpl_strsq(strsq_3.c_str(), &res, strsq_3_ds.c_str()));
  EXPECT_EQ(strsq_3_sq, std::string{res});
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
  static PDCPL_CONSTEXPR_20 std::string orig{"-hello a-z0-9A-Zb-d what's up-"};
  // broken into chunks so each range is more easily delineated
  static PDCPL_CONSTEXPR_20 std::string expanded{
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
 * Test that `pdcpl_jtoa` works as expected.
 */
TEST_F(StringTest, IntToCharConvertTest)
{
  // int to convert and expected result
  static constexpr int x = -282813239;
  static const std::string x_str{std::to_string(x)};
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
  static PDCPL_CONSTEXPR_20 std::string s{"hello there was a man"};
  static PDCPL_CONSTEXPR_20 std::string ss{"re was"};
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
  static PDCPL_CONSTEXPR_20 std::string s{
    "something to search in to search from the right"
  };
  static PDCPL_CONSTEXPR_20 std::string ss{"search "};
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

/**
 * Test that `pdcpl_concat` works as expected.
 */
TEST_F(StringTest, ConcatTest)
{
  // strings to concatenate + expected result
  static PDCPL_CONSTEXPR_20 std::string s1{"first string"};
  static PDCPL_CONSTEXPR_20 std::string s2{"second string"};
  static PDCPL_CONSTEXPR_20 std::string expected = s1 + s2;
  // actual result + length
  char *res;
  std::size_t res_size;
  ASSERT_FALSE(pdcpl_strcat(s1.c_str(), s2.c_str(), &res, &res_size));
  // compare + free
  EXPECT_EQ(expected, std::string{res});
  EXPECT_EQ(expected.size(), res_size);
  std::free(res);
}

}  // namespace
