#include <boost/filesystem.hpp>

#include <util/kill.h>
#include <util/libc_wrapper.h>
#include <util/log.h>

namespace aser {
namespace util {

template<typename Config>
process<Config>::~process() {
  if (!started() || terminated())
    return;

  try {
    kill_process(*this);
    wait();
  }
  catch (...) {}
}

template<typename Config>
void process<Config>::start() {
  LOG(boost::format("Executing %1%") % args_[0]);

  assert(!started());

  namespace fs = boost::filesystem;
  fs::path path{args_[0]};
  if (!fs::exists(path))
    throw std::invalid_argument(boost::str(
        boost::format("Path %1% does not exist") % path));

  std::vector<char*> args(args_.size() + 1, nullptr);
  std::transform(begin(args_), end(args_), begin(args),
      [](const std::string& s) { return const_cast<char*>(s.c_str()); });
  assert(args.back() == nullptr);

  config_.fork_setup();

  auto pid = ::fork();
  switch (pid) {
    case -1:
      config_.fork_error(errno);
      break;
    case 0:
      config_.exec_setup();
      ::execvp(args[0], &args[0]);
      config_.exec_error(errno);
      ::exit(1);
    default:
      pid_ = pid;
      config_.fork_success();
      break;
  }
}

template<typename Config>
void process<Config>::wait() {
  assert(started());
  assert(!terminated());

  error_if_equal(
      waitpid(pid_, &termination_status_, 0),
      -1,
      "Error waiting for process");

  if (WIFEXITED(termination_status_)
      || WIFSIGNALED(termination_status_))
    terminated_ = true;
}

template<typename Config>
pid_t process<Config>::pid() const noexcept {
  assert(started());
  return pid_;
}

template<typename Config>
int process<Config>::termination_status() const noexcept {
  assert(started());
  return termination_status_;
}

template<typename Config>
bool process<Config>::started() const noexcept {
  return pid_ != -1;
}

template<typename Config>
bool process<Config>::terminated() const noexcept {
  return terminated_;
}

template<typename Process>
void kill_process(Process& p, kill_mode mode) {
  assert(p.started());

  if (p.terminated())
    return;

  LOG(boost::format("Killing process %1%") % p.pid());

  switch (mode) {
    case kill_mode::PROC:
    util::kill(p.pid());
    break;
    case kill_mode::TREE:
    util::kill_group(p.pid());
    break;
  default:
    assert(false);
  }
}

} // namespace util
} // namespace aser

