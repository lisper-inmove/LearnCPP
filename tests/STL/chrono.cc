#include "tester.h"
#include "gtest/gtest.h"
#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::milliseconds;
using std::chrono::minutes;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace cvtest::tester {
TEST_F(Tester, STL_Chrono_Timers) {
  auto t0 = system_clock::now();
  std::this_thread::sleep_for(seconds(3));
  auto t1 = system_clock::now();
  auto delta_t = duration_cast<milliseconds>(t1 - t0);
  std::cout << "Time: " << delta_t.count() << "\n";
}

} // namespace cvtest::tester
