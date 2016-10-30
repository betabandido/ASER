#include <util/log.h>

namespace aser {
namespace perf {

template<typename Event>
event_manager<Event>::event_manager(
    const std::vector<event_info>& events,
    pid_t pid,
    bool attach,
    event_factory factory)
  : latest_samples_(events.size())
{
  events_.reserve(events.size());
  std::transform(begin(events), end(events),
      std::back_inserter(events_),
      [&](const event_info& info) { return factory(info); });

  for (auto& e : events_)
    e.open(pid, attach);
}

template<typename Event>
const typename event_manager<Event>::sample_list&
event_manager<Event>::read_events(event_read_mode mode) {
  for (auto& e : events_)
    e.read();

  std::transform(begin(events_), end(events_),
      begin(latest_samples_),
      [&](Event& event) { return event.scale(mode); });

  return latest_samples_;
}

template<typename Event>
Event event_manager<Event>::default_event_factory(const event_info& info) {
  return Event{info};
}

} // namespace perf
} // namespace aser

