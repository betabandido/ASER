#include <util/os.h>

#include <fcntl.h>
#include <stdexcept>

#include <util/libc_wrapper.h>

#ifndef __APPLE__
#error This file must only be included in OS X builds.
#endif

namespace aser {
namespace util {

void pipe2(int fd[2], int flags) {
  error_if_not_equal(
      ::pipe(fd),
      0,
      "Error creating pipe");

  for (int i = 0; i < 2; ++i) {
    auto fcntl_flags = error_if_equal(
        fcntl(fd[i], F_GETFD),
        -1,
        "Error getting flags");

    if (flags & ~O_CLOEXEC)
      throw std::invalid_argument("Flags not supported");

    if (flags & O_CLOEXEC)
      fcntl_flags |= FD_CLOEXEC;

    error_if_equal(
        fcntl(fd[i], F_SETFD, fcntl_flags),
        -1,
        "Error settings flags");
  }
}

void bind_process(pid_t pid, unsigned cpu) {
  throw std::logic_error("bind_process() is not supported on this platform");
}

} // namespace util
} // namespace aser

