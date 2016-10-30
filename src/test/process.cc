#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <util/process.h>

namespace {

TEST(pipe_test, is_open) {
  using aser::util::pipe;

  pipe p;
  EXPECT_TRUE(p.is_open(pipe::end_point::READ_END)
      && p.is_open(pipe::end_point::WRITE_END));

  p.close(pipe::end_point::READ_END);
  EXPECT_TRUE(!p.is_open(pipe::end_point::READ_END));

  p.close(pipe::end_point::WRITE_END);
  EXPECT_TRUE(!p.is_open(pipe::end_point::WRITE_END));
}

TEST(pipe_test, cannot_read_from_closed_pipe) {
  using aser::util::pipe;

  pipe p;
  p.close(pipe::end_point::READ_END);
  char buf;
  EXPECT_THROW(p.read(&buf, 1), std::runtime_error);
}

TEST(pipe_test, cannot_write_to_closed_pipe) {
  using aser::util::pipe;

  pipe p;
  p.close(pipe::end_point::WRITE_END);
  char buf;
  EXPECT_THROW(p.write(&buf, 1), std::runtime_error);
}

TEST(pipe_test, move_semantics) {
  using aser::util::pipe;

  pipe p;
  EXPECT_TRUE(p.is_open(pipe::end_point::READ_END)
      && p.is_open(pipe::end_point::WRITE_END));

  pipe q{std::move(p)};
  EXPECT_TRUE(q.is_open(pipe::end_point::READ_END)
      && q.is_open(pipe::end_point::WRITE_END));
  EXPECT_TRUE(!p.is_open(pipe::end_point::READ_END)
      && !p.is_open(pipe::end_point::WRITE_END));
}

TEST(pipe_test, read_write) {
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

class process_config_callback_mock {
public:
  MOCK_METHOD0(fork_setup, void());
  MOCK_METHOD0(exec_setup, void());
  MOCK_METHOD0(fork_success, void());
  MOCK_METHOD1(fork_error, void(int));
  MOCK_METHOD1(exec_error, void(int));
};

TEST(process_config_test, callbacks_test) {
  process_config_callback_mock mock;
  EXPECT_CALL(mock, fork_setup());
  EXPECT_CALL(mock, exec_setup());
  EXPECT_CALL(mock, fork_success());
  EXPECT_CALL(mock, fork_error(1000));
  EXPECT_CALL(mock, exec_error(1000));

  using namespace std::placeholders;

  auto config = aser::util::process_config{}
    .fork_setup(std::bind(&process_config_callback_mock::fork_setup, &mock))
    .exec_setup(std::bind(&process_config_callback_mock::exec_setup, &mock))
    .fork_success(std::bind(&process_config_callback_mock::fork_success, &mock))
    .fork_error(std::bind(&process_config_callback_mock::fork_error, &mock, _1))
    .exec_error(std::bind(&process_config_callback_mock::exec_error, &mock, _1));

  config.fork_setup();
  config.exec_setup();
  config.fork_success();
  config.fork_error(1000);
  config.exec_error(1000);
}

TEST(process_test, not_started_after_construction) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_FALSE(p.started());
}

TEST(process_test, not_terminated_after_construction) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_FALSE(p.terminated());
}

TEST(process_test, started_after_calling_start) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  EXPECT_TRUE(p.started());
  p.wait();
}

TEST(process_test, not_terminated_before_calling_wait) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  EXPECT_FALSE(p.terminated());
  p.wait();
}

TEST(process_test, terminated_after_calling_wait) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  p.wait();
  EXPECT_TRUE(p.terminated());
}

TEST(process_death_test, pid_not_valid_before_start) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_DEATH(p.pid(), "");
}

TEST(process_death_test, termination_status_not_valid_before_start) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_DEATH(p.termination_status(), "");
}

TEST(process_test, args_are_not_empty) {
  using process = aser::util::process<aser::util::process_config>;
  EXPECT_THROW(process{std::vector<std::string>{}}, std::invalid_argument);
}

TEST(process_test, path_exists) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_THROW(p.start(), std::invalid_argument);
}

TEST(process_death_test, cannot_call_wait_before_start) {
  using process = aser::util::process<aser::util::process_config>;
  process p("foo");
  EXPECT_DEATH(p.wait(), "");
}

TEST(process_death_test, cannot_wait_twice) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  p.wait();
  EXPECT_DEATH(p.wait(), "");
}

TEST(process_test, simple) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  EXPECT_GT(p.pid(), 0);
  p.wait();
  EXPECT_EQ(p.termination_status(), 0);
}

TEST(process_test, check_killed_process_termination_status) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  p.start();
  aser::util::kill_process(p);
  p.wait();
  auto status = p.termination_status();
  EXPECT_TRUE(WIFSIGNALED(status));
}

TEST(kill_process_death_test, process_must_have_started) {
  using process = aser::util::process<aser::util::process_config>;
  process p("/usr/bin/env", "true");
  EXPECT_DEATH(aser::util::kill_process(p), "");
}

TEST(process, kill_tree) {
  using namespace std::chrono_literals;
  namespace util = aser::util;
  using process = util::process<aser::util::process_config>;
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
  p.config()
    .exec_setup([]() { setpgid(getpid(), 0); });
  p.start();
  std::this_thread::sleep_for(1s);
  EXPECT_NO_THROW(util::kill_process(p, util::kill_mode::TREE));
  p.wait();
  auto status = p.termination_status();
  EXPECT_TRUE(WIFSIGNALED(status));
  std::this_thread::sleep_for(2s);
  EXPECT_FALSE(boost::filesystem::exists(tmp_path));
}

TEST(sync_process, basic) {
  using namespace std::chrono_literals;
  namespace fs = boost::filesystem;
  using aser::util::sync_process;

  auto tmp_path = fs::temp_directory_path();
  tmp_path /= fs::unique_path();
  auto cmd = boost::str(
      boost::format("echo process_started > %1%") % tmp_path);
  sync_process p("/bin/bash", "-c", cmd);
  p.prepare();
  std::this_thread::sleep_for(1s);
  EXPECT_FALSE(fs::exists(tmp_path));

  p.start();
  p.wait();
  EXPECT_TRUE(fs::exists(tmp_path));

  boost::system::error_code ec;
  fs::remove(tmp_path);
}

} // namespace

