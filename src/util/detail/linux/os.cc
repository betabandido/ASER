#include <util/os.h>

#include <util/libc_wrapper.h>

namespace aser {
namespace util {

void pipe2(int fd[2], int flags) {
  error_if_not_equal(
      ::pipe2(fd, flags),
      0,
      "Error at pipe2");
}

} // namespace util
} // namespace aser

