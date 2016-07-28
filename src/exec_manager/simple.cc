#include "simple.h"

#include <core/process_monitor.h>
#include <util/log.h>
#include <util/process.h>

namespace aser {

simple_manager::simple_manager(const std::vector<benchmark>& benchs)
  : exec_manager()
  , benchs_(benchs)
  , processes_(benchs.size())
{}

void simple_manager::start_impl() {
  LOG("Starting execution");

  for (auto& b : benchs_)
    prepare_bench(b);

  process_monitor monitor;
  for (auto& p : processes_)
    monitor.add_process(p);

  for (auto& p : processes_)
    p->start();

  monitor.wait_for_any();
  execution_end();

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
  processes_[bench.id] = std::move(process);
}

} // namespace aser

