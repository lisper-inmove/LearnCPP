/**
 *
 * Chapter 3: Variadic Templates
 *
 * */
#include "tester.h"
#include <functional>
#include <iostream>
#include <string>
namespace cvtest::tester {
template <typename T> struct parser; // [1] template declaration
void handle(double value) {          // [2] handle function definition
  std::cout << "Processing a double: " << value << "\n";
}
// handle是non-dependent name,所以在定义阶段就会找handle,这个时候只能找到 handle(double)
template <typename T> struct parser { // [3] template definition
  void parse() { handle(42); }        // [4] handle is a non-dependent name
};
// [5] handle(int) definition
void handle(int value) { std::cout << "Processing an int: " << value << "\n"; }

TEST_F(Tester, T_Name_Non_Dependent) {
  parser<int> p; // [6] template instantiation
  p.parse();
}

template <typename T> struct handler2 {
  void handle(T value) { std::cout << "handler<T>: " << value << "\n"; }
};
// arg.handle是name dependent 的,所以arg.handle直到初始化阶段才被确定
template <typename T> struct parser2 {
  void parse(T arg) { arg.handle(42); } // namespace cvtest::tester
};
template <> struct handler2<int> {
  void handle(int value) {
    std::cout << "handle<int>: " << value << "\n";
  } // namespace cvtest::tester
};
TEST_F(Tester, T_Name_Dependent) {
  handler2<int> h;
  parser2<handler2<int>> p;
  p.parse(h);
}
} // namespace cvtest::tester
