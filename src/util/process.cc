#include "process.h"

#include <cassert>
#include <sys/wait.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "kill.h"
#include "libc_wrapper.h"
#include "log.h"
#include "os.h"

namespace fs = boost::filesystem;

namespace aser {
namespace util {

//////////
// pipe //
//////////

pipe::pipe() {
  error_if_not_equal(
      ::pipe(&fd_[0]),
      0,
      "Error creating pipe");
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

bool pipe::is_open(end_point ep) const {
  auto idx = static_cast<unsigned>(ep);
  return fd_[idx] != -1;
}
      
void pipe::close(end_point ep) {
  if (!is_open(ep))
    return;

  auto idx = static_cast<unsigned>(ep);
  error_if_not_equal(
      ::close(fd_[idx]),
      0,
      "Error closing pipe");
  fd_[idx] = -1;
}

size_t pipe::read(char* buffer, size_t nbyte) {
  if (!is_open(end_point::READ_END))
    throw std::runtime_error("Cannot read from closed pipe");

  return static_cast<size_t>(error_if_equal(
      ::read(fd_[0], buffer, nbyte),
      static_cast<ssize_t>(-1),
      "Error reading from pipe"));
}

size_t pipe::write(const char* buffer, size_t nbyte) {
  if (!is_open(end_point::WRITE_END))
    throw std::runtime_error("Cannot write to closed pipe");

  return static_cast<size_t>(error_if_equal(
      ::write(fd_[1], buffer, nbyte),
      static_cast<ssize_t>(-1),
      "Error writing to pipe"));
}

/////////////
// process //
/////////////

process::~process() {
  try {
    LOG("process::~process()");

    if (state_ == exec_state::READY
        || state_ == exec_state::RUNNING)
      kill();

    if (state_ == exec_state::KILLED)
      wait();
  }
  catch (...) {}
}

void process::prepare() {
  assert(state_ == exec_state::NON_INITIALIZED);

  LOG(boost::format("Executing %1%") % args_[0]);

  fs::path path(args_[0]);
  if (!fs::exists(path))
    throw std::runtime_error(boost::str(boost::format("File %1% does not exist")
        % path
        ));

  std::vector<char*> args(args_.size() + 1, nullptr);
  std::transform(begin(args_), end(args_), begin(args),
      [](const std::string& s) { return const_cast<char*>(s.c_str()); });
  assert(args.back() == nullptr);

  pipe child_ready_pipe;

  auto pid = fork();
  char buf;

  switch (pid) {
  case -1:
    libc_error("Error at fork");
    break;
  case 0:
    // XXX creating a new process group allows kill_group to work safely.
    // We might want to explore other alternatives, though.
    setpgid(getpid(), 0);
    child_ready_pipe.close(pipe::end_point::READ_END);
    go_pipe_.close(pipe::end_point::WRITE_END);
    child_ready_pipe.close(pipe::end_point::WRITE_END);
    go_pipe_.read(&buf, 1);

    LOG("About to call execvp");
    execvp(args[0], &args[0]);

    // We are not supposed to reach here. Doing so, means execvp() failed.
    libc_error("execvp failed");
  default:
    child_ready_pipe.close(pipe::end_point::WRITE_END);
    go_pipe_.close(pipe::end_point::READ_END);
    child_ready_pipe.read(&buf, 1);
    child_ready_pipe.close(pipe::end_point::READ_END);
    pid_ = pid;
    state_ = exec_state::READY;
    break;
  }
}

void process::start() {
  assert(state_ == exec_state::READY);

  LOG(boost::format("Starting process %1%") % pid_);

  char buf = 0;
  go_pipe_.write(&buf, 1);
  go_pipe_.close(pipe::end_point::WRITE_END);
  state_ = exec_state::RUNNING;
}

void process::wait() {
  assert(state_ == exec_state::READY
      || state_ == exec_state::RUNNING
      || state_ == exec_state::KILLED);

  LOG(boost::format("Waiting for process %1%") % pid_);

  error_if_equal(
      waitpid(pid_, &termination_status_, 0),
      -1,
      "Error waiting for process");

  LOG(boost::format("Process %1% has finished") % pid_);

  if (WIFEXITED(termination_status_)
      || WIFSIGNALED(termination_status_)) {
    state_ = exec_state::JOINED;
    //pid_ = -1;
  }
}

int process::termination_status() const {
  if (state_ != exec_state::JOINED)
    throw std::runtime_error("Process not terminated");

  return termination_status_;
}

void process::kill() {
  // XXX There are situations where it is handy not to throw an exception if
  // a process has already been killed/joined -- for instance when a processes
  // completes its execution, and then we proceed to kill all managed threads
  // for cleanup purposes. But it might just be better to throw the exception
  // anyway, and force the caller to catch the exception.
  if (state_ == exec_state::KILLED
      || state_ == exec_state::JOINED)
    return;

  if (state_ == exec_state::NON_INITIALIZED)
    throw std::runtime_error("Process cannot be killed");

  LOG(boost::format("Killing process %1%") % pid_);

  // State is changed before trying to kill the process, since recovering
  // from an error when trying to kill a process is difficult. Even if kill()
  // or kill_group() fail, we consider the process to be killed.
  // TODO Is there a better way to do this?
  state_ = exec_state::KILLED;

  switch (kill_mode_) {
    case kill_mode::PROC:
    util::kill(pid_);
    break;
    case kill_mode::TREE:
    util::kill_group(pid_);
    break;
  default:
    assert(false);
  }
}

void bind_process(pid_t pid, unsigned cpu) {
#ifdef __linux__
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  error_if_equal(
      sched_setaffinity(pid, sizeof(mask), &mask),
      -1,
      "Error binding process to CPU");
#else
  throw std::runtime_error("bind_process() is not supported");
#endif
}

void bind_process(const process& p, unsigned cpu) {
  bind_process(p.pid(), cpu);
}

} // namespace util
} // namespace aser

