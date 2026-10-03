/**
 * 第五章 内存模型与原子变量
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

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

  /**
   *
   * 特性|compare_exchange_weak|compare_exchange_strong
   * 虚假失败|允许。即使原子变量值与 expected 相等，它也可能返回 false。|不允许。只有当原子变量值与 expected 不相等时，它才会返回 false。
   * 性能|可能更高。为了实现“虚假失败”的灵活性，编译器在某些平台上能生成更高效的代码。|相对较低。需要额外的检查来保证不会虚假失败，这会带来一些开销。
   * 使用场景|循环（Loop）中。因其允许失败，通常需要在循环中重试直到成功。|单次检查或无循环场景。适用于你期望一次调用就能准确反映比较结果的场景。
   *
   * */
  bool expected = true;
  // 原值与expected相同时，则把值修改为第二个值。 修改成功返回true，否则返回 false
  auto try1 = value.compare_exchange_strong(expected, false);
  auto try2 = value.compare_exchange_strong(expected, false);
  auto try3 = value.compare_exchange_weak(expected, false, std::memory_order_acq_rel, std::memory_order_acquire);
  std::cout << "try1 " << try1 << " , try2 " << try2 << " , try3 " << try3 << "\n";
}

class Foo {};

TEST_F(Tester, CIA_AtomicPointerTester) {

  Foo anArray[5];

  /**
   *
   *  fetch_sub, fetch_and, fetch_or, fetch_xor
   *
   * */
  std::atomic<Foo *> foo(anArray);
  Foo *p = foo.fetch_add(2); // p得到的是原值
  std::cout << "foo " << foo << "\n";
  std::cout << "p " << p << "\n";
  assert(p == anArray);
  assert(foo.load() == &anArray[2]);
}

/**
 * C++ 标准规定，T 必须是可平凡复制（trivially copyable） 的类型，同时还需要满足
 *  1. 没有虚函数或虚基类
 *  2. 所有成员都是可平凡复制的；
 *  3. 默认拷贝构造函数、拷贝赋值运算符等是平凡的（或显式默认实现）；
 *  4. 可以安全地使用 memcpy 复制其对象。
 *
 *  简单来说，只要你的类型能像 C 风格的结构体那样按字节复制，通常就符合条件。
 *
 * */
TEST_F(Tester, CIA_UserDefineAtomicTester) {
  struct Point {
    int x, y;
  };
  std::atomic<Point> p1{Point{1, 2}};
  Point pt{5, 6};

  p1.store(pt);
  Point loaded = p1.load();
  std::cout << "p1 x = " << loaded.x << " y = " << loaded.y << "\n";
}

// Listing 5.4
TEST_F(Tester, CIA_SequentialConsistencyTester) {
  std::atomic<bool> x, y;
  std::atomic<int> z;
  auto write_x = [&x] { x.store(true, std::memory_order_seq_cst); };
  auto write_y = [&y] { y.store(true, std::memory_order_seq_cst); };
  auto read_x_then_y = [&x, &y, &z] {
    while (!x.load(std::memory_order_seq_cst))
      ;
    if (y.load(std::memory_order_seq_cst))
      z++;
  };
  auto read_y_then_x = [&x, &y, &z] {
    while (!y.load(std::memory_order_seq_cst))
      ;
    if (x.load(std::memory_order_seq_cst))
      z++;
  };
  x = false;
  y = false;
  z = 0;
  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);
  a.join();
  b.join();
  c.join();
  d.join();
  assert(z.load() != 0);
}

// Listing 5.5
/**
 *
 * 是的，z 可能等于 0。
 * 分析这段代码（第212-221行）：
 * - 线程 A：x.store(true, relaxed) → y.store(true, relaxed)
 * - 线程 B：等 y.load(relaxed) == true → 检查 x.load(relaxed)，若为 true 则 z++
 * 关键在于 memory_order_relaxed 不提供任何线程间的同步或顺序保证。线程 B 看到 y == true，不代表它也能看到 x == true——从线程 B 的视角，线程 A 对 x 和 y
 * 的写入可以以任意顺序变为可见。
 * 在 C++ 标准层面，没有任何 happens-before 关系保证线程 B 在观察到 y == true 时也必然观察到 x == true。因此 z++ 可能不执行，z 保持为 0，第229行的
 * assert(z.load() != 0) 是可能触发失败的。
 * 实际表现取决于硬件内存模型：
 * - x86/64（TSO 强模型）：store-store 不会重排，实际上不会失败
 * - ARM / POWER（弱模型）：完全可能出现 z == 0 的情况
 * */
