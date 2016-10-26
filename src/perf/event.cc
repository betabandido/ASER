#include <perf/event.h>

#ifdef __linux__
#include <linux/perf_event.h>
#endif

namespace aser {
namespace perf {

// TODO find the right place for this
#ifdef __linux__
const uint64_t generic_events::cycles =
    static_cast<uint64_t>(PERF_COUNT_HW_CPU_CYCLES);
const uint64_t generic_events::instructions =
    static_cast<uint64_t>(PERF_COUNT_HW_INSTRUCTIONS);
#else
const uint64_t generic_events::cycles =
    static_cast<uint64_t>(0);
const uint64_t generic_events::instructions =
    static_cast<uint64_t>(0);
#endif

} // namespace perf
} // namespace aser

