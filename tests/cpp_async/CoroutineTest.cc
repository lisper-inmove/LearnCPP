#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
using namespace std::chrono_literals;

/**
 * Promise And Future
 * */

namespace cvtest::tester {
TEST_F(Tester, CouroutineTester) { std::cout << "协程 " << "\n"; }
} // namespace cvtest::tester
