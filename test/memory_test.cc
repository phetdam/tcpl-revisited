/**
 * @file memory_test.cc
 * @author Derek Huang
 * @brief memory.(c|h) tests
 * @copyright MIT License
 */

#include "pdcpl/memory.h"

#include <cstddef>
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
 * Test that `pdcpl_buffer_new` works as expected.
 */
TEST_F(MemoryTest, BufferNewTest)
{
  // normal new
  auto buffer = pdcpl_buffer_new(buf_size_);
  EXPECT_TRUE(buffer.data);
  EXPECT_EQ(buf_size_, buffer.size);
  std::free(buffer.data);
  // new with no allocation
  auto null_buffer = pdcpl_buffer_new(0);
  EXPECT_FALSE(null_buffer.data);
  EXPECT_EQ(0, null_buffer.size);
}

/**
 * Test that `pdcpl_buffer_clear` works as expected.
 */
TEST_F(MemoryTest, BufferClearTest)
{
  pdcpl_buffer buffer;
  buffer.data = std::malloc(buf_size_);
  ASSERT_TRUE(buffer.data) << "std::malloc(2014) failed";
  buffer.size = buf_size_;
  ASSERT_FALSE(pdcpl_buffer_clear(&buffer)) << "buffer clear failed";
  EXPECT_FALSE(buffer.data) << "buffer.data should be NULL";
  EXPECT_EQ(0, buffer.size);
}

/**
 * Test that `pdcpl_buffer_expand_exact` works as expected.
 */
TEST_F(MemoryTest, BufferExpandExactTest)
{
  auto buffer = pdcpl_buffer_new(buf_size_);
  ASSERT_TRUE(pdcpl_buffer_ready(&buffer)) << "buffer is not ready for use";
  ASSERT_FALSE(pdcpl_buffer_expand_exact(&buffer, buf_size_));
  EXPECT_TRUE(buffer.data) << "buffer.data is NULL";
  EXPECT_EQ(2 * buf_size_, buffer.size);
}

}  // namespace
