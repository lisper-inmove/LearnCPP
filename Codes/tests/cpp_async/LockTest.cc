#include "tester.h"
#include "gtest/gtest.h"
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std::chrono_literals;

/**
 * 锁机制
 * */

namespace cvtest::tester {
TEST_F(Tester, LockTester) { std::cout << "C++ 异步编程 第四章 锁" << "\n"; }

TEST_F(Tester, CounterValue) {
  int counter = 0;
  auto func = [&counter] {
    for (int i = 0; i < 1000000; i++) {
      counter++;
    }
  };
  std::thread t1(func);
  std::thread t2(func);

  t1.join();
  t2.join();
  // Counter的最终值肯定不会等于 20000000
  GTEST_LOG_(INFO) << "Last value of counter is " << counter << "\n";
}

/**
 *                             是否独占         是否递归   是否超时
 * std::mutex                  是               否         否
 * std::recursive_mutex        是               是         否
 * std::shared_mutex           1个独占,N个共享  否         否
 * std::recusive_timed_mutex   是               是         是
 * std::shared_timed_mutex     1个独占,N个共享  否         是
 *
 * mutex: 使用lock或try_lock获得互斥锁,直到调用unlock
 *        一个线程获得锁时，其它线程调用lock时将阻塞,调用try_lock时返回false
 *        一个已经获得互斥锁的线程再次获取时，结果未定义，通常会抛出异常
 *        对已经释放的锁再次释放，结果未定义
 *        线程在持有锁时销毁锁，或者线程终止未释放锁，结果未定义
 *
 * recursive_mutex:
 *        一个线程可以多次获得互斥锁，直到释放次数和获取的次数相同
 *        不同编译器的实现，获取递归锁的最大次数未指定，达到最大次数时 lock抛出
 *        std::system_error异常
 *
 * shared_mutex:
 *        共享: lock_shared, try_lock_shared, unlock_shared
 *        独享: lock, try_lock, unlock
 *
 * timed_mutex, recursive_timed_mutex, shared_timed_mutex:
 *        try_lock_for: 尝试锁定互斥锁并阻塞，直到指定的时间间隔过去（超时）
 *                      如果在指定的时间获取到锁，返回true，否则返回 false
 *                      如果指定的时间小于等于 timeout_duration.zero()，
 *                      它的行为与try_lock相同
 *        try_lock_until
 * */

TEST_F(Tester, CounterValueWithMutex) {
  std::mutex mtx;
  int counter = 0;
  auto func = [&counter, &mtx] {
    for (int i = 0; i < 1000000; i++) {
      mtx.lock();
      counter++;
      mtx.unlock();
    }
  };
  std::thread t1(func);
  std::thread t2(func);

  t1.join();
  t2.join();
  GTEST_LOG_(INFO) << "Last value of counter is " << counter << "\n";
}

/**
 * 死锁: 线程A获得锁1，线程B获得锁2，A等待B释放2，B等待A释放1
 * 活锁: 如果使用超时锁，可以解决死锁的问题，
 *    但是有可能会陷入 A一直获取到1，获取到不到2，超时释放1，再等待这样的循环，
 *    线程除了获取锁，等待锁，释放锁，不能做其它操作，这样的情况称为活锁
 *
 * 死锁与活锁的解决方案为：总是以一致的顺序获取锁，
 *    A总是先获取到1再获取2，B也是先去获取1再获取2，这样就能解决死锁与活锁的问题
 * */

/**
 * 锁管理
 * std::lock_guard
 *    在构造函数中获取锁，在析构函数中释放锁。无法在过程中释放锁
 * std::scoped_lock:
 *    与unique_lock一样，不同的是，它可以管理多个锁，根据锁传入的顺序获取
 * std::unique_lock(mtx):
 *    默认，与lock_guard作用相同
 * std::unique_lock(mtx, std::defer_lock):
 *    延迟锁定，不自动加锁，需要调用lock函数 std::shared_lock，只有
 * std::unique_lock(mtx, std::try_to_lock):
 *    尝试锁定，不阻塞，通过owns_lock()判断是否获取到锁
 * std::unique_lock(mtx, std::adopt_lock):
 *    接管锁，假定当前线程已经调用lock获取到锁权限，该对象将接管锁
 * std::unique_lock(mtx, std::chrono::seconds(1)):
 *    超时锁定，最多等待1秒，超时返回false
 * shared_mutex和shared_timed_mutex可以使用
 *
 * release(): 释放所有权，并不会解锁
 * mutex(): 获取关联的互斥量指针
 *
 * unique_lock与条件变量合作使用
 * wait函数会释放锁，等到被唤醒继续执行
 *
 * std::mutex mtx;
 * std::condition_variable cv;
 * bool date_ready = false;
 * void consumer() {
 *    std::unique_lock<std::mutex> lck(mtx);
 *    cv.wait(lck, []{ return data_ready; })
 *    // 处理数据
 * }
 * */
TEST_F(Tester, LockManagementTester) {}

/**
 * 条件变量
 *
 * 如果在 t1中不使用锁：
 *  C++内存模型，对同一变量的并发读写（其中至少一方是写操作）若无同步机制，
 *     即为数据竞争，程序行为未定义。
 *  编译器优化、CPU 缓存可能导致 t2 永远看不到 t1
 *     的更新，或者看到部分更新的中间值，导致条件判断错乱。
 *  t1 在 count 为 5 的倍数时（包括 5,10,...,100）都会发出通知，
 *     但 t2 可能尚未进入等待状态（因为线程调度顺序不定）。
 *
 *  条件变量通常与互斥锁配合，所有对共享变量的修改必须在持有锁的情况下进行，以确保：
 *    修改操作与条件检查之间存在 happens-before 关系；
 *    等待线程被唤醒后能立即看到最新的共享状态。
 * */
TEST_F(Tester, ConditionVariableTester) {
  int count = 0;
  std::mutex mtx;
  std::condition_variable cv;
  std::thread t1([&count, &cv, &mtx] {
    while (count < 100) {
      std::this_thread::sleep_for(100ms);
      std::unique_lock<std::mutex> lck(mtx);
      count++;
      if (count % 5 == 0) {
        GTEST_LOG_(INFO) << "Try to notify another thread.\n";
        lck.unlock();
        // cv.notify_all();
        cv.notify_one();
      }
    }
  });

  std::thread t2([&count, &cv, &mtx] {
    std::unique_lock<std::mutex> lck(mtx);
    GTEST_LOG_(INFO) << "Wait for notify \n";
    cv.wait(lck, [&count] { return count == 50; });
    GTEST_LOG_(INFO) << "Count is " << count << " \n";
  });

  t1.join();
  t2.join();
}

} // namespace cvtest::tester
