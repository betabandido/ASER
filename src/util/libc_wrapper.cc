#include "libc_wrapper.h"

#include <string.h>

namespace util {

void libc_error(const std::string& msg) {
  throw std::runtime_error(boost::str(boost::format("%1% (%2%)")
      % msg
      % strerror(errno)
      ));
}

} // namespace util

