/**
 *
 * Template basics
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {

template <typename T> T max(T a, T b) { return b < a ? a : b; }

// 如果要定义默认参数，就要同时定义默认的 模板类型
template <typename T = std::string> void hello(T name = "") {
  std::cout << "Hello " << name << "\n";
}

// define a template for return type
// multiple template parameters
template <typename T1, typename T2, typename RT> RT myMax(T1 a, T2 b) { return b < a ? a : b; }

// 自动返回类型推导
template <typename T1, typename T2> auto myMax2(T1 a, T2 b) { return b < a ? a : b; }

template <typename T1, typename T2> std::common_type_t<T1, T2> myMax3(T1 a, T2 b) {
  return b < a ? a : b;
}

/**
 *
 * 二阶段检查
 *
 * 第一阶段,模板定义阶段
 * 1. 语法错误
 * 2. 使用了与模板无关的未知名称
 * 3. 使用了与模板无关的静态断言
 *
 * 第一阶段,模板实例化阶段
 * 1. 依赖模板的参数被检查
 *
 * */

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

// 模板重载 ################################################################

int min(int a, int b) {
  std::cout << "nonetemplate called\n";
  return a < b ? a : b;
}

template <typename T> T min(T a, T b) {
  std::cout << "Template min called\n";
  return a < b ? a : b;
}

template <typename T1, typename T2> auto min1(T1 a, T2 b) {
  std::cout << "min1 no RT called\n";
  return a < b ? a : b;
}

template <typename RT, typename T1, typename T2> RT min1(T1 a, T2 b) {
  std::cout << "min1 with RT called\n";
  return a < b ? a : b;
}

TEST_F(Tester, TCG_TemplateOverloading_Tester) {
  min('a', 9.1);              // 都被转成int
  min(1.3, 1.2);              // template
  min(9, 5);                  // int
  min<>(10, 22);              // template
  min1(4, 7.2);               // no RT
  min1<long, double>(7.2, 4); // with RT called
}

} // namespace cvtest::tester
