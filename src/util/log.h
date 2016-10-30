#ifndef UTIL_LOG_H_
#define UTIL_LOG_H_

#define BOOST_LOG_DYN_LINK 1

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define LOG(x) BOOST_LOG_TRIVIAL(debug) << x
#define LOGI(x) BOOST_LOG_TRIVIAL(info) << x

#endif // UTIL_LOG_H_

