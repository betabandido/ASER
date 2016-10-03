#include <gtest/gtest.h>

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

#include <exec_manager/simple.h>

using aser::simple_manager;
namespace pt = boost::property_tree;

namespace {

#ifdef __linux__

// XXX binding a process to a CPU is only supported in Linux.

TEST(cpu_hopper, basic) {
  // TODO improve this test case by actually checking that processes
  // are being hopped around.
  pt::ptree properties;
  std::istringstream json_properties(
      "{"
      "  \"exec_manager\": {"
      "    \"benchmarks\": ["
      "      {"
      "        \"cmd\": \"/usr/bin/env sleep 5\","
      "        \"name\": \"sleep5a\""
      "      },"
      "      {"
      "        \"cmd\": \"/usr/bin/env sleep 5\","
      "        \"name\": \"sleep5b\""
      "      }"
      "    ]"
      "  },"
      "  \"exec_monitor\": {"
      "    \"type\": \"cpu-hopper\","
      "    \"sampling_length\": 500"
      "  }"
      "}");
  pt::read_json(json_properties, properties);
  simple_manager exec_mgr(properties);
  exec_mgr.start();
}

#endif

} // namespace

