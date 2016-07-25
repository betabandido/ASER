#include "kill.h"

#include <signal.h>
#include <unistd.h>

#include "libc_wrapper.h"

namespace aser {
namespace util {

void kill(pid_t pid) {
  error_if_equal(::kill(pid, SIGKILL), -1, "Error killing a process");
}

void kill_group(pid_t pid) {
  int gid = error_if_equal(getpgid(pid), -1, "Error getting the group id");
  error_if_equal(::kill(-gid, SIGKILL), -1, "Error killing a process group");
}

} // namespace util
} // namespace aser

