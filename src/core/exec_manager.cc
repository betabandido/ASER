#include "exec_manager.h"

#include <core/exec_monitor.h>
#include <util/factory.h>

namespace pt = boost::property_tree;

namespace aser {

exec_manager::exec_manager(const pt::ptree& properties)
  : exec_monitor_{util::create<exec_monitor>(
      properties.get<std::string>("exec_monitor.type"),
      *this,
      static_cast<const pt::ptree&>(properties))}
{}

exec_manager::~exec_manager() = default;

void exec_manager::start() {
  exec_monitor_->initialize();
  start_impl();
}

void exec_manager::prepare_exec_monitor() {
  exec_monitor_->prepare();
}

void exec_manager::start_exec_monitor() {
  exec_monitor_->start();
}

void exec_manager::stop_exec_monitor() {
  exec_monitor_->join();
  exec_monitor_->finalize();
}

} // namespace aser

