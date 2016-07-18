#include <gtest/gtest.h>

#include <util/process.h>

using util::process;

namespace {

TEST(pipe, read_write) {
  util::pipe p;

  auto read = [&]() {
    char c;
    auto nbyte = p.read(&c, 1);
    EXPECT_EQ(nbyte, 1);
    return c;
  };

  auto write = [&](char c) {
    auto nbyte = p.write(&c, 1);
    EXPECT_EQ(nbyte, 1);
  };

  for (int i = 0; i < 10; i++) {
    write(i);
    EXPECT_EQ(read(), i);
  }
}

TEST(process, constructor) {
  process p("/usr/bin/false");
  EXPECT_EQ(p.pid(), -1);
}

TEST(process, move_constructor) {
  process p("/usr/bin/false");
  process q = std::move(p);
  EXPECT_EQ(q.pid(), -1);
}

TEST(process, file_not_found) {
  process p("/wrong_path");
  EXPECT_THROW(p.prepare(), std::runtime_error);
}

TEST(process, exec) {
  process p("/usr/bin/false");
  EXPECT_EQ(p.pid(), -1);
  p.prepare();
  EXPECT_NE(p.pid(), -1);
  p.start();
  p.wait();
  EXPECT_EQ(p.pid(), -1);
}

TEST(process, kill_throw) {
  process p("/usr/bin/false");
  EXPECT_THROW(p.kill(), std::runtime_error);
  p.prepare();
  p.start();
  p.wait();
  EXPECT_THROW(p.kill(), std::runtime_error);
}

TEST(process, kill) {
  process p("/bin/sleep", "30");
  p.set_kill_mode(process::kill_mode::PROC);
  p.prepare();
  p.start();
  EXPECT_NO_THROW(p.kill());
  p.wait();
  auto status = p.termination_status();
  EXPECT_TRUE(WIFSIGNALED(status));
}

} // namespace

