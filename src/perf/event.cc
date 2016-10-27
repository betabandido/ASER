#include "event.h"

#include <map>

#ifdef __linux__
#include <linux/perf_event.h>
#endif

namespace aser {
namespace perf {

static const std::map<std::string, generic_events> generic_events_map = {
#ifdef __linux__
  {"linux", {static_cast<uint64_t>(PERF_COUNT_HW_CPU_CYCLES),
              static_cast<uint64_t>(PERF_COUNT_HW_INSTRUCTIONS)}},
#endif
  {"dummy", {0, 0}}
};

generic_events create_generic_events(const std::string& impl_name) {
  auto it = generic_events_map.find(impl_name);
  if (it == end(generic_events_map))
    throw std::invalid_argument("Invalid event type");

  return it->second;
}

} // namespace perf
} // namespace aser

