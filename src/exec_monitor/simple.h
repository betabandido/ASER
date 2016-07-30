#include <chrono>

#include <boost/property_tree/ptree.hpp>

#include <core/exec_monitor.h>

namespace aser {

class simple_monitor : public exec_monitor {
public:
  simple_monitor(
      exec_manager& exec_manager,
      const boost::property_tree::ptree& properties);

private:
  std::chrono::milliseconds sampling_interval_;

  void loop_impl() final;
};

} // namespace aser

