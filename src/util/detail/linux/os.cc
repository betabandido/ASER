#include <util/os.h>

#include <util/libc_wrapper.h>

#ifndef __linux__
#error This file must only be included in linux builds.
#endif

namespace aser {
namespace util {

void pipe2(int fd[2], int flags) {
  error_if_not_equal(
      ::pipe2(fd, flags),
      0,
      "Error at pipe2");
}

void bind_process(pid_t pid, unsigned cpu) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  error_if_equal(
      sched_setaffinity(pid, sizeof(mask), &mask),
      -1,
      "Error binding process to CPU");
}
} // namespace util
} // namespace aser

