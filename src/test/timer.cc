#include <gtest/gtest.h>

#include <util/timer.h>

using aser::util::callback_timer;
namespace chrono = std::chrono;

namespace {

template<class T>
void simple_test(T duration) {
  bool callback_called = false;
  auto t0 = chrono::system_clock::now();
  callback_timer t(
      chrono::duration_cast<chrono::milliseconds>(duration),
      [&] { 
        auto t = chrono::system_clock::now();
        callback_called = true;
        EXPECT_GE(t - t0, duration);
        EXPECT_LT(t - t0, duration * 1.1);
      });
  std::this_thread::sleep_for(duration * 1.5);
  EXPECT_TRUE(callback_called);
}

TEST(callback_timer, simple) {
  for (int i = 0; i < 5; i++)
    simple_test(chrono::milliseconds(100));
}

TEST(callback_timer, cancelation) {
  bool callback_called = false;
  {
    callback_timer t(
        chrono::milliseconds(100),
        [&] { callback_called = true; });
  }
  std::this_thread::sleep_for(chrono::milliseconds(200));
  EXPECT_FALSE(callback_called);
}

} // namespace

