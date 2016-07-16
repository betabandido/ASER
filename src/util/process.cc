#include "process.h"

#include <cassert>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "kill.h"
#include "libc_wrapper.h"

namespace fs = boost::filesystem;

namespace util {

static void pipe2(int fd[2], int flags) {
#ifdef __linux__
  error_if_not_equal(::pipe2(fd, flags), 0, "Error at pipe2");
#elif defined __APPLE__
  error_if_not_equal(::pipe(fd), 0, "Error at pipe");
  for (int i = 0; i < 2; ++i) {
    auto curr_flags = error_if_equal(
        fcntl(fd[i], F_GETFD),
        -1,
        "Error getting flags");
    error_if_equal(
        fcntl(fd[i], F_SETFD, curr_flags | flags),
        -1,
        "Error settings flags");
  }
#else
  #error pipe2 is not supported
#endif
}

pipe::pipe() {
  error_if_not_equal(::pipe(&fd_[0]), 0, "Error creating pipe");
}

pipe::pipe(int flags) {
  pipe2(&fd_[0], flags);
}

pipe::~pipe() {
  close(end_point::READ_END);
  close(end_point::WRITE_END);
}

pipe::pipe(pipe&& other)
  : fd_{std::move(other.fd_)}
{
  other.fd_ = {{ -1, -1 }};
}

pipe& pipe::operator=(pipe&& other) {
  fd_ = std::move(other.fd_);
  other.fd_ = {{ -1, -1 }};
  return *this;
}
      
void pipe::close(end_point ep) {
  auto idx = static_cast<unsigned>(ep);
  if (fd_[idx] == -1)
    return;

  error_if_not_equal(::close(fd_[idx]), 0, "Error closing pipe");
  fd_[idx] = -1;
}

process::~process() {
  try { kill(); }
  catch (...) {}
}

void process::prepare() {
  assert(state_ == exec_state::NON_INITIALIZED);

  fs::path path(args_[0]);
  if (!fs::exists(path))
    throw std::runtime_error(boost::str(boost::format("File %1% does not exist")
        % path
        ));

  pipe child_ready_pipe;

  auto pid = fork();
  switch (pid) {
  case -1:
    libc_error("Error at fork");
    break;
  case 0:
    throw std::logic_error("Child unexpectedly returned");
  default:
    break;
  }
}

void process::kill() const {
  if (pid_ == -1)
    throw std::runtime_error("Process is not running");

  switch (kill_mode_) {
    case kill_mode::PROC:
    ::util::kill(pid_);
    break;
    case kill_mode::TREE:
    kill_group(pid_);
    break;
  default:
    assert(false);
  }
}

} // namespace util

