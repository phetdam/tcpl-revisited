/**
 * @file memory_test.cc
 * @author Derek Huang
 * @brief memory.(c|h) tests
 * @copyright MIT License
 */

#include "pdcpl/memory.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <gtest/gtest.h>

namespace {

/**
 * Test fixture class for memory tests.
 */
class MemoryTest : public ::testing::Test {
protected:
  // default buffer size
  static inline constexpr std::size_t buf_size_ = 1024;
};

/**
 * Macro for asserting that the new `pdcpl_buffer` is ready for use.
 *
 * @param buf New `pdcpl_buffer` to check
 */
#define PDCPL_ASSERT_BUFFER_READY(buf) \
  ASSERT_TRUE(pdcpl_buffer_ready(&buf)) << "buffer is not ready for use"

/**
 * Macro for expecting that the `pdcpl_buffer` has non-`NULL` data.
 *
 * @param buf `pdcpl_buffer` to check
 */
#define PDCPL_EXPECT_BUFFER_DATA(buf) \
  EXPECT_TRUE(buf.data) << "buffer data pointer is NULL"

  /**
 * Macro for expecting that the `pdcpl_buffer` has `NULL` data.
 *
 * @param buf `pdcpl_buffer` to check
 */
#define PDCPL_EXPECT_NO_BUFFER_DATA(buf) \
  EXPECT_FALSE(buf.data) << "buffer data pointer is not NULL"

/**
 * Test that `pdcpl_buffer_new` works as expected.
 */
TEST_F(MemoryTest, BufferNewTest)
{
  // normal new
  auto buffer = pdcpl_buffer_new(buf_size_);
  PDCPL_EXPECT_BUFFER_DATA(buffer);
  EXPECT_EQ(buf_size_, buffer.size);
  std::free(buffer.data);
  // new with no allocation
  auto null_buffer = pdcpl_buffer_new(0);
  PDCPL_EXPECT_NO_BUFFER_DATA(null_buffer);
  EXPECT_EQ(0, null_buffer.size);
}

/**
 * Test that `pdcpl_buffer_realloc` works as expected.
 */
TEST_F(MemoryTest, BufferReallocTest)
{
  pdcpl_buffer buffer = pdcpl_buffer_new(buf_size_);
  PDCPL_ASSERT_BUFFER_READY(buffer);
  ASSERT_FALSE(pdcpl_buffer_realloc(&buffer, 2 * buf_size_));
  PDCPL_EXPECT_BUFFER_DATA(buffer);
  EXPECT_EQ(2 * buf_size_, buffer.size);
}

/**
 * Test that `pdcpl_buffer_clear` works as expected.
 */
TEST_F(MemoryTest, BufferClearTest)
{
  pdcpl_buffer buffer;
  buffer.data = std::malloc(buf_size_);
  ASSERT_TRUE(buffer.data) << "std::malloc(" << buf_size_ << ") failed";
  buffer.size = buf_size_;
  ASSERT_FALSE(pdcpl_buffer_clear(&buffer)) << "buffer clear failed";
  PDCPL_EXPECT_NO_BUFFER_DATA(buffer);
  EXPECT_EQ(0, buffer.size);
}

/**
 * Test that `pdcpl_buffer_expand_exact` works as expected.
 */
TEST_F(MemoryTest, BufferExpandExactTest)
{
  auto buffer = pdcpl_buffer_new(buf_size_);
  PDCPL_ASSERT_BUFFER_READY(buffer);
  ASSERT_FALSE(pdcpl_buffer_expand_exact(&buffer, buf_size_));
  PDCPL_EXPECT_BUFFER_DATA(buffer);
  EXPECT_EQ(2 * buf_size_, buffer.size);
}

/**
 * Test that `pdcpl_buffer_dynexpand_exact` works as expected.
 */
TEST_F(MemoryTest, BufferDynExpandExactTest)
{
  auto buffer = pdcpl_buffer_new(buf_size_);
  PDCPL_ASSERT_BUFFER_READY(buffer);
  // write position is at 1 past the halfway mark
  auto pos = static_cast<void*>(PDCPL_PTR_SHIFT(buffer.data, +, buf_size_ / 2));
  ASSERT_FALSE(pdcpl_buffer_dynexpand_exact(&buffer, pos, buf_size_));
  PDCPL_EXPECT_BUFFER_DATA(buffer);
  // buffer size was buf_size_ and should now be buf_size_ + buf_size_ / 2
  EXPECT_EQ(buf_size_ + buf_size_ / 2, buffer.size);
}

/**
 * Test that `pdcpl_buffer_dynexpand` works as expected.
 */
TEST_F(MemoryTest, BufferDynExpandTest)
{
  auto buffer = pdcpl_buffer_new(buf_size_);
  PDCPL_ASSERT_BUFFER_READY(buffer);
  // write position is at 1 past the halfway mark
  auto pos = static_cast<void*>(PDCPL_PTR_SHIFT(buffer.data, +, buf_size_ / 2));
  ASSERT_FALSE(pdcpl_buffer_dynexpand(&buffer, pos, buf_size_));
  PDCPL_EXPECT_BUFFER_DATA(buffer);
  // buffer size was buf_size_ and should now be buf_size_ + BUFSIZ
  EXPECT_EQ(buf_size_ + BUFSIZ, buffer.size);
}

}  // namespace
