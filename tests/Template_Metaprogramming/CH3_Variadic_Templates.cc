/**
 *
 * Variadic Templates
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {

template <typename T> T min(T a, T b) { return a < b ? a : b; }

/**
 * typename... Args: 模板参数包
 * Args... args: 函数参数包
 * min(args...): 参数包展开
 * sizeof...: 一个运算符，用于求参数包里元素的个数
 */
template <typename T, typename... Args> T min(T a, Args... args) {
  if constexpr (sizeof...(args) == 0) {
    return a;
  }
  return min(a, min(args...));
}

TEST_F(Tester, T_Variadic_Template) {
  /**
   * 依次生成6，5，4，3，2 参数的 min 函数 的重载
   * */
  auto minValue = min(1, 2, 3, 4, 5, 6);
  std::cout << "Min value of array is " << minValue << "\n";
}

} // namespace cvtest::tester
