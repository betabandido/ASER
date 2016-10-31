#include "event.h"

#include <map>

namespace aser {
namespace perf {

using generic_events_map = std::map<std::string, generic_events>;

extern const generic_events_map& __attribute__((weak)) get_generic_events_map() {
  static const generic_events_map events_map = {
    {"dummy", {0, 0}}
  };
  return events_map;
};

generic_events create_generic_events(const std::string& impl_name) {
  auto& events_map = get_generic_events_map();
  auto it = events_map.find(impl_name);
  if (it == end(events_map))
    throw std::invalid_argument("Invalid event type");

  return it->second;
}

} // namespace perf
} // namespace aser

