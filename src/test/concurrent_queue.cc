#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <util/concurrent_queue.h>

namespace {

TEST(concurrent_queue, basic) {
  aser::util::concurrent_queue<int> queue;
  constexpr int count = 10;

  for (int i = 0; i < count; i++)
    queue.push(i);

  for (int i = 0; i < count; i++)
    EXPECT_EQ(queue.pop(), i);
}

TEST(concurrent_queue, mixed) {
  aser::util::concurrent_queue<int> queue;
  constexpr int count = 10;

  std::thread t1([&] {
    for (int i = 0; i < count; i++)
      queue.push(i);
  });

  std::thread t2([&] {
    for (int i = 0; i < count; i++)
      EXPECT_EQ(queue.pop(), i);
  });

  t1.join();
  t2.join();
}

} // namespace

