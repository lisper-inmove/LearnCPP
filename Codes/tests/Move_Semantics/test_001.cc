/**
 *
 * Move Semantics
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {

template <typename T> class MyVector {
public:
  void push_back(const T &elem) { std::cout << "Copy function called\n"; }
  // void push_back(const T &&elem) { std::cout << "Const move function called\n"; }
};

const std::string getValue() { return "aosetuhsaoteuhsaoetuh"; }

TEST_F(Tester, MS_Coping_As_Fallback_tester) {
  MyVector<std::string> a;
  const std::string b = "Hello World";

  // 移动语义的 push_back函数未实现时，实际被调用的是复制版本
  a.push_back("aeosuth");

  a.push_back(std::move(b));
  a.push_back(getValue());
}

} // namespace cvtest::tester
