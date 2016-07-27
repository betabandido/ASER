#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <util/thread.h>

namespace chrono = std::chrono;
using namespace std::literals::chrono_literals;

namespace {

TEST(condition, basic) {
  aser::util::condition cond;

  auto t0 = chrono::system_clock::now();

  std::thread t1([&] {
      std::this_thread::sleep_for(1s);
      cond.notify();
  });

  std::thread t2([&] {
      cond.wait();
      auto t = chrono::system_clock::now();
      EXPECT_GT(t - t0, 1s);
      EXPECT_LT(t - t0, 1200ms);
  });

  t1.join();
  t2.join();
}

TEST(is_ready, basic) {
  auto f = std::async(
      std::launch::async,
      [] {
        std::this_thread::sleep_for(1s);
        return true;
      });

  EXPECT_FALSE(aser::util::is_ready(f));
        
  std::this_thread::sleep_for(1200ms);
  EXPECT_TRUE(aser::util::is_ready(f));
  EXPECT_TRUE(f.get());
}

} // namespace

