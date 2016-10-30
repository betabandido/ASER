#include <gtest/gtest.h>

#define BOOST_LOG_DYN_LINK 1

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

int main(int argc, char** argv) {
  namespace logging = boost::log;
  logging::core::get()->set_filter(
      logging::trivial::severity >= logging::trivial::info
  );

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

