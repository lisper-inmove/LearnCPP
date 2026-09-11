/**
 *
 * Move Semantics
 *
 * */
#include "tester.h"
namespace cvtest::tester {

class A {};

TEST_F(Tester, MS_ValueCategory) {
  A a{};

  std::move(a) = A{};
}

} // namespace cvtest::tester
