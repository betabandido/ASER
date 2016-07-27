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
 * It allows for one thread to block until the condition becomes ready. Once
 * the condition becomes ready and the thread waiting for the condition
 * unblocks, a thread can block again waiting for the condition to become
 * ready again.
 */
class condition {
public:
  /** Waits for the condition to be ready. */
  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&] { return ready_; });
    ready_ = false;
  }

  /** Marks the condition as ready and notifies the listener. */
  void notify() {
    set_ready();
    cv_.notify_one();
  }

private:
  bool ready_ { false };
  std::condition_variable cv_;
  std::mutex mutex_;

  void set_ready() {
    std::lock_guard<std::mutex> lock(mutex_);
    ready_ = true;
  }
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

