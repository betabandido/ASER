#ifndef UTIL_KILL_H_
#define UTIL_KILL_H_

#include <unistd.h>

namespace aser {
namespace util {

/** Kills a process.
 *
 * First SIGTERM is sent to the process. Immediately after that SIGKILL is sent.
 * TODO We might want to wait some time before the two signals.
 *
 * @param The identifier of the process to kill.
 */
void kill(pid_t pid);

/** Kills a process group.
 *
 * Kills the process group that the given process belongs to.
 *
 * @params pid The identifier of the process to kill, along with the rest of its group.
 */
void kill_group(pid_t pid);

} // namespace util
} // namespace aser

#endif // UTIL_KILL_H_

