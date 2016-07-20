#ifndef UTIL_TIMER_H_
#define UTIL_TIMER_H_

#include <functional>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace aser {
namespace util {

/** This class implements a timer that calls a function once the timer expires.
 *
 * If the timer object is destroyed, the timer is canceled and the callback
 * function will not be called.
 *
 * The timer uses a thread that blocks until either the timer expires or is
 * canceled.
 */
class callback_timer {
public:
  typedef std::function<void()> callback_func;

  /** Constructor.
   *
   * @param duration Timer duration (in milliseconds).
   * @param callback Callback function to call once the timer expires.
   */
  callback_timer(
      std::chrono::milliseconds duration,
      callback_func callback);

  /** Destructor. */
  ~callback_timer();

private:
  /** Timer duration. */
  std::chrono::milliseconds duration_;

  /** Callback function to call. */
  callback_func callback_;

  bool canceled_ { false };
  std::mutex mutex_;
  std::condition_variable cond_;
  std::thread thread_;

  void run();
};

} // namespace util
} // namespace aser

#endif // UTIL_TIMER_H_

