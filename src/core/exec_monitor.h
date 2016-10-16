#ifndef CORE_EXEC_MONITOR_H_
#define CORE_EXEC_MONITOR_H_

#include <unistd.h>

#include <functional>
#include <map>
#include <thread>

#include <core/exec_event.h>
#include <util/concurrent_queue.h>

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

  /** Enqueues an execution event.
   *
   * @param event The execution event.
   */
  void enqueue_event(exec_event event);

  /** Handles an execution event.
   *
   * @param event The execution event.
   */
  void event_handler(const exec_event& event);

protected:
  /** Function definition for an event handler. */
  typedef std::function<void(const exec_event&)> event_handler_func;

  virtual void initialize_impl() {}
  virtual void finalize_impl() {}
  virtual void prepare_impl() {}

  /** Method that should be implemented in derived monitors with the actual
   * logic that will monitor the execution.
   */
  virtual void loop_impl() = 0;

  /** Registers and event handler.
    *
    * @param type Event type.
    * @param func Handler function.
    */
  void register_event_handler(
      const exec_event::event_type& type,
      const event_handler_func& func);

private:
  /** Associated exec_manager. */
  exec_manager& exec_manager_;

  /** Indicates whether initialize() has been called. */
  bool initialized_ { false };

  /** Handle for the thread that runs the loop implementation. */
  std::thread thread_;

  /** Event queue. */
  util::concurrent_queue<exec_event> event_queue_;

  /** Event handlers. */
  std::map<exec_event::event_type, event_handler_func> event_handlers_;

  exec_monitor(const exec_monitor&) = delete;
  exec_monitor(exec_monitor&&) = delete;
  exec_monitor& operator=(const exec_monitor&) = delete;
  exec_monitor& operator=(exec_monitor&&) = delete;

  /** Processes pending events. */
  void process_events();
};

} // namespace aser

#endif // CORE_EXEC_MONITOR_H_

