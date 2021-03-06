#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <core/process_monitor.h>
#include <util/process.h>

using aser::process_monitor;
using namespace std::literals::chrono_literals;
namespace chrono = std::chrono;

namespace {

TEST(process_monitor, process_cannot_be_added_twice) {
  using process = aser::util::sync_process;
  process_monitor<process> pm;
  auto p1 = std::make_shared<process>("/usr/bin/env", "true");
  p1->prepare();
  pm.add_process(p1);
  EXPECT_THROW(pm.add_process(p1), std::invalid_argument);
  p1->start();
}

TEST(process_monitor, basic) {
  using process = aser::util::sync_process;
  process_monitor<process> pm;
  auto p1 = std::make_shared<process>("/usr/bin/env", "sleep", "1");
  auto p2 = std::make_shared<process>("/usr/bin/env", "sleep", "2");
  p1->prepare();
  p2->prepare();
  pm.add_process(p1);
  pm.add_process(p2);
  p1->start();
  p2->start();

  auto t0 = chrono::system_clock::now();
  auto res = pm.wait_for_any();
  auto t = chrono::system_clock::now();
  EXPECT_EQ(res.first, p1->pid());
  EXPECT_GT(t - t0, 1s);
  EXPECT_LT(t - t0, 1200ms);

  res = pm.wait_for_any();
  t = chrono::system_clock::now();
  EXPECT_EQ(res.first, p2->pid());
  EXPECT_GT(t - t0, 2s);
  EXPECT_LT(t - t0, 2200ms);
}

} // namespace

