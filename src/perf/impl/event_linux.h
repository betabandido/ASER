#include <asm/unistd.h>
#include <linux/perf_event.h>

#include <cstring>

#include <util/libc_wrapper.h>

namespace aser {
namespace perf {

event_linux_impl::~event_linux_impl() {
  if (fd_ != 0)
    close();
}

void event_linux_impl::open(const event_info& info, pid_t pid, bool attach) {
  perf_event_attr attr;
  memset(&attr, 0, sizeof(attr));
  attr.config = info.code;

  if (info.type == event_type::HARDWARE)
    attr.type = PERF_TYPE_HARDWARE;
  else if (info.type == event_type::RAW)
    attr.type = PERF_TYPE_RAW;
  else
    assert(false);

  attr.read_format =
    PERF_FORMAT_TOTAL_TIME_ENABLED
    | PERF_FORMAT_TOTAL_TIME_RUNNING;
  attr.exclude_user = info.modifiers & event_modifiers::EXCLUDE_USER;
  attr.exclude_kernel = info.modifiers & event_modifiers::EXCLUDE_KERNEL;
  attr.exclude_hv = info.modifiers & event_modifiers::EXCLUDE_HV;
  attr.inherit = 1;
  attr.disabled = !attach;
  attr.enable_on_exec = !attach;
  attr.size = sizeof(attr);
 
  fd_ = util::error_if_equal(
      syscall(__NR_perf_event_open, &attr, pid, -1, -1, 0),
      static_cast<long>(-1),
      "Error opening the event");
}

void event_linux_impl::close() noexcept {
  try {
    util::error_if_not_equal(
        ::close(fd_),
        0,
        "Error closing the event");
  } catch (const std::exception& e) {
    // TODO log this (in a safe way)
  }
}

event_linux_impl::count_type event_linux_impl::read() {
  count_type count;
  util::error_if_not_equal(
      ::read(fd_, count.data(), sizeof(count)),
      static_cast<ssize_t>(sizeof(count)),
      "Error reading PMC event");
  return count;
}

} // namespace perf
} // namespace aser

