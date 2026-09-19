/**
 *
 * Move Semantics
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {

void foo1(const std::string &lr) { std::cout << "copy foo1 called " << lr << "\n"; }
void foo1(std::string &&rv) {
  std::cout << "move foo1 called " << rv << "\n";
  // 如果foo1函数，没有修改rv，实参将不会有改变
  // rv[0] = 'A';
  // rv.clear();
}
void foo2(const std::string &lr) { std::cout << "copy foo2 called " << lr << "\n"; }
void foo3(std::string &lr) { std::cout << "copy foo3 called " << lr << "\n"; }

TEST_F(Tester, MS_Move_Tester) {
  foo1("Hello World");
  std::string s = "Hello World";
  foo1(s);

  // 因为foo3的lr不是const的，所以不能用右值来调用
  foo2("Hello World");
  // foo3("Hello World");

  // std::move 等同于 static_cast的写法
  std::string s0 = "Hello World";
  foo1(static_cast<decltype(s0) &&>(s0));

  std::string s1 = "Hello World";
  foo1(static_cast<std::string &&>(s1));

  // s1仅仅是被标记为数据可以拿走，但是最终还是由调用的函数来决定数据是否
  std::cout << "Value of s1 is " << s1 << "\n";
}

} // namespace cvtest::tester
