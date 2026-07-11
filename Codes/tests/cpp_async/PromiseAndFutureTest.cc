#include "tester.h"
#include "gtest/gtest.h"
#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <thread>
using namespace std::chrono_literals;

/**
 * Promise And Future
 * */

namespace cvtest::tester {
void task001(std::promise<int> &&prom) {
  // 如果promise在调用set_value或set_exception前被销毁
  // 会抛出 broken_promise异常
  try {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // 设置值唤醒future
    prom.set_value(42);
  } catch (...) {
    prom.set_exception(std::current_exception());
  }
}

int heavyWork(int a, int b) {
  std::this_thread::sleep_for(1s);
  return a + b;
}

TEST_F(Tester, PFTester) {
  std::promise<int> prom;
  std::future<int> fut = prom.get_future();
  // promise不可复制，只能移动
  std::jthread worker(task001, std::move(prom));
  worker.detach();
  std::cout << "Received: " << fut.get() << "\n";
}

TEST_F(Tester, PFWithTimeoutTester) {
  std::promise<int> prom;
  std::future<int> fut = prom.get_future();
  std::jthread worker(task001, std::move(prom));
  auto status = fut.wait_for(100ms);
  worker.detach();
  if (status == std::future_status::ready) {
    std::cout << "Work done...\n";
    std::cout << "Received: " << fut.get() << "\n";
  } else if (status == std::future_status::timeout) {
    std::cout << "Task timedout...\n";
  } else if (status == std::future_status::deferred) {
    std::cout << "Task deferred strategy\n";
  }
}

TEST_F(Tester, PFPackagedTaskTester) {
  std::packaged_task<int(int, int)> task(heavyWork);
  std::future<int> fut = task.get_future();
  std::thread t(std::move(task), 5, 3);
  t.detach();
  std::cout << "Result: " << fut.get() << "\n";
}

/**
 * Async 内部自动创建 promise和future，并返回future
 * 适合一次性的简单的异步任务
 * */
TEST_F(Tester, PFAsyncTester) {
  std::future<int> fut = std::async(std::launch::async, heavyWork, 10, 20);
  std::cout << "Do something other...." << "\n";
  int sum = fut.get();
  std::cout << "Result: " << sum << "\n";
}

TEST_F(Tester, PFSharedFutureTester) {
  std::promise<int> prom;
  std::shared_future<int> sharedFut = prom.get_future().share();
  std::thread producer([&prom]() {
    std::this_thread::sleep_for(2s);
    std::cout << "[Producer] finished, set value to 42\n";
    prom.set_value(42);
  });

  std::vector<std::thread> consumers;
  consumers.reserve(5);
  for (int i = 0; i < 5; i++) {
    consumers.emplace_back([&sharedFut, i]() {
      std::cout << "[Consumer " << i << "]" << "wait for result\n";
      int value = sharedFut.get();
      std::cout << "[Consumer " << i << "]" << "get result " << value << "\n";

      // int again = sharedFut.get(); // 拿到相同的值，且不会阻塞
    });
  }
  producer.join();
  for (auto &t : consumers) {
    t.join();
  }
}

} // namespace cvtest::tester
