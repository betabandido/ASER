#ifndef UTIL_OS_H_
#define UTIL_OS_H_

namespace aser {
namespace util {

void pipe2(int fd[2], int flags);

} // namespace util
} // namespace aser

#endif // UTIL_OS_H_

