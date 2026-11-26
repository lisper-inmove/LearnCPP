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
#include <type_traits>
namespace cvtest::tester {
TEST_F(Tester, CWMC_Atomic_Flag) {
  /**
   * std::atomic_flag 是唯一的一定 lock-free 的原子变量
   * */
  std::cout << std::boolalpha;
  std::atomic_flag flag = ATOMIC_FLAG_INIT;
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

/**
 * 用户自定义原子类型
 * 1. 不能有虚成员函数，也不能有虚基类
 * 2. 必须是 bitwise comparable
 * 3. 必须是 trivial copyable，它的基类，非static成员也必须是 trivial。
 *     不能自定义 copy assignment operator。只能使用 default
 */
class MyAtomicType {
public:
  int a;
  int b;
  MyAtomicType() = default;
  MyAtomicType(int a, int b) : a(a), b(b) {}
  MyAtomicType(const MyAtomicType &) = default;
  MyAtomicType &operator=(const MyAtomicType &) = default;
};
TEST_F(Tester, CWMC_Atomic_Custom_Type) {
  /**
   * 如果以下任何一个类型特征（type trait）为 false，程序将是非良构的
   */
  assert(std::is_trivially_copyable_v<MyAtomicType>);
  assert(std::is_copy_constructible_v<MyAtomicType>);
  assert(std::is_copy_assignable_v<MyAtomicType>);
  assert(std::is_move_constructible_v<MyAtomicType>);
  assert(std::is_move_assignable_v<MyAtomicType>);

  std::atomic<MyAtomicType> atomicValue;
  atomicValue.store(MyAtomicType(1, 2));
  std::cout << "atomicValue is (" << atomicValue.load().a << ", "
            << atomicValue.load().b << ")\n";
}

/**
 * 一个已有的对象的引用可以被包装成原子引用类型
 * 这个引用在使用期间，可以拥有原子操作的语义
 * 1. 或者运行时确保对象地址满足 required_alignment。
 * 标准规定，如果对象没有对齐到 required_alignment，构造 atomic_ref 是未定义行为。
 * 通常对基础类型没问题，但对结构体或容器元素要特别检查。
 * 2. atomic_ref有效期内，底层对象必须存活，atomic_ref不延长对象生命周期
 * 3. 一旦某个对象在多线程中被通过atomic_ref访问，所有对该对象的访问都必须通过atomic_ref进行
 * 4. atomic_ref 只保护整体T的原子性，不能保护T的成员变量的原子性
 */
TEST_F(Tester, CWMC_Atomic_Ref) {
  struct Counters {
    int a;
    int b;
  };
  assert(alignof(Counters) >= alignof(std::atomic<Counters>));
  Counters c;
  std::atomic_ref<Counters> ar(c);
  ar.store(Counters{1, 2});
  auto v = ar.load();
  std::cout << "c is (" << v.a << ", " << v.b << "\n";
}

/**
 * atomic<shared_ptr<T>>: 共享指针的原子操作
 *  如何安全地在多个线程间并发地读写 std::shared_ptr 对象
 *  一个shared_ptr对象包括两个指针，一个指向控制块，一个指向引用计数
 *  多线程之间共享同一个shared_ptr对象时，可能会出现数据竞争
 *  
 * 大部分实现中 atomic<shared_ptr<T>> 内部是使用自旋锁来保证原子性，性能可能不如使用 mutex
 * 使用 is_lock_free 在运行时检查是否保证无锁
 */
class Config {
  using DataMap = std::map<std::string, std::string>;
  std::atomic<std::shared_ptr<const DataMap>> data_;

public:
  Config() : data_(std::make_shared<const DataMap>()) {}
  std::string get(const std::string &key) const {
    auto snapshot = data_.load();
    auto it = snapshot->find(key);
    return (it != snapshot->end()) ? it->second : "";
  }
  void set(const std::string &key, const std::string &value) {
    auto old = data_.load();
    std::shared_ptr<const DataMap> desired;
    do {
      auto updated = std::make_shared<DataMap>(*old);
      (*updated)[key] = value;
      desired = std::move(updated);
    } while (!data_.compare_exchange_weak(old, desired));
  }
};

/**
 * atomic<T*>
 * 指针本身是原子的，与它指向的值无关
 */
TEST_F(Tester, CWMC_Atomic_Pointer) {
  static_assert(std::atomic<int *>::is_always_lock_free);
  int a[] = {1, 2, 3, 4, 5, 6};
  std::atomic<int *> v(a); // a[0]
  int *b = v.load();
  v++; // a[1]
  v--; // a[0]
  ++v;
  --v;
  v.fetch_add(2); // a[2]
  v.fetch_sub(2); // a[0]
  std::cout << "Value of v is " << v.load() << "\n";
}

TEST_F(Tester, CWMC_Atomic_Integral) {
  std::atomic<int> v(10);

  v.fetch_add(10);
  v.fetch_and(1);
  v.fetch_xor(100);
}

} // namespace cvtest::tester
