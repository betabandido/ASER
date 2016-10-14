#include "simple.h"

#include <thread>

#include <core/exec_manager.h>
#include <util/factory.h>
#include <util/log.h>

namespace chrono = std::chrono;
namespace pt = boost::property_tree;

using namespace aser::perf;

namespace aser {

static const std::vector<event_info> default_events = {
  {event_type::HARDWARE,
    generic_events::cycles,
    event_modifiers::EXCLUDE_NONE},
  {event_type::HARDWARE,
    generic_events::instructions,
    event_modifiers::EXCLUDE_NONE}
};

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
{}

void simple_monitor::loop_impl() {
  std::this_thread::sleep_for(sampling_interval_);

  for (auto& mgr : event_managers_) {
    auto& events = mgr.read_events(event::read_mode::RELATIVE);
    for (auto& e : events)
      LOG(boost::format("enabled: %1%, count: %2%, scaling: %3%")
          % e.enabled % e.value % e.scaling);

    assert(default_events[0].code == generic_events::cycles
        && default_events[1].code == generic_events::instructions);
    auto& cycles = events[0];
    auto& instrs = events[1];
    if (cycles.enabled && instrs.enabled)
      LOG(boost::format("IPC: %1%") % (instrs.value / cycles.value));
  }
}

// TODO we might want to move the event_handler to exec_monitor,
// and let subclasses register to handle certain types of events.
void simple_monitor::event_handler_impl(const exec_event& event) {
  using event_type = exec_event::event_type;

  switch (event.type) {
  case event_type::PROCESS_CREATED:
    add_process(event.pid);
    break;
  }
}

void simple_monitor::add_process(pid_t pid) {
  LOG(boost::format("Adding process %1%") % pid);

  // TODO add the option to bind the process to a CPU.
  // XXX create a cpu_mapper helper class to handle this.

  event_managers_.emplace_back(default_events, pid, true);
}

} // namespace aser

