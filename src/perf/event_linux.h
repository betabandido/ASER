#ifndef PERF_EVENT_LINUX_H_
#define PERF_EVENT_LINUX_H_

#ifdef __linux__

#include <array>

#include <unistd.h>

namespace aser {
namespace perf {

class event_linux_impl {
public:
  using count_type = std::array<uint64_t, 3>;

  ~event_linux_impl();

  void open(const event_info& info, pid_t pid, bool attach);
  void close() noexcept;

  count_type read();

private:
  /** File descriptor for the event. */
  int fd_ { 0 };
};

} // namespace perf
} // namespace aser

#include <perf/impl/event_linux.h>

#endif // __linux__

#endif // PERF_EVENT_LINUX_H_

