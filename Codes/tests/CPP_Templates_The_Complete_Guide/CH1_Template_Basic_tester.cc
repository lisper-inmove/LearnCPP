/**
 *
 * Template basics
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {

template <typename T> T max(T a, T b) { return b < a ? a : b; }

template <typename T = std::string> void hello(T name) { std::cout << "Hello " << name << "\n"; }

template <typename T1, typename T2, typename RT> RT myMax(T1 a, T2 b) { return b < a ? a : b; }

template <typename T1, typename T2> auto myMax2(T1 a, T2 b) { return b < a ? a : b; }

TEST_F(Tester, TCG_TemplateBasicTester) {
  int a = 10, b = 11;
  std::cout << "max of " << a << " and " << b << " is: " << max(a, b) << "\n";

  float c = 10.1, d = 11.2;
  std::cout << "max of " << c << " and " << d << " is: " << max(c, d) << "\n";

  hello(1);
  hello("inmove");

  auto ret = myMax<int, double, int>(a, c);
  std::cout << "result of myMax is " << ret << "\n";
}
} // namespace cvtest::tester
