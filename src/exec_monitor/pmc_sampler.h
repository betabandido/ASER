#ifndef EXEC_MONITOR_PMC_SAMPLER_H_
#define EXEC_MONITOR_PMC_SAMPLER_H_

#include <chrono>
#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <core/exec_monitor.h>
#include <perf/event.h>

namespace aser {

template<typename EventManager>
class pmc_sampler : public exec_monitor {
public:
  pmc_sampler(
      exec_manager& exec_manager,
      const boost::property_tree::ptree& properties);

private:
  using event_manager = EventManager;

  /** Interval between samples. */
  std::chrono::milliseconds sampling_interval_;

  /** Events to measure. */
  std::vector<perf::event_info> events_;

  /** PMCs event managers. */
  std::map<pid_t, event_manager> event_managers_;

  void loop_impl() final;

  /** Adds a new process.
   *
   * @param pid The pid of the process.
   */
  void add_process(pid_t pid);
};

} // namespace aser

#include <exec_monitor/impl/pmc_sampler.h>

#endif // EXEC_MONITOR_PMC_SAMPLER_H_

