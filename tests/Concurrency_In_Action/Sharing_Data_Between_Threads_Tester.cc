/**
 *
 * 第三章: 锁的使用
 *
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <list>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#define sync_cout std::osyncstream(std::cout)
using namespace std::chrono_literals;

namespace cvtest::tester {

class SomeBigObject {};
void swap(SomeBigObject &lhs, SomeBigObject &rhs);
void swap2(SomeBigObject &lhs, SomeBigObject &rhs);
class X {
private:
  SomeBigObject someDetail;
  std::mutex m;

public:
  X(SomeBigObject const &sd) : someDetail(sd) {}
  friend void swap2(X &lhs, X &rhs) {
    /**
     * C++ 17 之后提供了 RAII template, std::scoped_lock<>
     * */
    if (&lhs == &rhs) {
      return;
    }
    // std::scoped_lock guard(lhs.m, rhs.m); // 自动推断
    std::scoped_lock<std::mutex, std::mutex> guard(lhs.m, rhs.m);
    swap(lhs.someDetail, rhs.someDetail);
  }
  friend void swap(X &lhs, X &rhs) {
    if (&lhs == &rhs) {
      return;
    }
    // 必须手动调用 std::lock 确保不死锁
    std::lock(lhs.m, rhs.m);
    // 然后采用 std::adopt_lock 策略，告诉 lock_guard
    // 锁已经加好了，只需负责析构解锁
    std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
    swap(lhs.someDetail, rhs.someDetail);
  }
};

} // namespace cvtest::tester

/**
 * 异步编程
 * */

namespace cvtest::tester {
TEST_F(Tester, CIA_Concurrency_In_Action_CH3_Tester) {
  std::cout << "C++ 异步编程" << "\n";

  std::cout << "总核心数 " << std::thread::hardware_concurrency() << "\n";
}

/**
 * 2.5 Identifying Threads
 * */
TEST_F(Tester, CIA_IdentifyThreadTester) {
  std::thread::id myThreadId = std::this_thread::get_id();
  std::cout << "My thread id is " << myThreadId << "\n";
}

/**
 * lock_guard
 * */
TEST_F(Tester, CIA_LockGuardTester) {
  std::list<int> someList;
  std::mutex someMutex;
  auto addToList = [&](int newValue) {
    std::lock_guard<std::mutex> guard(someMutex);
    someList.push_back(newValue);
  };
  auto listContains = [&](int valueToFind) {
    std::lock_guard<std::mutex> guard(someMutex);
    return std::find(someList.begin(), someList.end(), valueToFind) != someList.end();
  };
}

/**
 * 使用 lock 解决死锁问题
 * 1. 一种比较简单的解决办法是，总是按指定的顺序获取锁
 * 2. 用std::lock(C++17之后用 std::scoped_lock)来获取锁
 * 3. 使用层级锁
 *
 * 避免死锁
 * 1. 尽量只使用一个锁
 * 2. 被锁保护的函数不要调用用户提供的函数
 * 3. 总是固定的顺序获取锁
 * */
TEST_F(Tester, CIA_SolveDeadlockTester) {}

/**
 * std::once_flag & std::call_one
 * */
TEST_F(Tester, CIA_DoOnceTester) {
  std::once_flag runOnceFlag;
  auto threadFunc = [&] { std::call_once(runOnceFlag, [] { std::cout << "Called only once\n"; }); };
  std::thread t1(threadFunc);
  std::thread t2(threadFunc);
  std::thread t3(threadFunc);
  t1.join();
  t2.join();
  t3.join();
}

/**
 * std::shared_mutex
 * std::shared_timed_mutex
 * */

/**
 * std::shared_lock
 * std::unique_lock
 * */

/**
 * std::recursive_mutex
 * */

class HierarchicalMutex {
private:
  std::mutex mtx;
  unsigned long hierarchyValue_;
  unsigned long previousHierarchyValue_;
  void check_for_hierarchy_violation() {
    if (thisThreadHierarchyValue_ <= hierarchyValue_) {
      throw std::logic_error("Mutex hierarchy violated");
    }
  }

  void update_hierarchy_value() {
    previousHierarchyValue_ = thisThreadHierarchyValue_;
    thisThreadHierarchyValue_ = hierarchyValue_;
  }

public:
  static thread_local unsigned long thisThreadHierarchyValue_;
  explicit HierarchicalMutex(unsigned long value) : hierarchyValue_(value), previousHierarchyValue_(0) {}

  void lock() {
    check_for_hierarchy_violation();
    mtx.lock();
    update_hierarchy_value();
  }

  void unlock() {
    if (thisThreadHierarchyValue_ != hierarchyValue_) {
      throw std::logic_error("Mutex hierarchy violated");
    }
    thisThreadHierarchyValue_ = previousHierarchyValue_;
    mtx.unlock();
  }
  bool try_lock() {
    check_for_hierarchy_violation();
    if (!mtx.try_lock()) {
      return false;
    }
    update_hierarchy_value();
    return true;
  }
};

thread_local unsigned long HierarchicalMutex::thisThreadHierarchyValue_(ULONG_MAX);

TEST_F(Tester, CIA_HierarchicalMutexTester) {
  HierarchicalMutex highLevelMutex(10000);
  HierarchicalMutex lowLevelMutex(5000);
  HierarchicalMutex otherMutex(6000);

  auto do_low_level_stuff = []() { std::cout << "low level stuff \n"; };
  auto do_high_level_stuff = []() { std::cout << "high level stuff \n"; };
  auto other_level_stuff = [&do_high_level_stuff]() {
    do_high_level_stuff();
    std::cout << "do other level stuff\n";
  };

  auto low_level_func = [&]() {
    std::lock_guard<HierarchicalMutex> lk(lowLevelMutex);
    return do_low_level_stuff();
  };

  auto high_level_func = [&]() {
    std::lock_guard<HierarchicalMutex> lk(highLevelMutex);
    do_high_level_stuff();
  };

  auto other_func = [&]() {
    std::lock_guard<HierarchicalMutex> lk(otherMutex);
    other_level_stuff();
  };

  auto thread_a = [&]() {
    std::lock_guard<HierarchicalMutex> lk(highLevelMutex);
    low_level_func();
  };

  auto thread_b = [&]() {
    std::lock_guard<HierarchicalMutex> lk(otherMutex);
    high_level_func();
  };

  std::thread t1(thread_a);
  std::thread t2(thread_b);
  t1.join();
  t2.join();
}

std::mutex mtx001;

std::unique_lock<std::mutex> get_lock() {
  extern std::mutex mtx001;
  std::unique_lock<std::mutex> lk(mtx001);
  std::cout << "Do something to prepare data...\n";
  return lk;
}

void process_data() {
  std::cout << "Before get lock process data...\n";
  std::unique_lock<std::mutex> lk(get_lock());
  std::cout << "After get lock process data...\n";
  std::this_thread::sleep_for(3s);
}

TEST_F(Tester, CIA_UniqueLockTester) {
  std::thread t1(process_data);
  std::thread t2(process_data);
  t1.join();
  t2.join();
}

} // namespace cvtest::tester
//
