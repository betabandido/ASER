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
      "{ \"exec_monitor\": {"
          "\"type\": \"simple\","
          "\"sampling_length\": 500"
      "}}");
  pt::read_json(json_properties, properties);

  std::vector<simple_manager::benchmark> benchs = {
    {0, {"/usr/bin/env", "sleep", "5"}},
    {1, {"/usr/bin/env", "sleep", "2"}}
  };
  simple_manager exec_mgr(properties, benchs);
  exec_mgr.start();
}

} // namespace

