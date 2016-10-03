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

TEST(concurrent_queue, timeout) {
  aser::util::concurrent_queue<int> queue;

  auto res = queue.pop(std::chrono::seconds(1));
  EXPECT_FALSE(res.first);

  queue.push(1234);
  res = queue.pop(std::chrono::seconds(1));
  EXPECT_TRUE(res.first);
  EXPECT_EQ(res.second, 1234);
}

} // namespace

