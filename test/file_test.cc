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
#include <fcntl.h>
#endif  // _WIN32

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "pdcpl/features.h"
#include "pdcpl/warnings.h"
#include "pdcpl/cpp/errno.h"
#include "pdcpl/cpp/memory.h"

#ifdef _WIN32
#include "pdcpl/cpp/hresult.h"
#endif  // _WIN32

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
  char* path;
  ASSERT_EQ(S_OK, pdcpl_win_gettempdir(&path));
  // take ownership so we don't leak if ASSERT_* fails
  pdcpl::unique_buffer<char> path_buf{path};
  // basic checks. documentation says the returned path will end with backslash
  // and we assert that the size is nonzero, otherwise indexing is dangerous
  const auto path_len = std::strlen(path_buf.get());
  ASSERT_GT(path_len, 0);
  EXPECT_EQ('\\', *(path_buf.get() + path_len - 1));
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
  // take ownership so we don't leak if ASSERT_* fails
  pdcpl::unique_buffer<char> path_buf{path};
  // basic checks. file name cannot be empty and there should not be any temp
  // file hanging around on the filesystem
  ASSERT_GT(std::strlen(path_buf.get()), 0);
  EXPECT_EQ(INVALID_FILE_ATTRIBUTES, GetFileAttributesA(path_buf.get()));
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
  EXPECT_TRUE(CloseHandle(handle)) << pdcpl::hresult_message();
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
  EXPECT_EQ(0, _close(fd)) << pdcpl::errno_message();
#else
  GTEST_SKIP();
#endif  // _WIN32
}

/**
 * Test that `pdcpl_fdopen` works as expected.
 */
TEST_F(FileTest, FdOpenTest)
{
#if defined(_WIN32)
  // create managed handle
  pdcpl::unique_handle handle = CreateFileA(
    "pdcpl_fdopen_test_template_win32",
    GENERIC_READ | GENERIC_WRITE,
    0U,
    NULL,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
    NULL
  );
  ASSERT_NE(INVALID_HANDLE_VALUE, handle.get()) <<
    pdcpl::hresult_message("CreateFileA error");
  // create managed file descriptor
  pdcpl::unique_fd fd = _open_osfhandle((intptr_t) handle.get(), _O_RDWR);
  ASSERT_NE(-1, fd.get()) << "_open_osfhandle error";
  // release handle as fd owns it now
  handle.release();
  // get FILE * from descriptor and close. binary mode for no translation
  auto f = pdcpl_fdopen(fd.get(), "w+b");
  ASSERT_TRUE(f) << pdcpl::errno_message();
  // release fd as f owns it now and then close the file
  fd.release();
  EXPECT_NE(EOF, std::fclose(f)) << pdcpl::errno_message();
#elif defined(PDCPL_POSIX_1_2008)
  // use mkstemp to get a managed read/write file descriptor
  std::string temp_name{"pdcpl_fdopen_test_templateXXXXXX"};
  pdcpl::unique_fd fd = mkstemp(temp_name.data());
  ASSERT_NE(-1, fd.get()) << pdcpl::errno_message("mkstemp error");
  // get FILE * from descriptor and close. although b is ignored on POSIX
  // systems, it is for Windows and C89 compatibility
  auto f = pdcpl_fdopen(fd.get(), "w+b");
  ASSERT_TRUE(f) << pdcpl::errno_message();
  // release fd as f owns it now and then close the file
  fd.release();
  EXPECT_NE(EOF, std::fclose(f)) << pdcpl::errno_message();
  // don't forget to unlink the file, otherwise it will hang around
  EXPECT_EQ(0, std::remove(temp_name.c_str())) << pdcpl::errno_message();
#else
  GTEST_SKIP();
#endif  // !defined(PDCPL_POSIX_1_2008)
}

/**
 * Test that `pdcpl_win_tempfile` works as expected.
 */
TEST_F(FileTest, WinTempFile)
{
#if defined(_WIN32)
  // usually we wouldn't explicitly set HRESULT to 0 in advance, but we do this
  // here just so we can check if it was set on error
  HRESULT status = 0;
  // HRESULT error message suffix formatter if getting FILE * fails
  auto err_suffix = [&status]
  {
// suppress C4165 warning that HRESULT is used as bool
PDCPL_MSVC_WARNING_DISABLE(4165)
    if (status) {
PDCPL_MSVC_WARNING_ENABLE()
      std::stringstream ss;
      ss << ", HRESULT: " << std::hex << status;
      return ss.str();
    }
    return std::string{};
  };
  // get FILE * and handle any errors
  pdcpl::unique_file f{pdcpl_win_tempfile(&status)};
  ASSERT_TRUE(f) << pdcpl::errno_message("error opening f") << err_suffix();
  // write string to file
  const std::string text{"hello here is some text"};
  ASSERT_NE(EOF, std::fputs(text.c_str(), f.get())) << pdcpl::errno_message();
  // get file size (seeking from the end), should equal string size
  auto file_size = std::ftell(f.get());
  ASSERT_NE(-1, file_size) << pdcpl::errno_message();
  ASSERT_EQ(text.size(), file_size);
  std::rewind(f.get());
  // read it all back + compare. no need to check std::fread return value
// suppress C4365 signed/unsigned mismatch warning
PDCPL_MSVC_WARNING_DISABLE(4365)
  auto text_buf = std::make_unique<char[]>(file_size + 1);
  std::fread(text_buf.get(), 1U, file_size, f.get());
PDCPL_MSVC_WARNING_ENABLE()
  EXPECT_EQ(text, std::string{text_buf.get()});
#else
  GTEST_SKIP();
#endif  // !defined(_WIN32)
}

}  // namespace
