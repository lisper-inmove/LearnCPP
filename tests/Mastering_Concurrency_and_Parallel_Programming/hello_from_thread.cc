#include "tester.h"
#include <iostream>

namespace cvtest::tester {

void hello_from_thread() { std::cout << "Hello From thread !\n"; }

TEST_F(Tester, MCPP_Hello_From_Thread) {
  std::thread t(hello_from_thread);

  t.join();
  std::cout << "Hello from main thread!\n";
}
} // namespace cvtest::tester
