/**
 *
 * Chapter one: Function Templates
 *
 * */
#include "tester.h"
#include <iostream>
#include <type_traits>
namespace cvtest::tester {
// 初识函数模板
template <typename T> T max0(T a, T b) { return a > b ? a : b; }
TEST_F(Tester, T_Function_Template) {
  std::cout << "max0 10 and 20 maximum is: " << max0<int>(10, 20) << "\n";
}

// common_type_t
template <typename T1, typename T2> std::common_type_t<T1, T2> max1(T1 a, T2 b) {
  return b < a ? a : b;
}
TEST_F(Tester, T_Return_Type_as_Common_Type) {
  std::cout << "max1 10 and 20.0 maximum is: " << max1<int, double>(10, 20.0) << "\n";
}

// Default template argument
// std::decay_t 保证不返回引用
// 其中 std::decay_t<T> 会做这些事：
// 1. 去掉引用：T&、T&& → T
// 2. 去掉顶层 const/volatile：const T → T
// 3. 数组退化为指针：T[N] → T*
// 4. 函数退化为函数指针：R(Args...) → R(*)(Args...)
template <typename T1, typename T2, typename RT = std::decay_t<decltype(true ? T1() : T2())>>
// template <typename T1, typename T2, typename RT = std::common_type_t<T1, T2>> // 同上面的写法等同
RT max2(T1 a, T2 b) {
  return b < a ? a : b;
}
TEST_F(Tester, T_Default_Template_Argument) {
  std::cout << "max2 10 and 20.0 maximum is: " << max2<int, double>(10, 20.0) << "\n";
}

// 模板函数的重载
int max3(int a, int b) { return a > b ? a : b; }
template <typename T> T max3(T a, T b) { return a > b ? a : b; }
TEST_F(Tester, T_Overload_Template) {
  max3(10, 11);   // 调用非模板
  max3<>(10, 11); // 调用框框
}

} // namespace cvtest::tester
