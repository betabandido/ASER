#include "process.h"

#include <cassert>
#include <sys/wait.h>

#include "libc_wrapper.h"
#include "log.h"
#include "os.h"

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

////////////////////
// process_config //
////////////////////

void process_config::fork_setup() {
  if (fork_setup_callback_)
    fork_setup_callback_();
}

process_config& process_config::fork_setup(callback_func func) {
  fork_setup_callback_ = func;
  return *this;
}

void process_config::fork_error(int error_code) {
  if (fork_error_callback_)
    fork_error_callback_(error_code);
}

process_config& process_config::fork_error(error_callback_func func) {
  fork_error_callback_ = func;
  return *this;
}

void process_config::exec_setup() {
  if (exec_setup_callback_)
    exec_setup_callback_();
}

process_config& process_config::exec_setup(callback_func func) {
  exec_setup_callback_ = func;
  return *this;
}

void process_config::exec_error(int error_code) {
  if (exec_error_callback_)
    exec_error_callback_(error_code);
}

process_config& process_config::exec_error(error_callback_func func) {
  exec_error_callback_ = func;
  return *this;
}

void process_config::fork_success() {
  if (fork_success_callback_)
    fork_success_callback_();
}

process_config& process_config::fork_success(callback_func func) {
  fork_success_callback_ = func;
  return *this;
}

//////////////////
// sync_process //
//////////////////

sync_process::sync_process(std::vector<std::string> args)
  : process_{std::move(args)}
  , go_pipe_{O_CLOEXEC}
{
  process_.config()
    .exec_setup([&]() { wait_for_parent(); })
    .fork_success([&]() { wait_for_child(); })
    .fork_error([](int error_code) { libc_error(error_code, "Error forking process"); })
    .exec_error([](int error_code) { libc_error(error_code, "Exec failed"); });
}

void sync_process::prepare() {
  process_.start();
}

void sync_process::start() {
  assert(process_.started());
  char buf = 0;
  go_pipe_.write(&buf, 1);
  go_pipe_.close(pipe::end_point::WRITE_END);
  on_hold_ = false;
}

void sync_process::wait() {
  assert(!on_hold_);
  process_.wait();
}

void sync_process::wait_for_parent() {
  // XXX creating a new process group allows kill_group to work safely.
  // We might want to explore other alternatives, though.
  setpgid(getpid(), 0);
  char buf;
  child_ready_pipe_.close(pipe::end_point::READ_END);
  go_pipe_.close(pipe::end_point::WRITE_END);
  child_ready_pipe_.close(pipe::end_point::WRITE_END);
  go_pipe_.read(&buf, 1);
}

void sync_process::wait_for_child() {
  char buf;
  child_ready_pipe_.close(pipe::end_point::WRITE_END);
  go_pipe_.close(pipe::end_point::READ_END);
  child_ready_pipe_.read(&buf, 1);
  child_ready_pipe_.close(pipe::end_point::READ_END);
}

} // namespace util
} // namespace aser

