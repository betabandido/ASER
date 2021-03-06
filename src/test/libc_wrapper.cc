#include <gtest/gtest.h>

#include <util/libc_wrapper.h>

namespace {

TEST(libc_wrapper, error_if_equal) {
  using aser::util::error_if_equal;
  EXPECT_EQ(error_if_equal(0, -1, ""), 0);
  EXPECT_THROW(error_if_equal(-1, -1, ""), std::runtime_error);
}

TEST(libc_wrapper, error_if_not_equal) {
  using aser::util::error_if_not_equal;
  EXPECT_EQ(error_if_not_equal(0, 0, ""), 0);
  EXPECT_THROW(error_if_not_equal(-1, 0, ""), std::runtime_error);
}

} // namespace

