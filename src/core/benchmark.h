#ifndef CORE_BENCHMARK_H_
#define CORE_BENCHMARK_H_

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

namespace aser {

struct benchmark {
  unsigned id;
  std::string name;
  std::vector<std::string> args;
};

namespace util {

benchmark parse(const boost::property_tree::ptree& pt);

} // namespace util

} // namespace aser

#endif // CORE_BENCHMARK_H_

