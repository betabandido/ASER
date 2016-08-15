#ifndef UTIL_PARSE_H_
#define UTIL_PARSE_H_

#include <algorithm>
#include <vector>

#include <boost/property_tree/ptree.hpp>

namespace aser {
namespace util {

template<typename T>
std::vector<T> parse_vector(const boost::property_tree::ptree& pt) {
  std::vector<T> v;
  std::transform(begin(pt), end(pt), std::back_inserter(v),
      [](auto& elem) { return parse(elem.second); });
  return v;
}

} // namespace util
} // namespace aser

#endif // UTIL_PARSE_H_

