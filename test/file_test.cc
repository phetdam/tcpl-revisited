/**
 * @file file_test.cc
 * @author Derek Huang
 * @brief file.(c|h) unit tests
 * @copyright MIT License
 */

#include "pdcpl/file.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fcntl.h>
#endif  // _WIN32

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "pdcpl/features.h"

#ifdef PDCPL_POSIX_1_2008
#include <stdlib.h>  // mkstemp()
#endif  // PDCPL_POSIX1_2008

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
#if defined(_WIN32)
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
#endif  // !defined(_WIN32)
}

/**
 * Test that `pdcpl_win_gettempfile` works as expected.
 */
TEST_F(FileTest, WinGetTempFileName)
{
#if defined(_WIN32)
  char *path;
  ASSERT_EQ(S_OK, pdcpl_win_gettempfilename(&path));
  // basic checks. file name cannot be empty and there should not be any temp
  // file hanging around on the filesystem
  ASSERT_GT(std::strlen(path), 0);
  EXPECT_EQ(INVALID_FILE_ATTRIBUTES, GetFileAttributesA(path));
#else
  GTEST_SKIP();
#endif  // !defined(_WIN32)
}

/**
 * Test that `pdcpl_win_gettempfh` works as expected.
 */
TEST_F(FileTest, WinGetTempFileHandle)
{
#if defined(_WIN32)
  HANDLE handle = INVALID_HANDLE_VALUE;
  ASSERT_EQ(S_OK, pdcpl_win_gettempfh(&handle, GENERIC_READ | GENERIC_WRITE, 0U));
  EXPECT_NE(INVALID_HANDLE_VALUE, handle);
  EXPECT_TRUE(CloseHandle(handle)) << "error closing handle: HRESULT " <<
    std::hex << HRESULT_FROM_WIN32(GetLastError()) << std::dec;
#else
  GTEST_SKIP();
#endif  // !defined(_WIN32)
}

/**
 * Test that `pdcpl_win_gettempfd` works as expected.
 */
TEST_F(FileTest, WinGetTempFileDesc)
{
#if defined(_WIN32)
  int fd;
  ASSERT_EQ(S_OK, pdcpl_win_gettempfd(&fd, _O_RDWR | _O_TEXT));
  EXPECT_EQ(0, _close(fd)) << "error closing fd: " << std::strerror(errno);
#else
  GTEST_SKIP();
#endif  // _WIN32
}

/**
 * Test that `pdcpl_fdopen` works as expected.
 */
TEST_F(FileTest, FdOpenTest)
{
  // TODO: maybe i should add a Win32 test as well
#if defined(PDCPL_POSIX_1_2008)
  // use mkstemp to get a read/write file descriptor
  std::string temp_name{"pdcpl_fdopen_test_templateXXXXXX"};
  auto fd = mkstemp(temp_name.data());
  ASSERT_LT(0, fd) << "mkstemp error: " << std::strerror(errno);
  // get FILE * from descriptor and close. although b is ignored on POSIX
  // systems, it is for Windows and C89 compatibility
  auto f = pdcpl_fdopen(fd, "w+b");
  EXPECT_NE(EOF, std::fclose(f));
#else
  GTEST_SKIP();
#endif  // !defined(PDCPL_POSIX_1_2008)
}

}  // namespace
