#include "tester.h"
#include "gtest/gtest.h"
#include <barrier>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

/**
 * 屏障
 *  与门闩不同的是。屏障可以重置
 *  屏障用于多阶段任务。
 *  多个线程同时进行一个阶段的任务，任何一个线程完成任务之后在屏障处等待
 *  所有线程都完成之后，屏障自动重置，同时开始进行下一阶段任务
 * */

namespace cvtest::tester {
TEST_F(Tester, BarrierTester) {
  // 回调函数，将在所有线程完成时被调用
  auto onComplete = []() noexcept { std::cout << "My work is done.\n"; };
  std::barrier mapBarrier(3, onComplete);

  auto worker = [&](std::vector<int> &numbers, int start, int end) {
    for (int i = start; i < end; i++) {
      numbers[i] *= 2;
    }

    // 推荐：用 static 复用引擎，避免每次重新初始化
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(3, 5); // 闭区间 [3, 5]
    int seconds = dist(gen);
    std::cout << "等待 " << seconds << " 秒...\n";
    std::this_thread::sleep_for(std::chrono::seconds(seconds));

    auto now = std::chrono::system_clock::now();
    auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), now};
    // GTEST_LOG_(INFO) << local_time << " Wait others\n";
    std::cout << local_time << " Wait others\n";
    mapBarrier.arrive_and_wait();
  };
  std::vector<int> numbers{};
  numbers.reserve(3000);
  for (int i = 0; i < 3000; i++) {
    numbers.push_back(i);
  }
  int sum = 0;

  std::vector<std::jthread> workers;
  workers.reserve(3);
  for (int i = 0; i < 3; i++) {
    workers.emplace_back(worker, std::ref(numbers), 1000 * i, 1000 * (i + 1));
  }
  for (auto &t : workers) {
    t.join();
  }

  std::this_thread::sleep_for(1s);
  sum = std::accumulate(numbers.begin(), numbers.end(), 0);
  GTEST_LOG_(INFO) << "Result of sum is " << sum << "\n";
}

} // namespace cvtest::tester
