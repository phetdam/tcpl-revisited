/**
 * @file file_test.cc
 * @author Derek Huang
 * @brief file.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/file.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

namespace {

/**
 * Main file test fixture.
 */
class FileTest : public ::testing::Test {};

/**
 * Test that `pdcpl_win_gettempdir` works as expecte.
 */
TEST_F(FileTest, WinGetTempDir)
{
#ifdef _WIN32
  char *path;
  ASSERT_EQ(S_OK, pdcpl_win_gettempdir(&path));
  // basic checks. documentation says the returned path will end with backslash
  // and we assert that the size is nonzero, otherwise indexing is dangerous
  const auto path_len = std::strlen(path);
  ASSERT_GT(path_len, 0);
  EXPECT_EQ('\\', path[path_len - 1]);
  // clean up
  std::free(path);
#else
  GTEST_SKIP();
#endif  // !_WIN32
}

}  // namespace
