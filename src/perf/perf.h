#ifndef PERF_PERF_H_
#define PERF_PERF_H_

#include <unistd.h>

#include <array>
#include <string>
#include <vector>

namespace aser {
namespace perf {

/** Event type.
 *
 * HARDWARE: generic hardware event.
 * RAW: raw event code.
 */
enum event_type { HARDWARE, RAW };

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
  int modifiers;
};

/** Generic hardware events. */
struct generic_events {
  static const uint64_t cycles;
  static const uint64_t instructions;
};

/** A performance event.
 *
 * This class manages a single peformance event for a process.
 */
class event {
public:
  /** Read mode for the event.
   *
   * An event can be read in a relative or an aggregated manner. In the first
   * case, the event count is reset every time after reading it. This is
   * useful when the goal is to obtain per-interval measurements. In the
   * second case, the event count is aggregated accross reading calls.
   */
  enum class read_mode {
    RELATIVE,
    AGGREGATED
  };

  /** An event sample. */
  struct sample {
    /** The value for the sample. */
    double value;

    /** Fraction of the time when the event was running. */
    double scaling;

    /** True if the event was enabled at any time; false otherwise. */
    bool enabled;
  };

  /** Constructor.
   *
   * @param info Event information.
   */
  event(event_info info);

  ~event();

  // event is moveable, but not copyable.
  event(event&& other) = default;
  event& operator=(event&& other) = default;

  /** Opens the event file descriptor.
   *
   * This method is capable of either attaching to an already running process,
   * or to configure the performance monitoring system so that event
   * monitoring is disabled until the program starts its execution
   * (see enable_on_exec).
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
  sample scale(read_mode mode);

private:
  enum count_field {
    RAW_VALUE = 0,
    TIME_ENABLED,
    TIME_RUNNING
  };

  /** Event information. */
  event_info info_;

  /** File descriptor for the event. */
  int fd_ { 0 };

  std::array<uint64_t, 3> count_;
  std::array<uint64_t, 3> prev_;
};

/** Event manager.
 *
 * This class handles multiple performance events for a process.
 */
class event_manager {
public:
  typedef std::vector<event::sample> sample_list;

  /** Constructor.
   *
   * @param events Information for each event.
   * @param pid Process identifier.
   * @param attach Whether to attach to a running process.
   */
  event_manager(
      const std::vector<event_info>& events,
      pid_t pid,
      bool attach);

  /** Returns the latest sample list.
   *
   * @param mode Read mode (relative or aggregated).
   * @return The list of most recent samples.
   */
  const sample_list& read_events(event::read_mode mode);

private:
  /** The events. */
  std::vector<event> events_;

  /** The latest samples for each event. */
  sample_list latest_samples_;
};

} // namespace perf
} // namespace aser

#endif // PERF_PERF_H_

