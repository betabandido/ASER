#include "cpu_hopper.h"

#include <iostream>
#include <thread>

#include <core/exec_manager.h>
#include <util/factory.h>
#include <util/log.h>
#include <util/os.h>

namespace chrono = std::chrono;
namespace pt = boost::property_tree;

namespace aser {

util::registar<exec_monitor, cpu_hopper,
    exec_manager&, const pt::ptree&>
  cpu_hopper_registar("cpu-hopper");

cpu_hopper::cpu_hopper(
    exec_manager& exec_manager,
    const pt::ptree& properties)
  : exec_monitor(exec_manager)
  , sampling_interval_{
        chrono::milliseconds(properties.get<unsigned>(
            "exec_monitor.sampling_length"))}
  , allocated_cpus_(std::thread::hardware_concurrency(), false)
{
  register_event_handler(
      exec_event::event_type::PROCESS_CREATED,
      [&](const exec_event& event) {
        add_process(event.pid);
      });
}

void cpu_hopper::loop_impl() {
  std::this_thread::sleep_for(sampling_interval_);
  hop_processes();
}

void cpu_hopper::add_process(pid_t pid) {
  LOG(boost::format("Adding process %1%") % pid);
  auto cpu = find_available_cpu();
  bind_process(pid, cpu);
  
  if (cpu_mapping_.size() == allocated_cpus_.size()) {
    // XXX current implementation requires that there is always
    // at least one unallocated CPU.
    throw std::runtime_error("Too many processes");
  }
}

void cpu_hopper::bind_process(pid_t pid, unsigned cpu) {
  try {
    util::bind_process(pid, cpu);
  } catch (const std::exception& e) {
    // TODO can we check whether the error actually took place after the
    // process ended its execution? Otherwise, it would be a real error.
    // TODO alternatively, we can log this as a potential real error, instead
    // of using the usual call to LOG, which it is used for debug purposes.
    std::cerr << boost::format("Error binding process: %1%\n") % e.what();
  }

  cpu_mapping_[pid] = cpu;
  allocated_cpus_[cpu] = true;
}

void cpu_hopper::hop_processes() {
  for (auto& m : cpu_mapping_)
    hop_process(m.first, m.second);
}

void cpu_hopper::hop_process(pid_t pid, unsigned cpu) {
  auto new_cpu = find_available_cpu();
  LOG(boost::format("Hopping process %1% from cpu %2% to cpu %3%")
     % pid % cpu % new_cpu); 
  bind_process(pid, new_cpu);
  allocated_cpus_[cpu] = false;
}

unsigned cpu_hopper::find_available_cpu() const {
  auto it = std::find(begin(allocated_cpus_), end(allocated_cpus_), false);
  if (it == end(allocated_cpus_))
    throw std::runtime_error("No available CPUs");
  return (it - begin(allocated_cpus_));
}

} // namespace aser