TEST_F(Tester, CIA_RelaxedOperationTester) {
  std::atomic<bool> x, y;
  std::atomic<int> z;
  auto write_x_then_y = [&x, &y] {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_relaxed);
  };
  auto read_y_then_x = [&x, &y, &z] {
    while (!y.load(std::memory_order_relaxed))
      ;
    if (x.load(std::memory_order_relaxed))
      z++;
  };
  x = false;
  y = false;
  z = 0;
  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);
  a.join();
  b.join();
  assert(z.load() != 0);
}

// Listing 5.6
TEST_F(Tester, CIA_RelaxedOperationMultipleThreadsTester) {
  std::atomic<int> x(0), y(0), z(0);
  std::atomic<bool> go(false);
  unsigned const loop_count = 10;
  struct read_values {
    int x, y, z;
  };
  read_values values1[loop_count];
  read_values values2[loop_count];
  read_values values3[loop_count];
  read_values values4[loop_count];
  read_values values5[loop_count];
  auto increment = [&go, &x, &y, &z](std::atomic<int> *var_to_inc, read_values *values) {
    while (!go)
      std::this_thread::yield();
    for (unsigned i = 0; i < loop_count; i++) {
      values[i].x = x.load(std::memory_order_relaxed);
      values[i].y = y.load(std::memory_order_relaxed);
      values[i].z = z.load(std::memory_order_relaxed);
      var_to_inc->store(i + 1, std::memory_order_relaxed);
      std::this_thread::yield();
    }
  };
  auto read_vals = [&go, &x, &y, &z](read_values *values) {
    while (!go)
      std::this_thread::yield();
    for (unsigned i = 0; i < loop_count; i++) {
      values[i].x = x.load(std::memory_order_relaxed);
      values[i].y = y.load(std::memory_order_relaxed);
      values[i].z = z.load(std::memory_order_relaxed);
      std::this_thread::yield();
    }
  };
  auto print = [](read_values *v) {
    for (unsigned i = 0; i < loop_count; i++) {
      if (i)
        std::cout << ",";
      std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
    }
    std::cout << "\n";
  };

  std::thread t1(increment, &x, values1);
  std::thread t2(increment, &y, values2);
  std::thread t3(increment, &z, values3);
  std::thread t4(read_vals, values4);
  std::thread t5(read_vals, values5);
  go = true;
  t5.join();
  t4.join();
  t3.join();
  t2.join();
  t1.join();
  print(values1);
  print(values2);
  print(values3);
  print(values4);
  print(values5);
}

// Listing 5.7
/**
 *
 *  release/acquire 只在同一原子变量的配对操作间建立 synchronizes-with：
 *  - x.store(release) ↔ x.load(acquire) ✓
 *  - y.store(release) ↔ y.load(acquire) ✓
 *  但 x 和 y 之间没有跨变量顺序保证。以下执行顺序是合法的：
 *  1. 线程 a：x.store(true, release)
 *  2. 线程 c：x.load(acquire) → true（与步骤1同步）；y.load(acquire) → false（线程 b 还没写入，或无跨变量可见性保证）
 *  3. 线程 b：y.store(true, release)
 *  4. 线程 d：y.load(acquire) → true（与步骤3同步）；x.load(acquire) → false（同样没有跨变量保证）
 *  结果：z = 0，断言失败。
 *
 * */
TEST_F(Tester, CIA_AcquireReleaseTester) {
  std::atomic<bool> x, y;
  std::atomic<int> z;
  auto write_x = [&x] { x.store(true, std::memory_order_release); };
  auto write_y = [&y] { y.store(true, std::memory_order_release); };
  auto read_x_then_y = [&x, &y, &z] {
    while (!x.load(std::memory_order_acquire))
      ;
    if (y.load(std::memory_order_acquire))
      z++;
  };
  auto read_y_then_x = [&x, &y, &z] {
    while (!y.load(std::memory_order_acquire))
      ;
    if (x.load(std::memory_order_acquire))
      z++;
  };
  x = false;
  y = false;
  z = 0;
  std::thread a(write_x);
  std::thread b(write_y);
  std::thread c(read_x_then_y);
  std::thread d(read_y_then_x);
  a.join();
  b.join();
  c.join();
  d.join();
  assert(z.load() != 0);
}

// Listing 5.8
TEST_F(Tester, CIA_AcquireReleaseImposeOrderingTester) {
  std::atomic<bool> x, y;
  std::atomic<int> z;
  auto write_x_then_y = [&x, &y] {
    x.store(true, std::memory_order_relaxed);
    y.store(true, std::memory_order_release);
  };
  auto read_y_then_x = [&x, &y, &z] {
    while (!y.load(std::memory_order_acquire))
      ;
    if (x.load(std::memory_order_relaxed))
      z++;
  };
  x = false;
  y = false;
  z = 0;
  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);
  a.join();
  b.join();
  assert(z.load() != 0);
}

} // namespace cvtest::tester
