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
  auto pid = ended_process_queue_.pop();
  auto it = status_.find(pid);
  assert(it != end(status_));

  LOG(boost::format("wait_for_any: %1% finished") % pid);

  auto result = std::make_pair(pid, it->second.get());
  status_.erase(it);
  return result;
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

  ended_process_queue_.push(pid);
  return status;
}

int process_monitor::wait_for_process(process_ptr process) {
  process->wait();
  auto status = process->termination_status();
  ended_process_queue_.push(process->pid());
  return status;
}

} // namespace aser

