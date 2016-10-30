#ifndef CORE_PROCESS_MONITOR_H_
#define CORE_PROCESS_MONITOR_H_

#include <unistd.h>

#include <future>
#include <map>

#include <util/concurrent_queue.h>

namespace aser {

/** Allows for monitoring a set of processes, and waiting for events such
 * as when a process ends its execution.
 */
template<typename Process>
class process_monitor {
public:
  // TODO use a raw pointer?
  typedef std::shared_ptr<Process> process_ptr;

  process_monitor() {}

  ~process_monitor();

  /** Adds a process given its pid.
   *
   * @param pid The process identifier.
   */
  void add_pid(pid_t pid);

  /** Adds a process.
   *
   * @param process The process.
   */
  void add_process(process_ptr process);

  /** Blocks the caller until a process finishes its execution.
   *
   * @return Returns the pid and the termination status of the process.
   */
  std::pair<pid_t, int> wait_for_any();

private:
  /** Map containing the (future) status for every pid. */
  std::map<pid_t, std::future<int>> status_;

  /** Queue containing the processes that ended their execution. */
  util::concurrent_queue<pid_t> ended_process_queue_;

  void check_duplicated_pid(pid_t pid) const;

  int wait_for_pid(pid_t pid);
  int wait_for_process(process_ptr process);

  process_monitor(const process_monitor&) = delete;
  process_monitor& operator=(const process_monitor&) = delete;
};

} // namespace aser

#include <core/impl/process_monitor.h>

#endif // CORE_PROCESS_MONITOR_H_

