#ifndef CORE_EXEC_EVENT_H_
#define CORE_EXEC_EVENT_H_

#include <unistd.h>

namespace aser {

struct exec_event {
  enum class event_type { PROCESS_CREATED };

  event_type type;
  pid_t pid;
};

} // namespace aser

#endif // CORE_EXEC_EVENT_H_

