#include <gtest/gtest.h>

#include <perf/perf.h>
#include <util/process.h>

// TODO remove
#include <iostream>

using namespace aser::perf;

namespace {

#ifdef __linux__

TEST(event_manager, simple) {
  aser::util::process p("/usr/bin/env", "false");
  p.prepare();

  std::vector<event_info> events = {
    {event_type::HARDWARE,
      generic_events::cycles,
      event_modifiers::EXCLUDE_NONE},
    {event_type::HARDWARE,
      generic_events::instructions,
      event_modifiers::EXCLUDE_NONE}
  };
  event_manager mgr(events, p.pid(), false);
  p.start();
  p.wait();

  auto& samples = mgr.read_events(event::read_mode::AGGREGATED);
  for (auto& s : samples) {
    std::cout << s.enabled << ", " << s.value << ", " << s.scaling << "\n";
    EXPECT_TRUE(s.enabled);
    EXPECT_GT(s.value, 0);
    EXPECT_DOUBLE_EQ(s.scaling, 1);
  }
}

#endif

} // namespace

