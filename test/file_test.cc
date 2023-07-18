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
#include <ios>
#include <iostream>

#include <gtest/gtest.h>

namespace {

/**
 * Main file test fixture.
 */
class FileTest : public ::testing::Test {};

/**
 * Test that `pdcpl_win_gettempdir` works as expected.
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

/**
 * Test that `pdcpl_win_gettempfile` works as expected.
 */
TEST_F(FileTest, WinGetTempFileName)
{
#ifdef _WIN32
  char *path;
  ASSERT_EQ(S_OK, pdcpl_win_gettempfilename(&path));
  // basic checks. file name cannot be empty and there should not be any temp
  // file hanging around on the filesystem
  ASSERT_GT(std::strlen(path), 0);
  EXPECT_EQ(INVALID_FILE_ATTRIBUTES, GetFileAttributesA(path));
#else
  GTEST_SKIP();
#endif  // !_WIN32
}

/**
 * Test that `pdcpl_win_gettempfh` works as expected.
 */
TEST_F(FileTest, WinGetTempFileHandle)
{
#ifdef _WIN32
  HANDLE handle = INVALID_HANDLE_VALUE;
  ASSERT_EQ(S_OK, pdcpl_win_gettempfh(&handle, GENERIC_READ | GENERIC_WRITE, 0U));
  EXPECT_NE(INVALID_HANDLE_VALUE, handle);
  EXPECT_TRUE(CloseHandle(handle)) << "error closing handle: HRESULT " <<
    std::hex << HRESULT_FROM_WIN32(GetLastError()) << std::dec;
#else
  GTEST_SKIP();
#endif  // !_WIN32
}

}  // namespace
