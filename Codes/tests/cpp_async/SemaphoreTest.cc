#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <semaphore>
#include <thread>
#include <vector>
using namespace std::chrono_literals;

/**
 * 信号量
 *    二元信号量: 类似于互斥锁，只有0和1两种状态
 *    计数信号量: 可以大于1，用于控制对具有有限数量实例的资源访问
 *    acquire: P操作，尝试减少计数，若计数为0,则阻塞
 *    try_acquire: P操作，不阻塞，失败返回false
 *    try_acquire_for/try_acquire_until: 带超时的尝试获取
 *    release: V操作，增加计数器
 * */

std::counting_semaphore<4> semaphore(3);

namespace cvtest::tester {
void worker(int id) {
  semaphore.acquire();
  // GTEST_LOG_(INFO) << "Thread " << id << " is working" << "\n";
  std::cout << "Thread " << id << " is working" << "\n";
  std::this_thread::sleep_for(1s);
  semaphore.release();
}

TEST_F(Tester, SemaphoreTester) {
  std::vector<std::thread> threads;
  threads.reserve(10);
  for (int i = 0; i < 10; i++) {
    threads.emplace_back(worker, i);
  }
  for (auto &t : threads) {
    t.join();
  }
}

} // namespace cvtest::tester
