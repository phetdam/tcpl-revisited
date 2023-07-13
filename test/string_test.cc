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

class ConcatTest : public StringTest, public ::testing::WithParamInterface<
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
TEST_P(ConcatTest, ParamTest)
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
