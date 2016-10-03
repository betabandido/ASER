#include <chrono>
#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <core/exec_monitor.h>

namespace aser {

/** This class implements a simple execution monitor that hops processes
 * around in a system. When a process is created, it is assigned a CPU.
 * After a programmable amount of time, all processes under the control
 * of the execution manager will hop to another CPU. This process continues
 * until the execution finishes.
 */
class cpu_hopper : public exec_monitor {
public:
  cpu_hopper(
      exec_manager& exec_manager,
      const boost::property_tree::ptree& properties);

private:
  typedef std::map<pid_t, unsigned> cpu_mapping;
  typedef std::vector<bool> cpu_allocation;

  /** Sampling interval between hops. */
  std::chrono::milliseconds sampling_interval_;

  /** Process to CPU mapping. */
  cpu_mapping cpu_mapping_;

  /** Map of allocated CPUs. */
  cpu_allocation allocated_cpus_;

  void loop_impl() final;

  void event_handler_impl(const exec_event& event) final;

  /** Adds a new process.
   *
   * @param pid The pid of the process.
   */
  void add_process(pid_t pid);

  /** Binds a process to a CPU.
   *
   * @param pid The pid of the process.
   * @param cpu The CPU number.
   */
  void bind_process(pid_t pid, unsigned cpu);

  /** Hops all processes. */
  void hop_processes();

  /** Hops a single process.
   *
   * @param pid The pid of the process.
   * @param cpu The new CPU number.
   */
  void hop_process(pid_t pid, unsigned cpu);

  /** Find an available (unallocated) CPU.
   *
   * @return The number of the available CPU.
   */
  unsigned find_available_cpu() const;
};

} // namespace aser

