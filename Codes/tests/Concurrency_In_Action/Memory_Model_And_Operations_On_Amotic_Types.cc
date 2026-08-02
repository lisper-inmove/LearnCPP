/**
 * 第五章 内存模型与原子变量
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <atomic>
#include <iostream>

namespace cvtest::tester {
TEST_F(Tester, CIA_Concurrency_In_Action_CH5_Tester) {
  std::cout << "C++ 异步编程 - CH4" << "\n";

  std::cout << "总核心数 " << std::thread::hardware_concurrency() << "\n";
}

TEST_F(Tester, CIA_AtomicBasicTester) {
  /**
   * is_lock_free 返回false表明，内部实际使用的是锁
   * */
  std::atomic_uint8_t value = 10;
  std::cout << "is lock free " << value.is_lock_free() << "\n";

  // 是否在所有支持的硬件中总是 lock_free
  std::cout << "is always lock free " << std::atomic<int>::is_always_lock_free << "\n";

  // 0: never lock free
  // 1: 是否无锁取决于变量的内存对齐、具体 CPU
  //    型号或操作系统内核。程序启动时需要动态检查，才能知道当前实例是否无锁。
  // 2: always lock free
  std::cout << "is bool always lock free " << ATOMIC_BOOL_LOCK_FREE << "\n";

  /**
   * ATOMIC_BOOL_LOCK_FREE
   * ATOMIC_CHAR_LOCK_FREE
   * ATOMIC_CHAR16_T_LOCK_FREE
   * ATOMIC_CHAR32_T_LOCK_FREE
   * ATOMIC_WCHAR_T_LOCK_FREE
   * ATOMIC_SHORT_LOCK_FREE
   * ATOMIC_INT_LOCK_FREE
   * ATOMIC_LONG_LOCK_FREE
   * ATOMIC_LLONG_LOCK_FREE
   * ATOMIC_POINTER_LOCK_FREE
   */

  if (ATOMIC_INT_LOCK_FREE == 2) {
    printf("int 原子操作永远无锁，可使用快速路径。\n");
  } else if (ATOMIC_INT_LOCK_FREE == 1) {
    // 运行时检查具体变量
    if (atomic_is_lock_free(&value)) {
      printf("当前这个 int 变量是无锁的（可能因为对齐良好）。\n");
    } else {
      printf("当前这个 int 变量是加锁的（可能未对齐）。\n");
    }
  } else { // 值为 0
    printf("int 原子操作使用互斥量，应视为加锁操作。\n");
  }

  // std::atomic_flag 是最基础的，永远无锁的原子布尔类型
  // std::atomic_flag 没有 is_lock_free 函数，它一定是 lock_free的
  // 它只有 test_and_set() 以及 clear() 操作
  // c++ 20 之后增加了 wait, notify_one，以及 notify_all
}

TEST_F(Tester, CIA_MemoryOrderTester) {
  /**
   *
   * std::memory_order_relaxed
   *  只保证当前操作的原子性，不保证任何顺序性
   *  编译器可以随意重排它周围的代码，cpu也可以乱序执行
   * std::memory_order_release
   *  只能用于Store操作
   *  保证在此语句之前的所有读写操作，都必须在此之前完成
   * std::memory_order_acquire
   *  只能用于Load操作
   *  保证在些语句之前的所有读写操作，都必须在这次读取之后才能执行
   * std::memory_order_consume
   * std::memory_order_acq_rel
   *  先获取，后释放。它保证之前的操作不能排到后面
   * std::memory_order_seq_cst, default
   *  最强顺序
   *  除了包含acq_rel的效果外，它还保证所有线程看到的顺序总是一致的
   *
   * Store operations, which can have memory_order_relaxed,
   *   memory_order_release, or memory_order_seq_cst ordering
   * Load operations, which can have memory_order_relaxed, memory_order_consume,
   *   memory_order_acquire, or memory_order_seq_cst ordering
   * Read-modify-write operations, which can have memory_order_relaxed, memory_
   *   order_consume , memory_order_acquire, memory_order_release, memory_order
   *   _acq_rel, or memory_order_seq_cst ordering
   * */
}

TEST_F(Tester, CIA_BoolTester) {
  std::atomic_bool value{false};

  /**
   *
   * 在 CIA_BoolTester 中，value 是 std::atomic_bool类型。
   * 两者在当前代码中行为完全等价，但有一个关键区别：
   *   value = false — 调用operator=，始终使用
   *   std::memory_order_seq_cst（最强的默认内存序），你无法改变它。
   *   value.store(false) — 调用 store() 成员函数，默认也是
   *   std::memory_order_seq_cst，但你可以选择传入更弱的内存序来优化性能：
   *   value.store(false, std::memory_order_release);  // 更弱的内存序，性能更好
   *   value.store(false, std::memory_order_relaxed);  // 最弱，仅保证原子性
   * 总结：在当前写法（都不指定内存序）下，两者没有任何区别。store()
   * 的优势在于当你不需要全序一致性（seq_cst）时，可以显式指定更弱的内存序以减少同步开销。这在你的
   *
   * */
  value.store(false);
  value = false;
  auto result = value.exchange(true);
  std::cout << "before value is: " << result << "\n";
  std::cout << "now value is: " << value.load() << "\n";

  bool expected = true;
  // 原值与expected相同时，则把值修改为第二个值。 修改成功返回true，否则返回 false
  auto try1 = value.compare_exchange_strong(expected, false);
  auto try2 = value.compare_exchange_strong(expected, false);
  std::cout << "try1 " << try1 << " , try2 " << try2 << "\n";
}

} // namespace cvtest::tester
