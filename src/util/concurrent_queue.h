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

  /** Pops the value at the front of the queue.
   *
   * If the queue is empty, the caller will block until an element becomes
   * available, or until the timeout expires.
   *
   * @param timeout Timeout to wait before the call fails.
   * @return A pair consisting of a boolean indicating whether the call
   *     succeeded (i.e., an element was found), and the element found (only
   *     valid if the call succeeded).
   */
  template<class Rep, class Period>
  std::pair<bool, value_type> pop(
      const std::chrono::duration<Rep, Period>& timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    bool elem_found = cv_.wait_for(lock, timeout,
        [=] { return !queue_.empty(); });
    std::pair<bool, value_type> result{};
    if (elem_found) {
      result.first = true;
      result.second = std::move(queue_.front());
      queue_.pop();
    }
    return result;
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

} // namespace util
} // namespace aser

#endif // UTIL_CONCURRENT_QUEUE_

