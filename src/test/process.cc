#include <gtest/gtest.h>

#include <util/process.h>

using util::process;

namespace {

TEST(process, build) {
  process p("/bin/ls");
  EXPECT_EQ(p.pid(), -1);
}


TEST(process, move) {
  process p("/bin/echo" "hello world");
  process q = std::move(p);
}

} // namespace

