#include "pmc_sampler.h"

#include <perf/event_dummy.h>
#include <perf/event_linux.h>
#include <perf/event_manager.h>
#include <util/factory.h>

namespace pt = boost::property_tree;

using namespace aser::perf;

namespace aser {

#ifdef __linux__

util::registar<
    exec_monitor,
    pmc_sampler<event_manager<event<event_linux_impl>>>,
    exec_manager&, const pt::ptree&>
  pmc_sampler_linux_registar("pmc-sampler[linux]");

#endif

util::registar<
    exec_monitor,
    pmc_sampler<event_manager<event<event_dummy_impl>>>,
    exec_manager&, const pt::ptree&>
  pmc_sampler_dummy_registar("pmc-sampler[dummy]");

} // namespace aser

