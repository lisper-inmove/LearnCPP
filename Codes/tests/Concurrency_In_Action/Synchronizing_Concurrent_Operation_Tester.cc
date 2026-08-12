/**
 *
 * 第四章: 同步并行操作
 *
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <chrono>
#include <cstdlib>
#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#define sync_cout std::osyncstream(std::cout)
using namespace std::chrono_literals;

namespace cvtest::tester {
TEST_F(Tester, CIA_Concurrency_In_Action_CH4_Tester) {
  std::cout << "C++ 异步编程 - CH4" << "\n";

  std::cout << "总核心数 " << std::thread::hardware_concurrency() << "\n";
}

/**
 *
 * std::condition_variable: 必须与mutex联合使用
 * std::condition_variable_any: 可以与任何mutex-like联合使用
 *
 * 定义所有线程都能访问的变量
 * std::mutext mut;
 * std::condition_variable cond;
 *
 * 其中一上线程等待被唤醒
 * thread1:
 *  // 获取锁
 *  lk = unique_lock<std::mutex> lk(mut);
 *  // 释放锁，然后阻塞，等待notify函数被调用
 *  // notify函数被调用时，首先获取锁，调用回调函数
 *  // 如果回调返回true，则从wait中返回
 *  // 如果返回 false，则释放锁，继续等待
 *  cond.wait(lk, [] {return xxx == true;});
 *
 * 唤醒一个或者所有
 * thread2:
 *  cond.notify_one() or cond.notify_all();
 * */

/**
 *
 * 4.2 Waiting for one-off events with futures
 * std::future
 * std::shared_future
 *
 * */

/**
 *  Returning values from background tasks
 *  std::async
 *
 * for example:
 * int find_the_answer_to_ltuae();
 * void do_other_stuff();
 * int main() {
 *    std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
 *    do_other_stuff();
 *    std::count << "The answer is " << the_answer.get() << "\n";
 * }
 *
 * // X是一个类，foo是一个成员函数，该成员函数，接收两个参数
 * std::async(&X::foo, &x, 42, "hello");
 * // std::launch::deferred:
 *     不会立即执行，只有对std::async返回的future对象调用get()或者wait()时才在那个线程上执行
 * // std::launch::async: 立即异步执行,明确要求在一个新线程上执行
 * // std::launch::deferred | std::launch::async:
 *      默认值，根据资源自行决策是启动一个新线程立即执行，还是等到get,wait调用时才执行
 * std::async(std::launch::deferred, &X::foo, &x, 42, "Hello");
 *
 * */
TEST_F(Tester, CIA_AsyncTester) {
  auto slowSquare = [](int x) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return x * x;
  };
  std::future<int> fut = std::async(slowSquare, 5);
  std::cout << "Do something... \n";
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "Continue...\n";
  int result = fut.get();
  std::cout << "Result is " << result << "\n";
}

TEST_F(Tester, CIA_AsyncStrategyTester) {
  auto heavyCompute = [](int n) {
    // deferred 输出的threadId与主线程的threadId一样
    std::cout << "Thread ID " << std::this_thread::get_id() << " start calculate....\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return n * 10;
  };
  std::future<int> futAsync = std::async(std::launch::async, heavyCompute, 7);
  std::cout << "async task submited, main thread id is " << std::this_thread::get_id() << "\n";

  std::future<int> futDeferred = std::async(std::launch::deferred, heavyCompute, 3);
  std::cout << "deferred task created, but not execute\n";

  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "call deferred's get() at once\n";
  int resDef = futDeferred.get();
  std::cout << "deferred result: " << resDef << "\n";

  int resAsync = futAsync.get();
  std::cout << "async result " << resAsync << "\n";
}

