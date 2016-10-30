#ifndef PERF_EVENT_H_
#define PERF_EVENT_H_

#include <unistd.h>

#include <array>
#include <string>

namespace aser {
namespace perf {

/** Event type.
 *
 * HARDWARE: generic hardware event.
 * RAW: raw event code.
 */
enum class event_type { HARDWARE, RAW };

/** Event modifiers.
 *
 * The following flags determine whether events originating at a certain
 * level are excluded. For instance, if EXCLUDE_KERNEL is used, events that
 * happen in kernel-space will be excluded.
 */
enum event_modifiers {
  EXCLUDE_NONE = 0,
  EXCLUDE_USER = 1,
  EXCLUDE_KERNEL = 2,
  EXCLUDE_HV = 4
};

/** Event information. */
struct event_info {
  event_type type;
  uint64_t code;
  uint8_t modifiers;
};

/** Generic hardware events. */
struct generic_events {
  const uint64_t cycles;
  const uint64_t instructions;
};

/** Returns the generic events for a given event implementation.
 *
 * @param type Event implementation name.
 * @return The generic events associated to the given implementation.
 */
generic_events create_generic_events(const std::string& impl_name);

/** Read mode for the event.
 *
 * An event can be read in a relative or an aggregated manner. In the first
 * case, the event count is reset every time after reading it. This is
 * useful when the goal is to obtain per-interval measurements. In the
 * second case, the event count is aggregated accross reading calls.
 */
enum class event_read_mode {
  RELATIVE,
  AGGREGATED
};

/** An event sample. */
struct event_sample {
  /** The value for the sample. */
  double value;

  /** Fraction of the time when the event was running. */
  double scaling;

  /** True if the event was enabled at any time; false otherwise. */
  bool enabled;
};

/** A performance event.
 *
 * This class manages a single peformance event for a process.
 */
template<class Impl>
class event {
public:
  /** Constructor.
   *
   * @param info Event information.
   * @param impl Event implementation details.
   */
  event(event_info info, Impl impl = Impl{});

  // event is moveable, but not copyable.
  event(event&& other) = default;
  event& operator=(event&& other) = default;

  /** Opens the event file descriptor.
   *
   * This method is capable of either attaching to an already running process,
   * or to configure the performance monitoring system so that event
   * monitoring is disabled until the program starts its execution
   * (see enable_on_exec in Linux).
   *
   * @param pid Process identifier.
   * @param attach Whether to attach to a running process.
   */
  void open(pid_t pid, bool attach);

  /** Reads the event values into an internal buffer. */
  void read();

  /** Scales the value read, and returns the latest sample.
   *
   * @param mode Read mode (relative or aggregated).
   * @return The latest sample read.
   */
  event_sample scale(event_read_mode mode);

private:
  enum count_field {
    RAW_VALUE = 0,
    TIME_ENABLED,
    TIME_RUNNING
  };

  using count_type = std::array<uint64_t, 3>;

  /** Event information. */
  event_info info_;

  /** Implementation details. */
  Impl impl_;

  /** Current counter values. */
  count_type count_;

  /** Previous counter values. */
  count_type prev_;

  static bool valid_modifiers(uint8_t modifiers);
};

} // namespace perf
} // namespace aser

#include <perf/impl/event.h>

#endif // PERF_EVENT_H_

