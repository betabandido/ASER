#include <gtest/gtest.h>

#include <util/process.h>

using util::process;

namespace {

TEST(process, constructor) {
  process p("/bin/echo" "hello world");
  EXPECT_EQ(p.pid(), -1);
}

TEST(process, move) {
  process p("/bin/echo" "hello world");
  process q = std::move(p);
}

TEST(process, exec) {
  process p("/bin/echo", "hello world");
  EXPECT_EQ(p.pid(), -1);
  p.prepare();
  EXPECT_NE(p.pid(), -1);
  p.start();
  p.wait();
  EXPECT_EQ(p.pid(), -1);
}

TEST(process, kill_throw) {
  process p("/bin/echo", "hello world");
  EXPECT_THROW(p.kill(), std::runtime_error);
  p.prepare();
  p.start();
  p.wait();
  EXPECT_THROW(p.kill(), std::runtime_error);
}

} // namespace

