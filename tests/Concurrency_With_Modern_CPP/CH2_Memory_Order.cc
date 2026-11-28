/**
 * 内存序
 * 现代CPU会做大量优化，写的代码顺序和实际执行的代码顺序可能并不相同。
 * 内存序的作用就是告诉编译器/CPU 哪些是可以重排的，哪些是禁止的
 *
 * 1. sequence-before: 单线内，语句A在B之前，那么它一定在B之前执行
 *  （这里的A和B可能不是单行代码，sequence-before保证的是结果一致，代码的执行顺序是有可能重排的）
 * 2. synchronizes-with: 线程间，通常是 release -> acquire 同一原子变量
 * 3. happens-before: 如果 A sequence-before B, B synchronizes-with C，那么 A happens-before C
 * 4. release-sequence: 释放序列。它允许release的同步语义通过一系列的'原子 读-改-写' 操作，
 *  像接力一样传递给后续的acquire操作。
 *  release -> 任意个RMW -> acquire，release-acquire也有happens-before的关系
 *
 * 1. memory_order_relaxed: 最弱，只保证原子性和单变量修改顺序一致性，不提供任何跨线程同步
 * 2. memory_order_acquire, memory_order_release:
 *  2.1 release(写)，保证该store之前的所有读写不会被重排到store之后
 *  2.2 acquire(读)，保证该load之后的所有读不会被重排到load之前
 *    thread1:
 *      a = 42 (1)
 *      ready.store(true, std::memory_order_release) (2)
 *    thread2:
 *      while(!ready.load(std::memory_order_acquire)); (3)
 *      assert(a == 42) // 一定成立 (4)
 *    (1)与(2), (3)与(4) 是 sequence-before
 *    (2)与(3)是synchronizes-with
 *    从而(1)与(4)形成 happens-before
 * 3. memory_order_acq_rel: 读-改-写。当一个操作既要消费别人发布的数据，又要发布数据给别人时
 *
 *  std::atomic<int> stage_(0);
 *  int shared_data = 0;
 *  void workerA() {
 *
 *  }
 * */
#include "tester.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

namespace cvtest::tester {
TEST_F(Tester, CWMC_Memory_Order_Relaxed) {
  std::string work = "Working";
  std::atomic<bool> ready(false);
  std::thread t1([&work, &ready]() {
    work = "Done";
    ready.store(true, std::memory_order_relaxed);
    // 依据 sequence-before, work = Done 一定在 ready = true前面执行，
    // 但是在其它线程里，可能会看到ready=true, work = "Working"
  });
  std::thread t2([&work, &ready]() {
    // relaxed并没有synchronizes-with
    while (!ready.load(std::memory_order_relaxed)) {
    }
    // work 在某些平台，某些条件下有可能为 working
    std::cout << "Work = " << work << "\n";
  });
  t1.join();
  t2.join();
}

/**
 * Acquire-Release 一个Release操作同步于一个Acquire操作，它们之间建立起一个顺序约束，这个顺序是单向的，一定是 relesae -> acquire
 * 其中 Release 保证了它之前的读与写一定不会在它之后执行（Release之后的操作有可能在Release之前执行）
 *      Acquire 它之后的读与写一定不会在它之前执行（Acquire之前的有可能在Acquire之后执行）
 * */
TEST_F(Tester, CWMC_Memory_Order_Release_Acquire) {
  std::string work = "Working";
  std::atomic<bool> ready(false);
  std::thread t1([&work, &ready]() {
    work = "Done";
    ready.store(true, std::memory_order_release);
  });
  std::thread t2([&work, &ready]() {
    while (!ready.load(std::memory_order_acquire)) {
    }
    // work一定是Done
    std::cout << "Work = " << work << "\n";
  });
  t1.join();
  t2.join();
}

class SpinLock {
  std::atomic_flag flag;

public:
  SpinLock() : flag(ATOMIC_FLAG_INIT) {}
  void lock() {
    /**
     * 旧值是 false，将其设置为true，返回false，循环退出，表示flag没有人使用，我抢到了
     * 旧值是 true，返回true，没有抢到flag
     * */
    while (flag.test_and_set(std::memory_order_acquire))
      ;
  }
  void unlock() {
    /**
     * 设置为false，释放flag
     * */
    flag.clear(std::memory_order_release);
  }
};

TEST_F(Tester, CWMC_SpinLock) {
  SpinLock spin;
  auto func = [&spin]() {
    spin.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::this_thread::get_id() << " do work \n";
    spin.unlock();
  };
  std::thread t1(func);
  std::thread t2(func);
  t1.join();
  t2.join();
}

TEST_F(Tester, CWMC_Release_Sequence) {
  std::atomic<int> atom{0};
  int somethingShared{0};
  auto writeShared = [&atom, &somethingShared]() {
    somethingShared = 2011;
    atom.store(2, std::memory_order_release);
  };
  auto readShared = [&atom, &somethingShared]() {
    while (!(atom.fetch_sub(1, std::memory_order_acquire) > 0)) {
      std::this_thread::sleep_for(100ms);
    }
    std::cout << "somethingShared: " << somethingShared << "\n";
  };
  std::thread t1(writeShared);
  std::thread t2(readShared);
  std::thread t3(readShared);

  t1.join();
  t2.join();
  t3.join();
  std::cout << "Atom: " << atom << "\n";
}

} // namespace cvtest::tester
