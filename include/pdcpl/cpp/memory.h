/**
 * @file cpp/memory.h
 * @author Derek Huang
 * @brief C++ header for memory management helpers
 * @copyright MIT License
 */

#ifndef PDCPL_CPP_MEMORY_H_
#define PDCPL_CPP_MEMORY_H_

#include <cstdlib>
#include <cstdio>
#include <memory>

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

}  // namespace pdcpl

#endif  // PDCPL_CPP_MEMORY_H_
