#include "exec_monitor.h"

#include <cassert>

#include <core/exec_manager.h>
#include <util/log.h>

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

void exec_monitor::prepare() {
  assert(initialized_);
  prepare_impl();
}

void exec_monitor::start() {
  assert(initialized_);
  thread_ = std::thread([&] {
    while (!exec_manager_.is_execution_over()) {
      loop_impl();
      process_events();
    }
  });
}

void exec_monitor::process_events() {
  while (true) {
    auto res = event_queue_.pop(std::chrono::seconds(0));
    if (!res.first)
      break;

    event_handler(res.second);
  }
}

void exec_monitor::event_handler(const exec_event& event) {
  LOG(boost::format("Processing event (type: %1%, pid: %2%)")
      % static_cast<int>(event.type)
      % event.pid);
  auto it = event_handlers_.find(event.type);
  if (it == end(event_handlers_)) {
    LOG("No handler registered");
    return;
  }

  auto& func = it->second;
  func(event);
}

void exec_monitor::join() {
  assert(initialized_);
  thread_.join();
}

void exec_monitor::enqueue_event(exec_event event) {
  event_queue_.push(std::move(event));
}

void exec_monitor::register_event_handler(
    const exec_event::event_type& type,
    const event_handler_func& func) {
  auto res = event_handlers_.emplace(type, func);
  assert(res.second);
}

} // namespace aser

