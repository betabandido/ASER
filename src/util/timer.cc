#include "timer.h"

namespace aser {
namespace util {

callback_timer::callback_timer(
    std::chrono::milliseconds duration,
    callback_func callback)
  : duration_{duration}
  , callback_{callback}
  , thread_{&callback_timer::run, this}
{
}

callback_timer::~callback_timer() {
  try {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      canceled_ = true;
    }
    cond_.notify_one();
    thread_.join();
  } catch (...) {
    std::terminate();
  }
}

void callback_timer::run() {
  std::unique_lock<std::mutex> lock(mutex_);
  if (!cond_.wait_for(lock, duration_, [&] { return canceled_; }))
    callback_();
}

} // namespace util
} // namespace aser

