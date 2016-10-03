#ifndef CORE_EXEC_MANAGER_H_
#define CORE_EXEC_MANAGER_H_

#include <atomic>
#include <memory>

#include <boost/property_tree/ptree.hpp>

namespace aser {

class exec_monitor;

/** Base class to manage the execution of benchmarks. */
class exec_manager {
public:
  /** Constructor.
   *
   * @param properties Configuration properties.
   */
  exec_manager(const boost::property_tree::ptree& properties);

  virtual ~exec_manager();

  /** Starts the execution. */
  void start();

  /** Returns whether the executions is over.
   *
   * @return True if the execution has finished; false otherwise.
   */
  bool is_execution_over() const {
    return execution_ended_;
  }

protected:
  virtual void start_impl() = 0;

  void execution_end() {
    execution_ended_ = true;
  }

  void prepare_exec_monitor();
  void start_exec_monitor();
  void stop_exec_monitor();

private:
  typedef std::unique_ptr<exec_monitor> exec_monitor_ptr;

  exec_monitor_ptr exec_monitor_;
  std::atomic<bool> execution_ended_ { false };

  exec_manager(const exec_manager&) = delete;
  exec_manager(exec_manager&&) = delete;
  exec_manager& operator=(const exec_manager&) = delete;
  exec_manager& operator=(exec_manager&&) = delete;
};

} // namespace aser

#endif // CORE_EXEC_MANAGER_H_

