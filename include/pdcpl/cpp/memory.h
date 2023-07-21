/**
 * @file cpp/memory.h
 * @author Derek Huang
 * @brief C++ header for memory management helpers
 * @copyright MIT License
 */

#ifndef PDCPL_CPP_MEMORY_H_
#define PDCPL_CPP_MEMORY_H_

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // _WIN32

#include <cstdlib>
#include <cstdio>
#include <memory>

#include "pdcpl/features.h"

namespace pdcpl {

/**
 * Deleter that uses `std::free` to deallocate requested memory.
 *
 * This is useful for managing `malloc`-ed buffers when working with C code.
 *
 * @tparam T type
 */
template <typename T>
struct free_deleter {
  /**
   * Free the specified buffer with `std::free`.
   *
   * @param ptr Address of buffer to free
   */
  void operator()(T* ptr) const
  {
    std::free(ptr);
  }
};

/**
 * Deleter for `FILE*` that uses `fclose` to close the stream.
 *
 * This is useful when working with `FILE*` returned from C functions.
 */
struct file_deleter {
  /**
   * Close the specified file pointer.
   *
   * @param stream File pointer to close
   */
  void operator()(std::FILE* stream) const
  {
    std::fclose(stream);
  }
};

/**
 * Unique pointer for `malloc`-ed buffers of type `T`.
 *
 * @tparam T type
 */
template <typename T>
using unique_buffer = std::unique_ptr<T, free_deleter<T>>;

/**
 * Unique pointer for `FILE*`.
 */
using unique_file = std::unique_ptr<std::FILE, file_deleter>;

#ifdef _WIN32
/**
 * Unique pointer analogue for a `HANDLE`.
 */
class unique_handle {
public:
  /**
   * Ctor.
   *
   * @param handle `HANDLE` to take ownership of
   */
  unique_handle(HANDLE handle) : handle_{handle} {}

  /**
   * Dtor.
   *
   * If the `HANDLE` is released, i.e. is `INVALID_HANDLE_VALUE`, no-op.
   */
  ~unique_handle()
  {
    if (handle_ != INVALID_HANDLE_VALUE)
      CloseHandle(handle_);
  }

  /**
   * Return the `HANDLE` by value.
   */
  auto get() const { return handle_; }

  /**
   * Release ownership of the `HANDLE`.
   *
   * The `HANDLE` is returned and subsequent calls to `get()` will result in
   * `INVALID_HANDLE_VALUE` being returned instead.
   */
  auto release()
  {
    auto handle = handle_;
    handle_ = INVALID_HANDLE_VALUE;
    return handle;
  }

private:
  HANDLE handle_;
};
#endif  // _WIN32

#if defined(_WIN32) || defined(PDCPL_HAS_UNISTD)
/**
 * Unique pointer analogue for a file descriptor.
 */
class unique_fd {
public:
  /**
   * Ctor.
   *
   * @param fd File descriptor to take ownership of
   */
  unique_fd(int fd) : fd_{fd} {}

  /**
   * Dtor.
   *
   * If the file descriptor has been released, i.e. is -1, no-op.
   */
  ~unique_fd()
  {
    if (fd_ != -1)
#if defined(_WIN32)
      _close(fd_);
#else
      close(fd_);
#endif  // !defined(_WIN32)
  }

  /**
   * Return the file descriptor by value.
   */
  auto get() const { return fd_; }

  /**
   * Release ownership of the file descriptor.
   *
   * The file descriptor is returned and subsequent calls to `get() return -1.
   */
  auto release()
  {
    auto fd = fd_;
    fd_ = -1;
    return fd;
  }

private:
  int fd_;
};
#endif // !defined(_WIN32) && !defined(PDCPL_HAS_UNISTD)

}  // namespace pdcpl

// don't pollute translation units
#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN
#endif  // _WIN32

#endif  // PDCPL_CPP_MEMORY_H_
