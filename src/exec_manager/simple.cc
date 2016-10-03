#include "simple.h"

#include <core/benchmark.h>
#include <core/process_monitor.h>
#include <util/factory.h>
#include <util/log.h>
#include <util/parse.h>
#include <util/process.h>

namespace pt = boost::property_tree;

namespace aser {

util::registar<exec_manager, simple_manager, const pt::ptree&>
  simple_manager_registar("simple");

simple_manager::simple_manager(const pt::ptree& properties)
  : exec_manager(properties)
  , benchs_(util::parse_vector<benchmark>(
        properties.get_child("exec_manager.benchmarks")))
  , processes_(benchs_.size())
{
  unsigned id = 0;
  for (auto& b : benchs_)
    b.id = id++;
}

void simple_manager::start_impl() {
  LOG("Starting execution");

  for (auto& b : benchs_)
    prepare_bench(b);

  process_monitor monitor;
  for (auto& p : processes_)
    monitor.add_process(p);

  prepare_exec_monitor(); 

  for (auto& p : processes_)
    p->start();

  start_exec_monitor();
  monitor.wait_for_any();
  execution_end();
  stop_exec_monitor();

  LOG("Cleaning up");

  for (auto& p : processes_) {
    try {
      p->kill();
    } catch (const std::exception& e) {
      LOG(e.what());
    } catch (...) {
      LOG("Unexpected error trying to kill a process");
    }
  }
}

void simple_manager::prepare_bench(const benchmark& bench) {
  LOG(boost::format("Preparing bench %1%") % bench.id);
  auto process = std::make_shared<util::process>(bench.args);
  process->prepare();
  notify_process_creation(process->pid());
  processes_[bench.id] = std::move(process);
}

} // namespace aser

