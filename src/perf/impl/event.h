#include <util/log.h>

namespace aser {
namespace perf {

template<typename Impl>
event<Impl>::event(event_info info, Impl impl)
  : info_{std::move(info)}
  , impl_{std::move(impl)}
{
  assert(valid_modifiers(info.modifiers));
}

template<typename Impl>
void event<Impl>::open(pid_t pid, bool attach) {
  impl_.open(info_, pid, attach);
}

template<typename Impl>
void event<Impl>::read() {
  count_ = impl_.read();
}

template<typename Impl>
event_sample event<Impl>::scale(event_read_mode mode) {
  auto value = count_[count_field::RAW_VALUE];
  auto enabled = count_[count_field::TIME_ENABLED];
  auto running = count_[count_field::TIME_RUNNING];

  LOG(boost::format("value: %1%, enabled: %2%, running: %3%")
      % value % enabled % running);

  if (running > enabled)
    throw std::runtime_error("Event ran for longer than it was enabled");

  if (running == 0
      && value != prev_[count_field::RAW_VALUE])
    throw std::runtime_error("Unexpected event value");

  if (mode == event_read_mode::RELATIVE) {
    value -= prev_[count_field::RAW_VALUE];
    enabled -= prev_[count_field::TIME_ENABLED];
    running -= prev_[count_field::TIME_RUNNING];
    prev_ = count_;
  }

  if (running == 0)
    return {0, 0, enabled > 0};

  auto scaling = static_cast<double>(running) / enabled;
  return {value / scaling, scaling, true};
}

template<typename Impl>
bool event<Impl>::valid_modifiers(uint8_t modifiers) {
  return modifiers <= (EXCLUDE_USER | EXCLUDE_KERNEL | EXCLUDE_HV);
}

} // namespace perf
} // namespace aser

