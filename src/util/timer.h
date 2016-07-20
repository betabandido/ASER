#ifndef UTIL_TIMER_H_
#define UTIL_TIMER_H_

#include <functional>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace aser {
namespace util {

class callback_timer {
public:
  typedef std::function<void()> callback_func;

  callback_timer(
      std::chrono::milliseconds duration,
      callback_func callback);

  ~callback_timer();

private:
  std::chrono::milliseconds duration_;
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

