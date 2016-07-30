#include "exec_monitor.h"

#include <cassert>

#include <core/exec_manager.h>

namespace aser {

exec_monitor::exec_monitor(exec_manager& exec_manager)
  : exec_manager_(exec_manager)
{}

void exec_monitor::initialize() {
  assert(!initialized_);
  initialize_impl();
  initialized_ = true;
}

void exec_monitor::finalize() {
  assert(initialized_);
  finalize_impl();
}

void exec_monitor::before_exec() {
  assert(initialized_);
  before_exec_impl();
}

void exec_monitor::after_exec() {
  assert(initialized_);
  after_exec_impl();
}

void exec_monitor::start() {
  assert(initialized_);
  thread_ = std::thread([&] {
    while (!exec_manager_.is_execution_over()) {
      loop_impl();
    }
  });
}

void exec_monitor::join() {
  assert(initialized_);
  thread_.join();
}

#if 0
void exec_monitor::thread_created(const thread& thread) {
  thread_created_impl(thread);
}

void exec_monitor::thread_ended(const thread& thread) {
  thread_ended_impl(thread);
}
#endif

} // namespace aser

