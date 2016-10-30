#include "libc_wrapper.h"

#include <cerrno>

namespace aser {
namespace util {

void libc_error(const std::string& msg) {
  libc_error(errno, msg);
}

void libc_error(int error_code, const std::string& msg) {
  throw std::system_error(
      std::error_code(error_code, std::generic_category()),
      msg);
}

} // namespace util
} // namespace aser

