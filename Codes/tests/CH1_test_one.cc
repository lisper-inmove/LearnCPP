#include "pch.h"
#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>

namespace cvtest::tester {
TEST_F(Tester, _1_EmptyTest) { EXPECT_EQ(1, 1); }

TEST_F(Tester, TestLoop) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += i;
  }
  std::cout << "Sum is " << sum << "\n";
}
} // namespace cvtest::tester
