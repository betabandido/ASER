#ifndef UTIL_PROCESS_H_
#define UTIL_PROCESS_H_

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <string>
#include <vector>

namespace aser {
namespace util {

/** Class representing a pipe between processes.
 *
 * TODO Implement reading and writing in a C++ way.
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
  pipe(pipe&& other);
  pipe& operator=(pipe&& other);

  pipe(const pipe& other) = delete;
  pipe& operator=(const pipe& other) = delete;

  /** Checks whether the given endpoint is open.
   *
   * @param ep Endpoint to check.
   * @return True if the endpoint is open; false otherwise.
   */
  bool is_open(end_point ep) const;

  /** Closes and endpoint.
   *
   * @param ep Endpoint to close.
   */
  void close(end_point ep); 

  /** Reads from the pipe.
   *
   * @param buffer Buffer where data will be placed.
   * @param nbyte Number of bytes to read.
   * @return Number of bytes actually read.
   */
  size_t read(char* buffer, size_t nbyte);

  /** Writes to a pipe.
   *
   * @param buffer Buffer where data is located.
   * @param nbyte Number of bytes to write.
   * @return Number of bytes actually written.
   */
  size_t write(const char* buffer, size_t nbyte);

private:
  /** File descriptors. */
  std::array<int, 2> fd_ {{ -1, -1 }};
};

/** Class responsible to manage the creation of a process and the following
 * execution of a command.
 *
 * TODO Think of the most natural way to signal errors when methods are not
 * called in the expected order. Should these errors be handled with
 * assertions of just with regular exceptions?
 */
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

  // TODO do we really need this constructor?
  process(const std::vector<std::string>& args)
    : args_{args}
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

  /** Prepares the execution of the command.
   *
   * This method creates a process, and then starts the execution of the
   * command given in the arguments in the constructor. But, the command
   * execution is put on hold until start() is called.
   */
  void prepare();

  /** Starts the execution of the command. */
  void start();

  /** Waits for process termination. */
  void wait();

  /** Returns the termination status. */
  int termination_status() const;

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
  void kill();

private:
  enum class exec_state {
    NON_INITIALIZED,
    READY,
    RUNNING,
    KILLED,
    JOINED
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

  /** Termination status. */
  int termination_status_ { 0 };
};

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
void bind_process(const process& p, unsigned cpu);

} // namespace util
} // namespace aser

#endif // UTIL_PROCESS_H_

