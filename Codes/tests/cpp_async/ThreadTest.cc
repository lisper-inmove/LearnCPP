#include "tester.h"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <string>
#include <syncstream>
#include <thread>

#define sync_cout std::osyncstream(std::cout)
using namespace std::chrono_literals;

/**
 * 异步编程
 * */

namespace cvtest::tester {
TEST_F(Tester, AsyncTester) {
  std::cout << "C++ 异步编程" << "\n";

  std::cout << "总核心数 " << std::thread::hardware_concurrency() << "\n";
}

// ----------------------- 创建线程 -------------------------------
void func() { std::cout << "Using function pointer\n"; }

class FuncObjectClass {
public:
  void operator()() { std::cout << "Using function object class\n"; }
};

class Obj {
public:
  void func() { std::cout << "Using a non-static member function\n"; }
};

class SObj {
public:
  static void staticFunc() { std::cout << "Using a static memberfunction\n"; }
};

TEST_F(Tester, ThreadCreateTester) {
  // 1. 用函数指针创建线程
  std::thread t1(func);

  // 2. 用lambda函数创建线程
  auto lambdaFunc = []() { std::cout << "Using lambda function\n"; };
  std::thread t2(lambdaFunc);

  // 3. lambda 可以直接嵌入
  std::thread t3([]() { std::cout << "Using embedded lambda function\n"; });

  // 4. 用函数对象
  std::thread t4{FuncObjectClass()};

  // 5. 用类的成员函数
  Obj obj{};
  std::thread t5(&Obj::func, &obj);

  // 6. 用静态成员函数
  std::thread t6(&SObj::staticFunc);

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  t6.join();
}

// --------------- 线程的操作 -------------------------------
// get_id返回的线程Id，支持 std::hash，所以它可以
// 比较，序列化，打印，也可以做为map的键

void ThreadFunc() {
  auto threadId = std::this_thread::get_id();
  std::cout << "Current Thread id is " << threadId << "\n";
  auto threadStart = std::chrono::steady_clock::now();
  auto milliseconds = std::chrono::milliseconds(1000);
  auto seconds = std::chrono::seconds(1);
  // sleep_for 会累积误差（因为工作耗时+睡眠 > 周期）
  std::this_thread::sleep_for(milliseconds);
  std::this_thread::sleep_for(seconds);
  auto sleepForEnd = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      sleepForEnd - threadStart);
  std::cout << "Sleep for " << elapsed << " milliseconds\n";

  auto sleepUnitStart = std::chrono::steady_clock::now();
  std::this_thread::sleep_until(sleepUnitStart + 2s);
  auto sleepUntilEnd = std::chrono::steady_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      sleepUntilEnd - sleepUnitStart);
  std::cout << "Sleep until " << elapsed << " milliseconds\n";
}

TEST_F(Tester, ThreadSleepTester) {
  std::thread t1(ThreadFunc);
  t1.join();
}

// -------------------- 参数传递 ------------------------------

void ThreadFuncWithParameters0(int val) {
  // 通过值传递，避免数据竞争，但是效率不高
  std::cout << " val is " << val << "\n";
}

void ThreadFuncWithParameters1(std::string &str, int &val) {
  // 引用传参
  std::cout << "str is " << str << " val is " << val << "\n";
}

void ThreadFuncWithParameters2(const std::vector<int> &v) {
  std::cout << "Size of v is " << v.size() << "\n";
}

TEST_F(Tester, ThreadParameters) {
  std::string str{"Hello World"};
  int val = 10;
  std::vector vec{10, 11};
  std::thread t1(ThreadFuncWithParameters0, 1);
  std::thread t2(ThreadFuncWithParameters1, std::ref(str), std::ref(val));
  std::thread t3(ThreadFuncWithParameters2, std::cref(vec));

  t1.join();
  t2.join();
  t3.join();
}

// -------------------线程是可以移动的 -------------------
// std::thread t2 = std::move(t1);
// t2.join()

// --------------------- 线程的 join ---------------------
// 1. 如果尚未对线程调用join函数，那么线程是可连接且因此是活动的
// 2. 默认构造的线程 或 已连接 的线程是不可连接的
//    std::thread t1; // 不可连接
// 3. 使用 t.joinable() 来判断是否可连接

// --------------------- 线程的 detach --------------------

void daemonThread() {
  // std::cout << "Daemon thread starting...\n";
  sync_cout << "Daemon thread starting...\n";
  int timeout = 10;
  while (timeout > 0) {
    sync_cout << "Daemon thread is running...\n";
    std::this_thread::sleep_for(1s);
    timeout--;
  }
  sync_cout << "Daemon thread exiting...\n";
}

TEST_F(Tester, DaemonThreadTester) {
  std::thread t1(daemonThread);

  t1.detach();
  std::this_thread::sleep_for(15s);
}