TEST_F(Tester, CIA_LoopWaitForFutureTester) {
  std::future<int> fut = std::async([]() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 42;
  });

  // 默认的策略有可能是deferred，从而导致一直等待
  // while (fut.wait_for(std::chrono::milliseconds(100)) !=
  //        std::future_status::ready) {
  //   std::cout << "Waiting...\n";
  // }

  std::future_status status;
  do {
    status = fut.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::deferred) {
      std::cout << "task deferred, call get() sync execute\n";
      break;
    } else if (status == std::future_status::timeout) {
      std::cout << "Wait timeout, continue...\n";
    }
  } while (status != std::future_status::ready);
  int result = fut.get();
  std::cout << "Result is " << result << "\n";
}

TEST_F(Tester, CIA_RaiseExceptionTester) {
  auto throwException = [](bool flag) {
    if (flag)
      throw std::runtime_error("Some error");
    return 100;
  };
  std::future<int> fut = std::async(throwException, true);
  try {
    int val = fut.get();
    std::cout << "Value is " << val << "\n";
  } catch (const std::exception &e) {
    std::cout << "Catch an exception " << e.what() << "\n";
  }
}

/**
 * Associating a task with a future
 * std::packaged_task
 * */
TEST_F(Tester, CIA_PackagedTaskTester) {
  // 1. 定义一个packaged_task，包装一个求和函数
  std::packaged_task<int(int, int)> task([](int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return a + b;
  });
  // 线程启动之前获取关联的future
  std::future<int> result = task.get_future();
  // task只能移动
  std::thread t(std::move(task), 10, 20);
  // 阻塞直到task执行完毕
  // 如果包装的函数抛异常，future.get()会向外传播
  std::cout << "计算结果: " << result.get() << "\n";
  t.join();
}

TEST_F(Tester, CIA_PromiseTester) {
  std::promise<int> prom;
  // 从promise中获取future
  std::future<int> fut = prom.get_future();
  std::thread t([&prom]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // set_value立即设置共享状态，并唤醒主线程
    prom.set_value(42);
    // 抛出异常
    // prom.set_exception(std::current_exception());

    // ** 当需要工作线程的所有资源全部清理干净，主线程才工作时
    // 当你需要返回一个依赖于 thread_local
    // 变量的结果，且该变量的析构过程可能会修改或失效这个结果时，就必须用它。
    // 等到线程完全析构后才传值到future
    // thread_local int local_data = 42;
    // prom.set_value_at_thread_exit(local_data);

    // 多次调用 set_vaule或set_exception，抛出 std::future_error
    // 错误码为 promise_already_satisfied

    // 被销毁时如果还未设置任何值或异常，它的析构函数会自动设置一个std::future_error，错误码为broken_promise

    std::cout << "Producer set value \n";
  });
  std::cout << "Wait for result \n";
  // 阻塞直到线程运行完成
  int result = fut.get();
  std::cout << "Get result " << result << "\n";
  t.join();
}

/**
 *
 * promise VS packaged_task
 * 1. promise:
 *   1.1 手动设置值或异常，不包含任何可调用对象。通过set_value来将结果写回future
 *   1.2 将promize移动到线程中，在线程中去设置值
 * 2. packaged_task:
 *   2.1 包含一个可调用对象，只需要调用它，它会自动把返回值设置回future
 *   2.2 它不包含线程，只是任务，执行权由用户控制
 *
 * std::async =
 *    “一键上门取件”（高德地图一键下单）。你只管把东西（函数）交给系统，系统自动派车（创建线程）、规划路线（调度），你最后看物流单号（future）拿结果。最省心，但调度权完全交给平台。
 * std::packaged_task =
 *    “专用打包盒”（把货物打包，贴上未来收件码）。盒子本身不负责运输，你可以亲手把它交给任何司机（std::thread）、放入中转站（线程池队列）。货物（函数）和运输（线程）彻底解耦。
 * std::promise =
 *    “手动扫码枪”。盒子里空空如也，你需要在任意时刻、任意地点手动扫一下码（set_value），数据才会出现在收件人的未来清单（future）里。纯粹的手动数据通道，不包含任何要执行的货物（函数）。
 *
 * */

} // namespace cvtest::tester
//
