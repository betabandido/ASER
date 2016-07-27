#include "process_monitor.h"

#include <sys/wait.h>

#include <util/log.h>
#include <util/process.h>

namespace aser {

void process_monitor::add_pid(pid_t pid) {
  check_duplicated_pid(pid);
  status_[pid] = std::async(
      std::launch::async,
      [=]() { return wait_for_pid(pid); });
}

void process_monitor::add_process(process_ptr process) {
  check_duplicated_pid(process->pid());
  status_[process->pid()] = std::async(
      std::launch::async,
      [=]() { return wait_for_process(process); });
}

void process_monitor::check_duplicated_pid(pid_t pid) const {
  if (status_.find(pid) != end(status_))
    throw std::invalid_argument("Duplicated pid");
}

std::pair<pid_t, int> process_monitor::wait_for_any() {
  process_ending_.wait();

  // XXX We might want to avoid this loop for performance reasons.
  for (auto it = begin(status_); it != end(status_); ++it) {
    if (util::is_ready(it->second)) {
      auto result = std::make_pair(it->first, it->second.get());
      status_.erase(it);
      return result;
    }
  }

  throw std::runtime_error("No processes to wait for");
}

int process_monitor::wait_for_pid(pid_t pid) {
  LOG(boost::format("Waiting for pid %1%") % pid);

  // XXX think what's the proper way to notify the owner
  // of process_monitor about a failure.
  
  int status;
  auto res = waitpid(pid, &status, 0);
  if (res == -1)
    throw std::runtime_error("Error waiting for process");
  if (res != pid)
    throw std::runtime_error("Unexpected error");

  process_ending_.notify();
  return status;
}

int process_monitor::wait_for_process(process_ptr process) {
  process->wait();
  auto status = process->termination_status();
  process_ending_.notify();
  return status;
}

} // namespace aser

