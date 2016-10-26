#include <array>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <perf/event.h>
#include <perf/event_manager.h>
//#include <util/process.h>

using namespace aser::perf;

namespace {

class mock_event_impl {
public:
  using count_type = std::array<uint64_t, 3>;

  MOCK_METHOD3(open, void(const event_info&, pid_t, bool));
  MOCK_METHOD0(close, void());
  MOCK_METHOD0(read, count_type());
};

TEST(event_death_test, invalid_modifiers) {
  event_info info =
      {event_type::HARDWARE, 0, std::numeric_limits<uint8_t>::max()};
  mock_event_impl impl;
  EXPECT_DEATH({ event<mock_event_impl> e(info, std::move(impl)); }, "");
}

TEST(event_test, running_is_larger) {
  using testing::Return;
  using testing::_;

  event_info info = {event_type::HARDWARE, 0, event_modifiers::EXCLUDE_NONE};
  pid_t pid = 1000;
  std::array<uint64_t, 3> count = {2000, 1000, 1500};

  mock_event_impl impl;
  EXPECT_CALL(impl, open(_, pid, true))
    .Times(1);
  EXPECT_CALL(impl, read())
    .WillOnce(Return(count));

  event<mock_event_impl> event{info, std::move(impl)};
  event.open(pid, true);
  event.read();
  EXPECT_THROW(
      event.scale(event_read_mode::AGGREGATED),
      std::runtime_error
  );
}

TEST(event_test, aggregated) {
  using testing::InSequence;
  using testing::Return;
  using testing::_;

  event_info info = {event_type::HARDWARE, 0, event_modifiers::EXCLUDE_NONE};
  pid_t pid = 1000;
  std::array<uint64_t, 3> count = {2000, 1000, 1000};
  constexpr unsigned iterations = 5;

  mock_event_impl impl;
  EXPECT_CALL(impl, open(_, pid, true))
    .Times(1);
  {
    InSequence s;
    auto aggregated_count = count;
    for (int i = 1; i <= iterations; ++i) {
      EXPECT_CALL(impl, read())
        .WillOnce(Return(aggregated_count));
      aggregated_count[0] += count[0];
      aggregated_count[1] += count[1];
      aggregated_count[2] += count[2];
    }
  }

  event<mock_event_impl> event{info, std::move(impl)};
  event.open(pid, true);

  for (unsigned i = 1; i <= iterations; ++i) {
    event.read();
    auto sample = event.scale(event_read_mode::AGGREGATED);
    EXPECT_DOUBLE_EQ(sample.value, i * count[0]);
    EXPECT_DOUBLE_EQ(sample.scaling, count[1] / count[2]);
    EXPECT_TRUE(sample.enabled);
  }
}

TEST(event_test, relative) {
  using testing::InSequence;
  using testing::Return;
  using testing::_;

  event_info info = {event_type::HARDWARE, 0, event_modifiers::EXCLUDE_NONE};
  pid_t pid = 1000;
  std::array<uint64_t, 3> count = {2000, 1000, 1000};
  constexpr int iterations = 5;

  mock_event_impl impl;
  EXPECT_CALL(impl, open(_, pid, true))
    .Times(1);
  {
    InSequence s;
    auto aggregated_count = count;
    for (int i = 1; i <= iterations; ++i) {
      EXPECT_CALL(impl, read())
        .WillOnce(Return(aggregated_count));
      aggregated_count[0] += count[0];
      aggregated_count[1] += count[1];
      aggregated_count[2] += count[2];
    }
  }

  event<mock_event_impl> event{info, std::move(impl)};
  event.open(pid, true);

  for (int i = 1; i <= iterations; ++i) {
    event.read();
    auto sample = event.scale(event_read_mode::RELATIVE);
    EXPECT_DOUBLE_EQ(sample.value, count[0]);
    EXPECT_DOUBLE_EQ(sample.scaling, count[1] / count[2]);
    EXPECT_TRUE(sample.enabled);
  }
}

class mock_event {
public:
  mock_event(event_info info) {}

  MOCK_METHOD2(open, void(pid_t, bool));
  MOCK_METHOD0(read, void());
  MOCK_METHOD1(scale, event_sample(event_read_mode));
};

TEST(event_manager_test, basic) {
  using testing::Return;

  constexpr event_info cycles =
      {event_type::HARDWARE, 0, event_modifiers::EXCLUDE_NONE};
  constexpr event_info instrs =
      {event_type::HARDWARE, 1, event_modifiers::EXCLUDE_NONE};

  std::vector<event_info> events = {cycles, instrs};
  pid_t pid = 1000;
  bool attach = true;
  auto read_mode = event_read_mode::AGGREGATED;

  auto event_factory = [&](const event_info& info) {
    mock_event mock{info};
    EXPECT_CALL(mock, open(pid, attach))
      .Times(1);
    EXPECT_CALL(mock, read())
      .Times(1);
    EXPECT_CALL(mock, scale(read_mode))
      .Times(1)
      .WillOnce(Return(event_sample{1.0, 1.0, true}));
    return mock;
  };

  event_manager<mock_event> manager(events, pid, attach, event_factory);

  auto samples = manager.read_events(read_mode);
  for (auto& s : samples) {
    EXPECT_DOUBLE_EQ(s.value, 1.0);
    EXPECT_DOUBLE_EQ(s.scaling, 1.0);
    EXPECT_TRUE(s.enabled);
  }
}

#if 0

#ifdef __linux__

TEST(event_manager, simple) {
  aser::util::process p("/usr/bin/env", "false");
  p.prepare();

  std::vector<event_info> events = {
    {event_type::HARDWARE,
      generic_events::cycles,
      event_modifiers::EXCLUDE_NONE},
    {event_type::HARDWARE,
      generic_events::instructions,
      event_modifiers::EXCLUDE_NONE}
  };
  event_manager mgr(events, p.pid(), false);
  p.start();
  p.wait();

  auto& samples = mgr.read_events(event::read_mode::AGGREGATED);
  for (auto& s : samples) {
    std::cout << s.enabled << ", " << s.value << ", " << s.scaling << "\n";
    EXPECT_TRUE(s.enabled);
    EXPECT_GT(s.value, 0);
    EXPECT_DOUBLE_EQ(s.scaling, 1);
  }
}

#endif

#endif

} // namespace

