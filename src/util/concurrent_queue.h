#ifndef UTIL_CONCURRENT_QUEUE_
#define UTIL_CONCURRENT_QUEUE_

#include <condition_variable>
#include <mutex>
#include <queue>

namespace aser {
namespace util {

/** A concurrent queue that can be used in a producer-consumer paradigm. */
template<typename T>
class concurrent_queue {
public:
  typedef typename std::queue<T>::value_type value_type;

  /** Pushes a value into the queue.
   *
   * @param value The value to push.
   */
  void push(const value_type& value) {
    {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    }
    cv_.notify_one();
  }

  /** Pushes a value into the queue.
   *
   * @param value The value to push.
   */
  void push(value_type&& value) {
    {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    }
    cv_.notify_one();
  }

  /** Pops the value at the front of the queue.
   *
   * If the queue is empty, the caller will block.
   *
   * @return The value at the front of the queue.
   */
  value_type pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [=] { return !queue_.empty(); });
    auto value = std::move(queue_.front());
    queue_.pop();
    return value;
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

} // namespace util
} // namespace aser

#endif // UTIL_CONCURRENT_QUEUE_

