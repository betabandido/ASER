#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <util/process.h>

using aser::util::process;

namespace {

TEST(pipe, read_write) {
  aser::util::pipe p;

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
  process p("/usr/bin/env", "false");
  EXPECT_EQ(p.pid(), -1);
}

TEST(process, move_constructor) {
  process p("/usr/bin/env", "false");
  process q = std::move(p);
  EXPECT_EQ(q.pid(), -1);
}

TEST(process, file_not_found) {
  process p("/wrong_path");
  EXPECT_THROW(p.prepare(), std::runtime_error);
}

TEST(process, exec) {
  process p("/usr/bin/env", "false");
  EXPECT_EQ(p.pid(), -1);
  p.prepare();
  EXPECT_NE(p.pid(), -1);
  p.start();
  p.wait();
  //EXPECT_EQ(p.pid(), -1);
}

TEST(process, kill_throw) {
  process p("/usr/bin/env", "false");
  EXPECT_THROW(p.kill(), std::runtime_error);
  p.prepare();
  p.start();
  p.wait();
// XXX See note on process::kill().
//  EXPECT_THROW(p.kill(), std::runtime_error);
}

TEST(process, kill) {
  process p("/usr/bin/env", "sleep", "30");
  p.set_kill_mode(process::kill_mode::PROC);
  p.prepare();
  p.start();
  EXPECT_NO_THROW(p.kill());
  p.wait();
  auto status = p.termination_status();
  EXPECT_TRUE(WIFSIGNALED(status));
}

TEST(process, kill_tree) {
  using namespace std::chrono_literals;
  // The command creates a hierarchy of processes so that calling p.kill()
  // only kills the top process, not its children. When kill_mode::TREE is
  // used, however, all the children should die too.
  // The grandchild will write to a temporary file unless it is killed first.
  // We check for the presence of that file in order to test the kill tree
  // approach. If the file does not exist, p.kill() worked as expected.
  auto tmp_path = boost::filesystem::unique_path();
  auto cmd = boost::str(boost::format(
        "/bin/bash -c '(sleep 2; echo kill_proc_failed > %1%)'") % tmp_path);
  process p("/bin/bash", "-c", cmd);
  p.set_kill_mode(process::kill_mode::TREE);
  p.prepare();
  p.start();
  std::this_thread::sleep_for(1s);
  EXPECT_NO_THROW(p.kill());
  p.wait();
  auto status = p.termination_status();
  EXPECT_TRUE(WIFSIGNALED(status));
  std::this_thread::sleep_for(2s);
  EXPECT_FALSE(boost::filesystem::exists(tmp_path));
}

} // namespace

