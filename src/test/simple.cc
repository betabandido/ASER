#include <gtest/gtest.h>

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

#include <exec_manager/simple.h>

using aser::simple_manager;
namespace pt = boost::property_tree;

namespace {

TEST(simple_manager, basic) {
  pt::ptree properties;
  std::istringstream json_properties(
      "{"
      "  \"exec_manager\": {"
      "    \"benchmarks\": ["
      "      {"
      "        \"cmd\": \"/usr/bin/env sleep 5\","
      "        \"name\": \"sleep5\""
      "      },"
      "      {"
      "        \"cmd\": \"/usr/bin/env sleep 2\","
      "        \"name\": \"sleep2\""
      "      }"
      "    ]"
      "  },"
      "  \"exec_monitor\": {"
      "    \"type\": \"simple\","
      "    \"sampling_length\": 500"
      "  }"
      "}");
  pt::read_json(json_properties, properties);
  simple_manager exec_mgr(properties);
  exec_mgr.start();
}

} // namespace

