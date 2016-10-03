#ifndef CORE_EXEC_MONITOR_H_
#define CORE_EXEC_MONITOR_H_

#include <thread>

//#include <core/thread.h>

namespace aser {

class exec_manager;

/** Base class to monitor the execution of benchmarks. */
class exec_monitor {
public:
  /** Constructor.
   *
   * @param exec_manager Execution manager in charge of the execution.
   */
  exec_monitor(exec_manager& exec_manager);

  virtual ~exec_monitor() = default;

  /** Initializes the execution monitor. */
  void initialize();

  /** Finalizes the execution monitor. */
  void finalize();

  /** Prepares the execution monitor.
   *
   * Once the execution is ready to start, this method allows the execution
   * monitor to become ready to monitor the execution. */
  void prepare();

  /** Starts the execution monitor. */
  void start();

  /** Waits until the execution monitor finishes. */
  void join();

//  void thread_created(const thread& thread);
//  void thread_ended(const thread& thread);

protected:
//  exec_manager& get_exec_manager() const {
//    return exec_manager_;
//  }

  virtual void initialize_impl() {}
  virtual void finalize_impl() {}
  virtual void prepare_impl() {}

  /** Method that should be implemented in derived monitors with the actual
   * logic that will monitor the execution.
   */
  virtual void loop_impl() = 0;

//  virtual void thread_created_impl(const thread& thread) = 0;
//  virtual void thread_ended_impl(const thread& thread) = 0;

private:
  exec_manager& exec_manager_;
  bool initialized_ { false };
  std::thread thread_;

  exec_monitor(const exec_monitor&) = delete;
  exec_monitor(exec_monitor&&) = delete;
  exec_monitor& operator=(const exec_monitor&) = delete;
  exec_monitor& operator=(exec_monitor&&) = delete;
};

} // namespace aser

#endif // CORE_EXEC_MONITOR_H_

