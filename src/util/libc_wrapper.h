#ifndef UTIL_LIBC_WRAPPER_H
#define UTIL_LIBC_WRAPPER_H

#include <string>

#include <boost/format.hpp>

namespace util {

void libc_error(const std::string& msg);

/** Throws an error if result is equal to error_result.
 *
 * @param result The result from a libc call.
 * @param error_result Expected value for an error.
 * @param error_msg Error message.
 * @return The result from the libc call.
 *         If there is an error, an exception is thrown instead.
 */
template<typename T>
T error_if_equal(T result, T error_result, const std::string& error_msg) {
  if (result == error_result)
    libc_error(error_msg);
  return result;
}

/** Throws an error if result is not equal to success_result.
 *
 * @param result The result from a libc call.
 * @param success_result Expected value for success.
 * @param error_msg Error message.
 * @return The result from the libc call.
 *         If there is an error, an exception is thrown instead.
 */
template<typename T>
T error_if_not_equal(T result, T success_result, const std::string& error_msg) {
  if (result != success_result)
    libc_error(error_msg);
  return result;
}

} // namespace util

#endif // UTIL_LIBC_WRAPPER_H

