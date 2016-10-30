#include <gtest/gtest.h>

#include <cstring>

#include <util/os.h>

namespace {

TEST(pipe2_test, basic) {
  using aser::util::pipe2;

  int fd[2];
  pipe2(fd, 0);

  std::string write_data = "foobar";
  ::write(fd[1], write_data.c_str(), write_data.size());
  
  char read_data[write_data.size() + 1];
  memset(read_data, 0, write_data.size() + 1);
  ::read(fd[0], read_data, write_data.size());

  EXPECT_EQ(write_data, std::string(read_data));
}

} // namespace

