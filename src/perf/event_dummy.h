#ifndef PERF_EVENT_DUMMY_H_
#define PERF_EVENT_DUMMY_H_

#include <array>

#include <unistd.h>

namespace aser {
namespace perf {

class perf_dummy_impl {
public:
  using count_type = std::array<uint64_t, 3>;

  void open(const event_info& info, pid_t pid, bool attach) {}
  void close() noexcept {}

  count_type read() {
    return {0, 0, 0};
  }
};

} // namespace perf
} // namespace aser

#endif // PERF_EVENT_DUMMY_H_

