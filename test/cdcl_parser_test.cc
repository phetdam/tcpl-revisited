/**
 * @file cdcl_parser_test.cc
 * @author Derek Huang
 * @brief cdcl_parser.hh unit tests
 * @copyright MIT License
 */

#include "pdcpl/cdcl_parser.hh"

#include <cstring>
#include <filesystem>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

// default location for the test data root, determined at compile time. this is
// used if PDCPL_BCDP_TEST_DIR is not set in the environment
#ifndef PDCPL_BCDP_TEST_DIR
#define PDCPL_BCDP_TEST_DIR ""
#endif  // PDCPL_BCDP_TEST_DIR

namespace {

/**
 * Base test fixture class for all C declaration parser tests.
 */
class DclParserTest : public ::testing::Test {
protected:
  /**
   * Test setup function.
   *
   * If there is a skip reason, the test is skipped and the reason printed.
   */
  void SetUp() override
  {
    if (skip_tests_reason_.size())
      GTEST_SKIP() << skip_tests_reason_;
  }

  /**
   * Test suite setup function.
   *
   * Sets the test skipping reason if `PDCPL_BCDP_TEST_DIR` is not defined,
   * is defined but does not exist, or exists but is not a directory.
   */
  static void SetUpTestSuite()
  {
    if (test_data_dir_.empty())
      skip_tests_reason_ = "PDCPL_BCDP_TEST_DIR not defined";
    else if (!std::filesystem::exists(test_data_dir_))
      skip_tests_reason_ = "PDCPL_BCDP_TEST_DIR " + test_data_dir_.string() +
        " does not exist";
    else if (!std::filesystem::is_directory(test_data_dir_))
      skip_tests_reason_ = "PDCPL_BCDP_TEST_DIR " + test_data_dir_.string() +
        " is not a directory";
  }

  /**
   * Return the test data directory path given by `PDCPL_BCDP_TEST_DIR`.
   *
   * This is provided as a convenience to test subclasses.
   */
  const auto& test_data_dir() const noexcept { return test_data_dir_; }

  // test data directory path
  static inline const auto test_data_dir_{
    []
    {
      // first try to get from environment
      auto dir_path = std::getenv("PDCPL_BCDP_TEST_DIR");
      if (dir_path)
        return std::filesystem::path{dir_path};
      // otherwise, try the macro, else return empty path
      if (std::strlen(PDCPL_BCDP_TEST_DIR))
        return std::filesystem::path{PDCPL_BCDP_TEST_DIR};
      return std::filesystem::path{};
    }()
  };

private:
  static inline std::string skip_tests_reason_;
};

/**
 * Parameterized testing fixture for C declaration parsing tests.
 */
class DclParserParamTest
  : public DclParserTest,
    public ::testing::WithParamInterface<std::string_view> {};

/**
 * Test that the sample inputs are understood by the parser.
 *
 * No semantic action results are evaluated.
 */
TEST_P(DclParserParamTest, PureParseTest)
{
  pdcpl::cdcl_parser parser;
  ASSERT_TRUE(parser(test_data_dir() / GetParam())) << parser.last_error();
}

INSTANTIATE_TEST_SUITE_P(
  ParseTest,
  DclParserParamTest,
  ::testing::Values("bdcl.in.1", "bdcl.in.2", "bdcl.in.3")
);

}  // namespace
