#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>

namespace aser {
namespace util {

/** Thread-safe boolean condition.
 *
 * It allows for threads to block until the condition becomes ready.
 */
class condition {
public:
  /** Waits for the condition to be ready. */
  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&] { return ready_; });
  }

  /** Marks the condition as ready and notifies the listener. */
  void notify() {
    {
    std::lock_guard<std::mutex> lock(mutex_);
    ready_ = true;
    }
    cv_.notify_all();
  }

private:
  bool ready_ { false };
  std::condition_variable cv_;
  std::mutex mutex_;
};

/** Checks whether a future is ready without blocking.
 *
 * @param f The future.
 * @returns True if the future is ready; false otherwise.
 */
template<typename T>
bool is_ready(const std::future<T>& f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

} // namespace util
} // namespace aser

#endif // UTIL_THREAD_H_

