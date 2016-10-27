#include <thread>

#include <core/exec_manager.h>
#include <util/log.h>

namespace aser {

template<typename EventManager>
pmc_sampler<EventManager>::pmc_sampler(
    exec_manager& exec_manager,
    const boost::property_tree::ptree& properties)
  : exec_monitor(exec_manager)
  , sampling_interval_{
      std::chrono::milliseconds(properties.get<unsigned>(
          "exec_monitor.sampling_length"))}
{
  register_event_handler(
      exec_event::event_type::PROCESS_CREATED,
      [&](const exec_event& event) {
        add_process(event.pid);
      });

  using namespace perf;

  auto default_events =
    create_generic_events(properties.get<std::string>("exec_monitor.event"));

  events_ = {
    {event_type::HARDWARE, default_events.cycles, event_modifiers::EXCLUDE_NONE},
    {event_type::HARDWARE, default_events.instructions, event_modifiers::EXCLUDE_NONE}
  };
}

template<typename EventManager>
void pmc_sampler<EventManager>::loop_impl() {
  std::this_thread::sleep_for(sampling_interval_);

  for (auto& elem : event_managers_) {
    auto& mgr = elem.second;
    auto& events = mgr.read_events(perf::event_read_mode::RELATIVE);
    for (auto& e : events)
      LOG(boost::format("enabled: %1%, count: %2%, scaling: %3%")
          % e.enabled % e.value % e.scaling);

#if 0
    assert(default_events[0].code == perf::generic_events::cycles
        && default_events[1].code == perf::generic_events::instructions);
    auto& cycles = events[0];
    auto& instrs = events[1];
    if (cycles.enabled && instrs.enabled)
      LOG(boost::format("IPC: %1%") % (instrs.value / cycles.value));
#endif
  }
}

template<typename EventManager>
void pmc_sampler<EventManager>::add_process(pid_t pid) {
  LOG(boost::format("Adding process %1%") % pid);

  // TODO add the option to bind the process to a CPU.
  // XXX create a cpu_mapper helper class to handle this.

  auto res = event_managers_.emplace(
      pid, event_manager{events_, pid, true});
  assert(res.second);
}

} // namespace aser

