#ifndef PERF_EVENT_MANAGER_H_
#define PERF_EVENT_MANAGER_H_

#include <unistd.h>

#include <array>
#include <functional>
#include <vector>

namespace aser {
namespace perf {

/** Event manager.
 *
 * This class handles multiple performance events for a process.
 */
template<typename Event>
class event_manager {
public:
  using event_factory = std::function<Event(const event_info&)>;
  using event_list = std::vector<Event>;
  using sample_list = std::vector<event_sample>;

  /** Constructor.
   *
   * @param events Information for each event.
   * @param pid Process identifier.
   * @param attach Whether to attach to a running process.
   * @param factory Event factory.
   */
  event_manager(
      const std::vector<event_info>& events,
      pid_t pid,
      bool attach,
      event_factory factory = default_event_factory
      );

  /** Returns the latest sample list.
   *
   * @param mode Read mode (relative or aggregated).
   * @return The list of most recent samples.
   */
  const sample_list& read_events(event_read_mode mode);

private:
  /** The events. */
  event_list events_;

  /** The latest samples for each event. */
  sample_list latest_samples_;

  /** Default event factory. */
  static Event default_event_factory(const event_info& info);
};

} // namespace perf
} // namespace aser

#include <perf/impl/event_manager.h>

#endif // PERF_EVENT_MANAGER_H_

