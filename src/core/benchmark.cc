#include "benchmark.h"

#include <boost/algorithm/string.hpp>

namespace pt = boost::property_tree;

namespace aser {

static std::vector<std::string> parse_cmd(const std::string& cmd) {
  std::vector<std::string> args;
  boost::split(args, cmd, boost::is_any_of(" "));
  return args;
}

namespace util {

benchmark parse(const pt::ptree& pt) {
  return {
    0,
    pt.get<std::string>("name"),
    parse_cmd(pt.get<std::string>("cmd"))
  };
}

} // namespace util

} // namespace aser

