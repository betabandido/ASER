#include "perf.h"

#ifdef __linux__
#include <asm/unistd.h>
#include <linux/perf_event.h>
#endif

#include <cstring>

#include <util/libc_wrapper.h>
#include <util/log.h>

namespace aser {
namespace perf {

#ifdef __linux__
const uint64_t generic_events::cycles =
  static_cast<uint64_t>(PERF_COUNT_HW_CPU_CYCLES);
const uint64_t generic_events::instructions =
  static_cast<uint64_t>(PERF_COUNT_HW_INSTRUCTIONS);
#else
const uint64_t generic_events::cycles = static_cast<uint64_t>(0);
const uint64_t generic_events::instructions = static_cast<uint64_t>(0);
#endif

event::event(event_info info)
  : info_(std::move(info))
  , prev_{0, 0, 0}
{}

event::~event() {
  if (fd_ != 0)
    close(fd_);
}

void event::open(pid_t pid, bool attach) {
#ifdef __linux__
  perf_event_attr attr;
  memset(&attr, 0, sizeof(attr));
  attr.config = info_.code;

  if (info_.type == event_type::HARDWARE)
    attr.type = PERF_TYPE_HARDWARE;
  else if (info_.type == event_type::RAW)
    attr.type = PERF_TYPE_RAW;
  else
    assert(false);

  attr.read_format =
    PERF_FORMAT_TOTAL_TIME_ENABLED
    | PERF_FORMAT_TOTAL_TIME_RUNNING;
  attr.exclude_user = info_.modifiers & event_modifiers::EXCLUDE_USER;
  attr.exclude_kernel = info_.modifiers & event_modifiers::EXCLUDE_KERNEL;
  attr.exclude_hv = info_.modifiers & event_modifiers::EXCLUDE_HV;
  attr.inherit = 1;
  attr.disabled = !attach;
  attr.enable_on_exec = !attach;
  attr.size = sizeof(attr);
 
  fd_ = util::error_if_equal(
      syscall(__NR_perf_event_open, &attr, pid, -1, -1, 0),
      static_cast<long>(-1),
      "Error opening the event");
#else
  throw std::runtime_error("No support for performance counters");
#endif
}

void event::read() {
  util::error_if_not_equal(
      ::read(fd_, count_.data(), sizeof(count_)),
      static_cast<ssize_t>(sizeof(count_)),
      "Error reading PMC event");
}

event::sample event::scale(read_mode mode) {
  auto value = count_[count_field::RAW_VALUE];
  auto enabled = count_[count_field::TIME_ENABLED];
  auto running = count_[count_field::TIME_RUNNING];

  LOG(boost::format("value: %1%, enabled: %2%, running: %3%")
      % value % enabled % running);

  if (mode == read_mode::RELATIVE) {
    value -= prev_[count_field::RAW_VALUE];
    enabled -= prev_[count_field::TIME_ENABLED];
    running -= prev_[count_field::TIME_RUNNING];
    prev_ = count_;
  }

  if (running == 0)
    return {0, 0, enabled > 0};

  assert(enabled > 0);
  auto scaling = static_cast<double>(running) / enabled;
  return {value / scaling, scaling, true};
}

event_manager::event_manager(
    const std::vector<event_info>& events,
    pid_t pid,
    bool attach)
  : latest_samples_(events.size())
{
  std::transform(begin(events), end(events), std::back_inserter(events_),
      [](const event_info& info) { return event(info); });

  for (auto& e : events_)
    e.open(pid, attach);
}

const event_manager::sample_list&
event_manager::read_events(event::read_mode mode) {
  for (auto& e : events_)
    e.read();

  std::transform(begin(events_), end(events_), begin(latest_samples_),
      [&](event& event) { return event.scale(mode); });

  return latest_samples_;
}

} // namespace perf
} // namespace aser

