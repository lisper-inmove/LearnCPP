/**
 *
 * 第二章: 管理线程
 *
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <cassert>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

namespace cvtest::tester {
TEST_F(Tester, CIA_Concurrency_In_Action_CH2_Tester) {
  std::cout << "C++ 异步编程 - CH2" << "\n";

  std::cout << "总核心数 " << std::thread::hardware_concurrency() << "\n";
}

TEST_F(Tester, CIA_StartThread1) {
  auto do_some_task = []() { std::cout << "从一个普通函数启动线程\n"; };
  std::thread t1(do_some_task);
  t1.join();
}

class ThreadClass {
public:
  ThreadClass() {}
  ThreadClass(const ThreadClass &other) { std::cout << "拷贝构造函数\n"; }
  void operator()() const { std::cout << "可执行对象启动线程\n"; }
};

/**
 * 函数对象会被复制到执行线程所属的存储空间中
 * */
TEST_F(Tester, CIA_StartThread2) {
  ThreadClass tcls;
  std::thread t1(tcls);
  std::thread t2(ThreadClass{});
  std::thread t3{ThreadClass()};
  std::thread t4((ThreadClass()));
  t1.join();
  t2.join();
  t3.join();
  t4.join();
}

/**
 * 用lambda函数
 * */
TEST_F(Tester, CIA_StartThread3) {
  std::thread t1([] { std::cout << "Lambda 函数启动线程\n"; });
  t1.join();
}

/**
 *
 * */
TEST_F(Tester, CIA_ThreadTerminate) {
  std::thread t1([] { std::cout << "Lambda 函数启动线程\n"; });
  std::this_thread::sleep_for(5s);
  std::cout << "Main end\n";
}

} // namespace cvtest::tester
