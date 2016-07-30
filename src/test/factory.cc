#include <gtest/gtest.h>

#include <util/factory.h>

using aser::util::create;
using aser::util::registar;

namespace {

class base {
public:
  virtual std::string type() const {
    return "base";
  }
};

class d1 : public base {
public:
  virtual std::string type() const {
    return "d1";
  }
};

class d2 : public base {
public:
  d2(const std::string& s)
    : s_{s}
  {}

  virtual std::string type() const {
    return "d2" + s_;
  }

private:
  std::string s_;
};

TEST(factory, basic) {
  registar<base, d1> d1_registar("d1");
  registar<base, d2, std::string> d2_registar("d2");

  EXPECT_EQ(create<base>("d1")->type(), "d1");
  EXPECT_EQ(create<base>("d2", std::string("arg"))->type(), "d2arg");
}

TEST(factory_death, not_registered) {
  EXPECT_DEATH(create<base>("error"), "Assertion failed");
}

} // namespace