// -------------------------jthread ----------------------
void JThreadFunc() {
  std::cout << "This is a jthread " << std::this_thread::get_id() << " .\n";
}
TEST_F(Tester, JThreadTester) { std::jthread t1(JThreadFunc); }

// ------------------- 让出线程执行 -----------------------
// std::this_thread::yield
namespace {
int count = 5;
std::mutex mtx;
} // namespace
TEST_F(Tester, YieldTester) {
  auto work = [&](const std::string &name) {
    while (count > 0) {
      bool workToDo = rand() % 2;
      if (workToDo) {
        sync_cout << name << ": working\n";
        std::lock_guard<std::mutex> lock(mtx);
        for (auto start = std::chrono::steady_clock::now(), now = start;
             now < start + 3s; now = std::chrono::steady_clock::now()) {
        }
      } else {
        sync_cout << name << ": yielding\n";
        std::this_thread::yield();
      }
      count--;
    }
  };
  std::jthread t1(work, "t1");
  std::jthread t2(work, "t2");
}

// ---------------- 取消线程 ------------------------
// 通过一个变量来控制线程的取消
TEST_F(Tester, CancelThreadTester) {
  class Counter {
    using Callback = std::function<void(void)>;

  public:
    Counter(const Callback &callback) {
      t = std::jthread([callback, this] {
        while (running.load() == true) {
          callback();
          std::this_thread::sleep_for(1s);
        }
      });
    }
    void stop() { running.store(false); }

  private:
    std::jthread t;
    std::atomic_bool running{true};
  };
  Counter counter([&]() {
    auto now = std::chrono::system_clock::now();
    sync_cout << now << ": Callback: Running...\n";
  });
  std::this_thread::sleep_for(3s);
  counter.stop();
}

template <typename T>
void show_stop_props(std::string_view name, const T &stop_item) {
  GTEST_LOG_(INFO) << std::boolalpha << name
                   << ": stop_posible = " << stop_item.stop_possible()
                   << ", stop_requested = " << stop_item.stop_requested()
                   << "\n";
}

void func_with_stop_token(std::stop_token stop_token) {
  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(300ms);
    if (stop_token.stop_requested()) {
      GTEST_LOG_(INFO) << "stop_worker: "
                       << "Stopping as requested\n";
      return;
    }
    GTEST_LOG_(INFO) << "stop_worker: Going back to sleep\n";
  }
}

// 通过stop_token来实现 (C++20)
TEST_F(Tester, CancelThreadTesterStopToken) {
  auto worker1 = std::jthread(func_with_stop_token);
  std::stop_token stop_token = worker1.get_stop_token();
  show_stop_props("stop_token", stop_token);
  std::this_thread::sleep_for(1s);
  worker1.request_stop();
  worker1.join();
  show_stop_props("stop_token after request", stop_token);
}

// 使用stop_source来在另一个线程中停止
TEST_F(Tester, CancelThreadTesterStopSource) {
  auto worker = std::jthread(func_with_stop_token);
  std::stop_source stop_source = worker.get_stop_source();
  show_stop_props("stop_source", stop_source);
  auto stopper = std::thread(
      [](std::stop_source source) {
        std::this_thread::sleep_for(500ms);
        GTEST_LOG_(INFO) << "Request stop for worker"
                         << "via source\n";
        source.request_stop();
      },
      stop_source);
  stopper.join();
  std::this_thread::sleep_for(200ms);
  show_stop_props("stop_source after request", stop_source);
}

// 注册回调函数，当通过令牌或停止源请求时，该函数将被调用
TEST_F(Tester, CancelThreadTesterCallback) {
  auto worker = std::jthread(func_with_stop_token);
  std::stop_callback callback1(worker.get_stop_token(), [] {
    GTEST_LOG_(INFO) << "Stop callback for worker " << "executed by thread "
                     << std::this_thread::get_id() << "\n";
  });
  GTEST_LOG_(INFO) << "Main thread " << std::this_thread::get_id() << "\n";
  std::stop_callback callback2(worker.get_stop_token(), [] {
    GTEST_LOG_(INFO) << "Stop callback for worker " << "executed by thread "
                     << std::this_thread::get_id() << "\n";
  });
  std::this_thread::sleep_for(1s);
  worker.request_stop();
  worker.join();
}

// ---------------- Thread Local -----------------------------
TEST_F(Tester, ThreadLocalTester) {
  thread_local int val = 1;
  std::thread t1([]() {
    val = val * 2 + 1;
    GTEST_LOG_(INFO) << "Value of val is " << val << "\n";
  });
  std::thread t2([]() {
    val = val * 2 + 3;
    GTEST_LOG_(INFO) << "Value of val is " << val << "\n";
  });

  t1.join();
  t2.join();
}

} // namespace cvtest::tester
//
