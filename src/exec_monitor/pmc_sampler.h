#include <chrono>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <core/exec_monitor.h>
#include <perf/event.h>
#include <perf/event_dummy.h>
#include <perf/event_linux.h>
#include <perf/event_manager.h>

namespace aser {

class pmc_sampler : public exec_monitor {
public:
  pmc_sampler(
      exec_manager& exec_manager,
      const boost::property_tree::ptree& properties);

private:
  //using event_type = perf::event<perf::perf_linux_impl>;
  using event_type = perf::event<perf::perf_dummy_impl>;
  using event_manager = perf::event_manager<event_type>;

  /** Interval between samples. */
  std::chrono::milliseconds sampling_interval_;

  /** PMCs event managers. */
  std::vector<event_manager> event_managers_;

  void loop_impl() final;

  /** Adds a new process.
   *
   * @param pid The pid of the process.
   */
  void add_process(pid_t pid);
};

} // namespace aser

