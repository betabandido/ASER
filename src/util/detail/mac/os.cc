#include <util/os.h>

#include <fcntl.h>
#include <stdexcept>

#include <util/libc_wrapper.h>

namespace aser {
namespace util {

void pipe2(int fd[2], int flags) {
  error_if_not_equal(
      ::pipe(fd),
      0,
      "Error at pipe");

  for (int i = 0; i < 2; ++i) {
    auto fcntl_flags = error_if_equal(
        fcntl(fd[i], F_GETFD),
        -1,
        "Error getting flags");

    if (flags & O_CLOEXEC)
      fcntl_flags |= FD_CLOEXEC;
    else
      throw std::invalid_argument("Flags not supported");

    error_if_equal(
        fcntl(fd[i], F_SETFD, fcntl_flags),
        -1,
        "Error settings flags");
  }
}

} // namespace util
} // namespace aser

