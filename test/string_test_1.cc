/**
 * @file string_test_1.cc
 * @author Derek Huang
 * @brief string.(c|h) unit tests 1
 * @copyright MIT License
 */

#include "pdcpl/string.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

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
#include "pdcpl/file.h"
#include "pdcpl/cpp/errno.h"
#include "pdcpl/cpp/common.h"
#include "pdcpl/cpp/memory.h"

// tests reading from an in-memory buffer as a FILE* need fmemopen
#ifdef PDCPL_POSIX_1_2008
#include <stdio.h>  // fmemopen

#include <cstring>  // std::strtok
#include <memory>
#endif  // PDCPL_POSIX_1_2008

#ifdef _WIN32
#include <cstdio>  // std::fputs

#include "pdcpl/cpp/hresult.h"
#endif  // _WIN32

namespace {

/**
 * Test fixture for parametrized testing of `pdcpl_isesc`.
 */
class IsEscTest : public ::testing::TestWithParam<std::pair<int, bool>> {};

/**
 * Test that `pdcpl_isesc` works as expected.
 */
TEST_P(IsEscTest, Test)
{
  // character, expected truth
  const auto [input, truth] = GetParam();
  EXPECT_EQ(truth, pdcpl_isesc(input));
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  IsEscTest,
  ::testing::Values(
    IsEscTest::ParamType{'a', false},
    IsEscTest::ParamType{'b', false},
    IsEscTest::ParamType{'\\', true},
    IsEscTest::ParamType{'\t', true},
    IsEscTest::ParamType{'1', false},
    IsEscTest::ParamType{'?', false},
    IsEscTest::ParamType{'\a', true},
    IsEscTest::ParamType{'\n', true}
  )
);

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
 * Test fixture for parametrized testing of word getting or counting.
 *
 * Currently has tests for `pdcpl_strwc`, `pdcpl_fwc`, `pdcpl_getword`.
 */
class WordTest : public ::testing::TestWithParam<
  std::tuple<std::string, std::vector<std::string>, std::size_t>> {
public:
  /**
   * Create an input for `WordTest` parametrized tests.
   *
   * Number of words is determined by counting number of elements retrieved by
   * using `string_split`, tokenizing by spaces or newlines.
   *
   * @param s Input string
   * @returns Tuple of `s`, vector of words in `s`, number of lines in `s`
   */
  static ParamType CreateInput(const std::string& s)
  {
    return {
      s,
      string_split(s, " \n"),
      // need cast since std::count returns [signed] iterator difference_type
      static_cast<decltype(s.size())>(1 + std::count(s.cbegin(), s.end(), '\n'))
    };
  }
};

/**
 * Test that `pdcpl_strwc` works as expected.
 */
TEST_P(WordTest, StringWordCountTest)
{
  // input string, word vector, line count
  const auto& [input, words, n_lines] = GetParam();
  // if something goes wrong, the test will abort
  pdcpl_wcresults res;
  ASSERT_FALSE(pdcpl_strwc(input.c_str(), &res));
  EXPECT_EQ(res.nc, input.size());
  EXPECT_EQ(res.nl, n_lines);
  EXPECT_EQ(res.nw, words.size());
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  WordTest,
  ::testing::Values(
    WordTest::CreateInput(
      "A really long\n"
      "sequence of words over many\n"
      "long lines\n"
      "\n"
      "Also a bad\n"
      "haiku"
    ),
    WordTest::CreateInput(
      "Many days and many nights\n"
      "We journeyed past the deserts of the wasteland\n"
      "\n"
      "\n"
      "Dealing with     really    bad      spacing          choices\n"
      "\n"
      "\n"
      "\n"
      "But somehow\n"
      "managing to finish this poem"
    )
  )
);

/**
 * Test that `pdcpl_fwc` works as expected.
 *
 * @note Requires POSIX.1-2008 `fmemopen` to be available.
 */
TEST_P(WordTest, FileWordCountTest)
{
#if defined(_WIN32) || defined(PDCPL_POSIX_1_2008)
  // input string, word vector, line count
  const auto& [input, words, n_lines] = GetParam();
  pdcpl_wcresults res;
  // get managed FILE* from the string's char buffer (no null terminator)
#if defined(_WIN32)
  pdcpl::unique_file file{pdcpl_win_tempfile(NULL)};
  ASSERT_TRUE(file) << pdcpl::errno_message() << ", " << pdcpl::hresult_message();
  // since Win32 has no fmemopen equivalent, we manually write to this file.
  // note that the Win32 version of fputs is not documented to set errno
  ASSERT_NE(EOF, std::fputs(input.c_str(), file.get()));
  // rewind so we can read from the beginning of the file
  std::rewind(file.get());
#else
  pdcpl::unique_file file{fmemopen((void*) input.c_str(), input.size(), "r")};
  ASSERT_TRUE(file) << pdcpl::errno_message();
#endif  // !defined(_WIN32)
  // check that results are as expected
  ASSERT_FALSE(pdcpl_fwc(file.get(), &res));
  EXPECT_EQ(res.nc, input.size());
  EXPECT_EQ(res.nl, n_lines);
  EXPECT_EQ(res.nw, words.size());
#else
  GTEST_SKIP();
#endif  // !defined(_WIN32) && !defined(PDCPL_POSIX_1_2008)
}

/**
 * Test that `pdcpl_getword` works as expected.
 *
 * @note Requires POSIX.1-2008 `fmemopen` to be available.
 */
TEST_P(WordTest, FileGetWordTest)
{
#if defined(_WIN32) || defined(PDCPL_POSIX_1_2008)
  // input string, word vector
  // const auto& [input, n_words, n_chars, n_lines] = GetParam();
  const auto& input = std::get<0>(GetParam());
  const auto& words = std::get<1>(GetParam());
  // get managed FILE* from the string's char buffer (no null terminator)
#if defined(_WIN32)
  pdcpl::unique_file file{pdcpl_win_tempfile(NULL)};
  ASSERT_TRUE(file) << pdcpl::errno_message() << ", " << pdcpl::hresult_message();
  // since Win32 has no fmemopen equivalent, we manually write to this file.
  // note that the Win32 version of fputs is not documented to set errno
  ASSERT_NE(EOF, std::fputs(input.c_str(), file.get()));
  // rewind so we can read from the beginning of the file
  std::rewind(file.get());
#else
  pdcpl::unique_file file{fmemopen((void*) input.c_str(), input.size(), "r")};
  ASSERT_TRUE(file) << pdcpl::errno_message();
#endif  // !defined(_WIN32)
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
  EXPECT_EQ(words, act_words);
#else
  GTEST_SKIP();
#endif  // !defined(_WIN32) && !defined(PDCPL_POSIX_1_2008)
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
TEST_P(PrintWidthTest, Test)
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
 * Test fixture for parametrized testing of `pdcpl_strrev`.
 */
class StringReverseTest : public ::testing::TestWithParam<
  std::tuple<std::string, std::string, std::size_t>> {
public:
  /**
   * Create an input for `StringReverseTest` parametrized tests.
   *
   * @param s String to reverse
   * @returns Tuple of `s`, `s` reversed, and the size of `s`
   */
  static ParamType CreateInput(const std::string& s)
  {
    return {s, std::string{s.rbegin(), s.rend()}, s.size()};
  }
};

/**
 * Test that `pdcpl_strrev` works as expected.
 */
TEST_P(StringReverseTest, Test)
{
  // string to reverse, expected reversed string, expected reverse size
  const auto& [input, exp_rev, exp_size] = GetParam();
  // actual reversed string + its size
  char* act_rev;
  std::size_t act_size;
  ASSERT_FALSE(pdcpl_strrev(input.c_str(), &act_rev, &act_size));
  // contents/size should match
  EXPECT_EQ(exp_rev, std::string{act_rev});
  EXPECT_EQ(exp_size, act_size);
  // string is on heap, clean up
  std::free(act_rev);
}

INSTANTIATE_TEST_SUITE_P(
  StringTest,
  StringReverseTest,
  ::testing::Values(
    StringReverseTest::CreateInput("hello nice to meet you"),
    StringReverseTest::CreateInput("another string to reverse"),
    StringReverseTest::CreateInput("")
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
TEST_P(TabStopTest, Test)
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
TEST_P(HexConvertTest, Test)
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
TEST_P(SqueezeTest, Test)
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
