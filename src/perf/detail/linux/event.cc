#include <map>

#include <perf/event.h>

#ifndef __linux__
#error This file must only be included in linux builds.
#endif

#include <linux/perf_event.h>

namespace aser {
namespace perf {

using generic_events_map = std::map<std::string, generic_events>;

const generic_events_map& get_generic_events_map() {
  static const generic_events_map events_map = {
    {"linux", {static_cast<uint64_t>(PERF_COUNT_HW_CPU_CYCLES),
                static_cast<uint64_t>(PERF_COUNT_HW_INSTRUCTIONS)}},
    {"dummy", {0, 0}}
  };
  return events_map;
};

} // namespace perf
} // namespace aser

