/**
 *
 * Chapter 2 Memory Model
 *
 * 1. strong memory model: sequential consistency
 *  同一线程里的程序一定是按顺序执行的
 *  假设 thread1 x.tore, res1 = y.load
 *       thread2 y.store, res2 = x.load
 *  全局执行顺序里面可能的顺序是
 *    1. x.store, y.load, y.store, x.load
 *    2. x.store, y.store, y.load, x.load
 *    3. x.store, y.store, x.load, y.load
 *    4. y.store, x.load, x.store, y.load
 *    5. y.store, x.store, y.load, x.load
 *    6. y.store, x.store, x.load, y.load
 *  无论如何，x.store一定在y.load前面
 *            y.store一定在x.load前面
 *  这个就是顺序一致性，它是强一致性
 *
 * 2. weak memory model: relaxed-semantic
 *  与 strong memory model 相对的
 *  thread1 有可能看到 x.load 在 y.store 前面
 *
 *
 * 1. std::memory_order_seq_cst: 原子变量的默认内存序
 *  x.store(1) 等同于 x.store(1, std::memory_order_seq_cst)
 *
 * */
#include "tester.h"
#include <atomic>
#include <ios>
#include <iostream>
#include <memory>
#include <thread>
namespace cvtest::tester {
TEST_F(Tester, CWMC_Atomic_Flag) {
  /**
   * std::atomic_flag 是唯一的一定 lock-free 的原子变量
   * */
  std::cout << std::boolalpha;
  std::atomic_flag flag{false};
  std::cout << "before test_and_set value of flag is " << flag.test() << "\n";
  flag.test_and_set(std::memory_order_seq_cst);
  std::cout << "after test_and_set value of flag is " << flag.test() << "\n";
  flag.clear(std::memory_order_seq_cst);
  std::cout << "after clear value of flag is " << flag.test() << "\n";
  std::cout << "Current value is " << flag.test() << "\n";

  auto t1 = std::thread([&flag]() {
    flag.wait(!flag.test()); // flag与参数值不等时阻塞
    std::cout << "[" << std::this_thread::get_id() << "] flag is true\n";
  });
  auto t2 = std::thread([&flag]() {
    flag.test_and_set();
    std::cout << "[" << std::this_thread::get_id() << "] set flag to true\n";
    std::cout << "Notify another thread\n";
    flag.notify_one();
    // flag.notify_all();
  });
  t1.join();
  t2.join();
}

TEST_F(Tester, CWMC_Lock_Free) {
  std::cout << std::boolalpha;
  if (std::atomic<int>::is_always_lock_free) {
    assert(std::atomic<int>().is_lock_free());
    std::cout << "if is_always_lock_free, then must lock_free\n";
  }
  std::cout << "is atomic<float> always lock free "
            << std::atomic<float>::is_always_lock_free << "\n";
  std::cout << "is atomic<float> lock free "
            << std::atomic<float>().is_lock_free() << "\n";
}

/**
 * is_lock_free: 检查是否是 lock-free
 * atomic_ref<T>::is_always_lock_free: 编译期检查是否是 always-lock-free
 * load: 返回原子变量的值
 * operator T: 与load一样
 * store: 修改原子变量的值
 * exchange: 修改原子变量的值，并返回旧值
 * compare_exchange_strong: 比较并交换值
 * compare_exchange_weak
 * notify_one(c++20)
 * notify_all(c++20)
 * wait(c++20)
 *
 * */
TEST_F(Tester, CWMC_Atomic_Bool) {
  std::cout << std::boolalpha;
  std::atomic<bool> flag{false};
  std::cout << "Value of flag is " << flag << "\n";
  flag.store(true);
  std::cout << "After flag.store(true), flag is " << flag << "\n";
  flag.load(std::memory_order_relaxed);
}

TEST_F(Tester, CWMC_Compare_Exchange) {
  /**
   * bool compare_exchange_strong(T& expected, T desired);
   * bool compare_exchange_weak(T& expected, T desired);
   * 在一次原子操作中进行值的交换(Compare And Swap又叫CAP)
   * strong与weak的区别在于：weak在atomicValue == expected的时候也有可能返回 false
   *
   * 如果 atomicValue 与 expected 相同，返回True，并将值设置为 desired
   * 如果 比对失败，expected 被设置为 atomicValue
   * expected既有入参也有出参的作用
   *
   * 等同于以下语句:
   * if (atomicValue == expected) {
   *  atomicValue = desired;
   *  return true;
   * } else {
   *  expected = atomicValue; // 为了让调用者知道当前是什么值
   *  return false;
   * }
   *
   * */
  std::cout << std::boolalpha;
  std::atomic<int> value{0};
  value.store(30);

  int expected = 20;
  int desired = 10;
  std::cout << "Before value is " << value << "\n";
  std::cout << "Before expected is " << expected << "\n";
  std::cout << "Before desired is " << desired << "\n";

  value.compare_exchange_strong(expected, desired);
  std::cout << "-----------------splitter---------------------------\n";

  std::cout << "After value is " << value << "\n";
  std::cout << "After expected is " << expected << "\n";
  std::cout << "After desired is " << desired << "\n";
  std::cout << "####################################################\n";

  std::cout << "Before value is " << value << "\n";
  std::cout << "Before expected is " << expected << "\n";
  std::cout << "Before desired is " << desired << "\n";
  value.compare_exchange_strong(expected, desired);
  std::cout << "-----------------splitter---------------------------\n";
  std::cout << "After value is " << value << "\n";
  std::cout << "After expected is " << expected << "\n";
  std::cout << "After desired is " << desired << "\n";
}

} // namespace cvtest::tester
