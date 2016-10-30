#ifndef UTIL_PROCESS_H_
#define UTIL_PROCESS_H_

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <functional>
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

/** Process configuration. */
class process_config {
public:
  using callback_func = std::function<void()>;
  using error_callback_func = std::function<void(int)>;

  void fork_setup();
  void exec_setup();
  void fork_success();
  void fork_error(int error_code);
  void exec_error(int error_code);

  process_config& fork_setup(callback_func func);
  process_config& exec_setup(callback_func func);
  process_config& fork_success(callback_func func);
  process_config& fork_error(error_callback_func func);
  process_config& exec_error(error_callback_func func);

private:
  callback_func fork_setup_callback_;
  callback_func exec_setup_callback_;
  callback_func fork_success_callback_;
  error_callback_func fork_error_callback_;
  error_callback_func exec_error_callback_;
};

/** Class responsible to manage the creation of a process and the following
 * execution of a command.
 */
template<typename Config>
class process {
public:
  /** Constructor
   *
   * @param path Path to the executable.
   * @param args Arguments.
   */
  template<typename... Args>
  process(std::string&& path, Args&&... args)
    : process{std::vector<std::string>{std::move(path), std::forward<Args>(args)...}}
  {}

  process(std::vector<std::string> args, Config config = Config{})
    : args_{args}
    , config_{std::move(config)}
  {
    if (args_.empty())
      throw std::invalid_argument("Argument list is empty");
  }

  /** Destructor.
   *
   * The process is killed if it is still running.
   */
  ~process();

  // Moving is allowed, but not copying.
  process(process&& other) = default;
  process& operator=(process&& other) = default;

  process(const process& other) = delete;
  process& operator=(const process& other) = delete;

  Config& config() {
    return config_;
  }

  /** Returns whether the process has started its execution. */
  bool started() const noexcept;

  /** Returns whether the process has terminated its execution. */
  bool terminated() const noexcept;

  /** Starts the execution of the command. */
  void start();

  /** Waits for process termination. */
  void wait();

  /** Returns the process identifier
   *
   * @return The process identifier.
   */
  pid_t pid() const noexcept;

  /** Returns the termination status. */
  int termination_status() const noexcept;

private:
  /** Process configuration. */
  Config config_;

  /** Arguments to pass to exec(). */
  std::vector<std::string> args_;

  /** Process identifier. */
  pid_t pid_ { -1 };

  /** Termination status. */
  int termination_status_;

  /** Whether the process is terminated. */
  bool terminated_ { false };
};

/** The kill mode determines the behavior of kill().
 *
 * If PROC is used, just this process is killed.
 * If TREE is used, this process and all of its children are killed.
 */
enum class kill_mode { PROC, TREE };

/** Kills a process.
 *
 * If kill_mode is KILL_TREE this method also kills all of the process' children.
 * The implementation kills all the processes with the same group identifier.
 * This should be equivalent to killing all the children.
 */
template<typename Process>
void kill_process(Process& p, kill_mode mode = kill_mode::PROC);

class sync_process {
public:
  template<typename... Args>
  sync_process(std::string&& path, Args&&... args)
    : sync_process{std::vector<std::string>{std::move(path), std::forward<Args>(args)...}}
  {}

  sync_process(std::vector<std::string> args);

  /** Prepares the execution of the command.
   *
   * This method creates a process, and then starts the execution of the
   * command given in the arguments in the constructor. But, the command
   * execution is put on hold until start() is called.
   */
  void prepare();

  /** Starts the execution of the command for real. */
  void start();

  /** Waits for process termination. */
  void wait();

  bool started() const noexcept {
    return process_.started();
  }

  bool terminated() const noexcept {
    return process_.terminated();
  }

  pid_t pid() const noexcept {
    return process_.pid();
  }

  int termination_status() const noexcept {
    return process_.termination_status();
  }

private:
  process<process_config> process_;

  /** Pipe to notify the child that it should start the execution. */
  pipe go_pipe_;

  /** Pipe to notify the parent that the children is ready to call exec(). */
  pipe child_ready_pipe_;

  /** Whether the execution is on hold, waiting for start() to be called. */
  bool on_hold_ { true };

  void wait_for_parent();
  void wait_for_child();
};

} // namespace util
} // namespace aser

#include <util/impl/process.h>

#endif // UTIL_PROCESS_H_

