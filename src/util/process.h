#ifndef UTIL_PROCESS_H_
#define UTIL_PROCESS_H_

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <string>
#include <vector>

namespace util {

/** Class representing a pipe between processes.
 *
 * TODO Implement reading and writing (not needed for the current usage).
 */
class pipe {
public:
  enum class end_point { READ_END = 0, WRITE_END };

  /** Constructor. */
  pipe();

  /** Constructor.
   *
   * @param flags Flags to use to create the pipe.
   */
  pipe(int flags);

  /** Destructor. 
   *
   * The pipe is automatically closed.
   */
  ~pipe();

  // Moving is allowed, but not copying.
  pipe(const pipe& other) = delete;
  pipe(pipe&& other);
  pipe& operator=(const pipe& other) = delete;
  pipe& operator=(pipe&& other);

  /** Closes and endpoint.
   *
   * @param ep Endpoint to close.
   */
  void close(end_point ep); 

private:
  /** File descriptors. */
  std::array<int, 2> fd_ {{ -1, -1 }};
};

class process {
public:
  /** The kill mode determines the behavior of kill().
   *
   * If PROC is used, just this process is killed.
   * If TREE is used, this process and all of its children are killed.
   */
  enum class kill_mode { PROC, TREE };

  /** Constructor
   *
   * @param path Path to the executable.
   * @param args Arguments.
   */
  template<typename... Args>
  process(std::string&& path, Args&&... args)
    : args_{std::move(path), std::forward<Args>(args)...}
    , go_pipe_(O_CLOEXEC)
  {}

  /** Destructor.
   *
   * The process is killed if it is still running.
   */
  ~process();

  // Moving is allowed, but not copying.
  process(const process& other) = delete;
  process(process&& other) = default;
  process& operator=(const process& other) = delete;
  process& operator=(process&& other) = default;

  /** Returns the process identifier
   *
   * @return The process identifier.
   */
  pid_t pid() const { return pid_; }

  void prepare();

  /** Sets the kill mode.
   *
   * @param mode The kill mode.
   */
  void set_kill_mode(kill_mode mode) {
    kill_mode_ = mode;
  }

  /** Kills the process.
   *
   * If kill_mode is KILL_TREE this method also kills all of the process' children.
   * The implementation kills all the processes with the same group identifier.
   * This should be equivalent to killing all the children.
   */
  void kill() const;

private:
  enum class exec_state {
    NON_INITIALIZED,
    READY,
    RUNNING
  };

  /** Arguments to pass to exec(). */
  std::vector<std::string> args_;

  /** Process identifier. */
  pid_t pid_ { -1 };

  /** Execution state. */
  exec_state state_ { exec_state::NON_INITIALIZED };

  /** Kill mode. */
  kill_mode kill_mode_ { kill_mode::TREE };

  /** Pipe to notify the child that it should start the execution. */
  pipe go_pipe_;
};

} // namespace util

#endif // UTIL_PROCESS_H_

