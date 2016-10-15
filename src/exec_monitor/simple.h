#include <chrono>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <core/exec_monitor.h>

namespace aser {

class simple_monitor : public exec_monitor {
public:
  simple_monitor(
      exec_manager& exec_manager,
      const boost::property_tree::ptree& properties);

private:
  /** Interval between calls to loop_impl(). */
  std::chrono::milliseconds sampling_interval_;

  void loop_impl() final;

  /** Adds a new process.
   *
   * @param pid The pid of the process.
   */
  void add_process(pid_t pid);
};

} // namespace aser

