#include "simple.h"

#include <thread>

#include <core/exec_manager.h>
#include <util/factory.h>
#include <util/log.h>

namespace chrono = std::chrono;
namespace pt = boost::property_tree;

namespace aser {

util::registar<exec_monitor, simple_monitor,
    exec_manager&, const pt::ptree&>
  simple_monitor_registar("simple");

simple_monitor::simple_monitor(
    exec_manager& exec_manager,
    const pt::ptree& properties)
  : exec_monitor(exec_manager)
  , sampling_interval_{
        chrono::milliseconds(properties.get<unsigned>(
            "exec_monitor.sampling_length"))}
{
  register_event_handler(
      exec_event::event_type::PROCESS_CREATED,
      [&](const exec_event& event) {
        add_process(event.pid);
      });
}

void simple_monitor::loop_impl() {
  std::this_thread::sleep_for(sampling_interval_);
}

void simple_monitor::add_process(pid_t pid) {}

} // namespace aser

