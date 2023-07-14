/**
 * @file string_test_1.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests 1
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

#include "pdcpl/features.h"
#include "pdcpl/cpp/common.h"
#include "pdcpl/cpp/memory.h"

// tests reading from an in-memory buffer as a FILE* need fmemopen
#ifdef PDCPL_POSIX_1_2008
#include <errno.h>
#include <stdio.h>  // fmemopen

#include <cstring>  // std::strtok
#include <memory>
#endif  // PDCPL_POSIX_1_2008

namespace {

/**
 * Test fixture class for string tests.
 */
class StringTest : public ::testing::Test {
protected:
  // string to count words, chars, lines for
  static inline PDCPL_CONSTEXPR_20 std::string wc_string_{
    "A really long\n"
    "sequence of words over many\n"
    "long lines\n"
    "\n"
    "Also a bad\n"
    "haiku"
  };
  // number of words, chars, lines
  static inline constexpr std::size_t wc_words_ = 14;
  static inline PDCPL_CONSTEXPR_20 std::size_t wc_chars_ = wc_string_.size();
  static inline constexpr std::size_t wc_lines_ = 6;
};

/**
 * Test that `pdcpl_strwc` works as expected.
 */
TEST_F(StringTest, StringWordCountTest)
{
  pdcpl_wcresults res;
  // if something goes wrong, the test will abort
  ASSERT_FALSE(pdcpl_strwc(wc_string_.c_str(), &res));
  EXPECT_EQ(res.nc, wc_chars_);
  EXPECT_EQ(res.nl, wc_lines_);
  EXPECT_EQ(res.nw, wc_words_);
}

#ifdef PDCPL_POSIX_1_2008
/**
 * Test that `pdcpl_fwc` works as expected.
 *
 * @note Requires POSIX.1-2008 `fmemopen` to be available.
 */
TEST_F(StringTest, FileWordCountTest)
{
  pdcpl_wcresults res;
  // get managed FILE* from the string's char buffer (no null terminator)
  pdcpl::unique_file file{fmemopen((void*) wc_string_.c_str(), wc_chars_, "r")};
  ASSERT_TRUE(file) << "fmemopen error: " << strerror(errno);
  // check that results are as expected
  ASSERT_FALSE(pdcpl_fwc(file.get(), &res));
  EXPECT_EQ(res.nc, wc_chars_);
  EXPECT_EQ(res.nl, wc_lines_);
  EXPECT_EQ(res.nw, wc_words_);
}

/**
 * Split a string by delimiters into a substring vector.
 *
 * @param str String to split by delimiters
 * @param delims Delimiters to split string with
 * @returns `std::vector<std::string>` of the component substrings
 */
auto string_split(const std::string& str, const char* delims)
{
  // buffer to hold a copy of the string contents
  auto buffer = std::make_unique<char[]>(str.size() + 1);
  std::strcpy(buffer.get(), str.c_str());
  // vector to put the substrings in
  std::vector<std::string> substrs;
  // emplace tokens from buffer into the vector
  char *token = std::strtok(buffer.get(), delims);
  while (token) {
    substrs.emplace_back(token);
    token = std::strtok(nullptr, delims);
  }
  return substrs;
}

/**
 * Test that `pdcpl_getword` works as expected.
 *
 * @note Requires POSIX.1-2008 `fmemopen` to be available.
 */
TEST_F(StringTest, FileGetWordTest)
{
  // get managed FILE* from the string's char buffer (no null terminator)
  pdcpl::unique_file file{fmemopen((void*) wc_string_.c_str(), wc_chars_, "r")};
  ASSERT_TRUE(file) << "fmemopen error: " << strerror(errno);
  // expected vector of words to get
  const auto exp_words = string_split(wc_string_, " \n");
  // word buffer, word length, vector of words, status
  char* word;
  std::size_t word_len;
  std::vector<std::string> act_words;
  int status;
  // loop until word length is zero (word will be NULL then)
  while (status = pdcpl_getword(file.get(), &word, &word_len), word_len) {
    ASSERT_FALSE(status) << "pdcpl_getword error: " << strerror(-status);
    // append to vector + clean up
    act_words.emplace_back(word);
    std::free(word);
  }
  // compare values
  EXPECT_EQ(exp_words, act_words);
}
#endif  // PDCPL_POSIX_1_2008

/**
 * Test that `pdcpl_strrev` works as expected.
 */
TEST_F(StringTest, StringReverseTest)
{
  // string to reverse
  static PDCPL_CONSTEXPR_20 std::string rev_string{"hello nice to meet you"};
  // actual reversed string + its size
  char* act_rev;
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

/**
 * Test fixture for parametrized testing of `pdcpl_tolower`.
 */
class ToLowerTest
  : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
public:
  /**
   * Return an input for `ToLowerTest` parametrized tests.
   *
   * @param s Input string
   * @returns Pair of `s` and `s` converted to all lowercase
   */
  static ParamType CreateInput(const std::string& s)
  {
    auto s_lower = s;
    // std::tolower
    std::for_each(
      s_lower.begin(),
      s_lower.end(),
      [](auto c) { return static_cast<decltype(c)>(std::tolower(c)); }
    );
    return {s, s_lower};
  }
};

/**
 * Test that `pdcpl_tolower` works as expected.
 */
TEST_P(ToLowerTest, Test)
{
  // original and lowercase string
  const auto& [input, expected_lower] = GetParam();
  // use pdcpl_tolower and compare
  auto actual_lower = input;
  std::for_each(
    actual_lower.begin(),
    actual_lower.end(),
    [](auto c) { return pdcpl_tolower(c); }
  );
  EXPECT_EQ(expected_lower, actual_lower);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  ToLowerTest,
  ::testing::Values(
    ToLowerTest::CreateInput("STOP the SHOUTING!"),
    ToLowerTest::CreateInput("Lots of MIxEd CASE"),
    ToLowerTest::CreateInput("1gn0R3 Str4ng3! charact3r$")
  )
);

/**
 * Test fixture for parametrized testing of `pdcpl_strexpand`.
 */
class StringExpandTest
  : public ::testing::TestWithParam<std::pair<std::string, std::string>> {};

/**
 * Test that `pdcpl_strexpand` works as expected.
 */
TEST_P(StringExpandTest, Test)
{
  // original and expanded strings
  const auto& [orig, expanded] = GetParam();
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

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  StringExpandTest,
  ::testing::Values(
    StringExpandTest::ParamType{
      "-hello a-z0-9A-Zb-d what's up-",
      "-hello "
      "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZbcd"
      " what's up-"
    },
    StringExpandTest::ParamType{
      "new A-Z string to 0-9a-z expand --some",
      "new ABCDEFGHIJKLMNOPQRSTUVWXYZ string to "
      "0123456789abcdefghijklmnopqrstuvwxyz expand --some"
    }
  )
);

}  // namespace
