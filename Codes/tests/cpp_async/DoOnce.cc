
#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <mutex>
#include <thread>
using namespace std::chrono_literals;

/**
 * 只执行一次的任务
 * std::once_flag
 * std::call_once
 * */

namespace cvtest::tester {
TEST_F(Tester, DoOnceTester) {
  std::once_flag runOnceFlag;
  auto threadFunc = [&] {
    std::call_once(runOnceFlag, [] { std::cout << "Called only once\n"; });
  };
  std::thread t1(threadFunc);
  std::thread t2(threadFunc);
  std::thread t3(threadFunc);
  t1.join();
  t2.join();
  t3.join();
}
} // namespace cvtest::tester
