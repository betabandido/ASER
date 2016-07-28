#include <gtest/gtest.h>

#include <exec_manager/simple.h>

using aser::simple_manager;

namespace {

TEST(simple_manager, basic) {
  std::vector<simple_manager::benchmark> benchs = {
    {0, {"/usr/bin/env", "sleep", "5"}},
    {1, {"/usr/bin/env", "sleep", "2"}}
  };
  simple_manager exec_mgr(benchs);
  exec_mgr.start();
}

} // namespace

