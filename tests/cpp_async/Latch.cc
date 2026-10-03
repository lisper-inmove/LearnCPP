#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <latch>
#include <numeric>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

/**
 * 门闩
 *  将一个任务分成独立的多个部分。完成之后，再由一个汇总的线程做最后的总结
 * */

std::latch mapLatch{2};
std::latch reduceLatch{1};

namespace cvtest::tester {
TEST_F(Tester, LatchTester) {
  auto mapThread = [&](std::vector<int> &numbers, int start, int end) {
    for (int i = start; i < end; i++) {
      numbers[i] *= 2;
    }
    GTEST_LOG_(INFO) << std::this_thread::get_id() << " job done\n";
    mapLatch.count_down();
  };

  auto reduceThread = [&](const std::vector<int> &numbers, int &sum) {
    mapLatch.wait();
    for (auto &i : numbers) {
      std::cout << i << " ";
    }
    std::cout << "\n";
    sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    reduceLatch.count_down();
  };

  std::vector<int> numbers{1, 2, 3, 4, 5};
  int sum = 0;
  int N = numbers.size();
  std::thread t1(mapThread, std::ref(numbers), 0, N / 2);
  std::thread t2(mapThread, std::ref(numbers), N / 2, N);
  std::thread t10(reduceThread, std::ref(numbers), std::ref(sum));
  t1.join();
  t2.join();
  t10.join();

  reduceLatch.wait();
  GTEST_LOG_(INFO) << "Result of sum is " << sum << "\n";
}

} // namespace cvtest::tester
