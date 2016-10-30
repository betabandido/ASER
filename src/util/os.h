#ifndef UTIL_OS_H_
#define UTIL_OS_H_

#include <unistd.h>

namespace aser {
namespace util {

void pipe2(int fd[2], int flags);

/** Binds the process matching the given pid to the given CPU.
 *
 * @param pid The process identifier.
 * @param cpu The virtual CPU.
 */
void bind_process(pid_t pid, unsigned cpu);

/** Binds the given process to the given CPU.
 *
 * @param pid The process.
 * @param cpu The virtual CPU.
 */
template<typename Process>
void bind_process(const Process& p, unsigned cpu) {
  bind_process(p.pid(), cpu);
}

} // namespace util
} // namespace aser

#endif // UTIL_OS_H_

