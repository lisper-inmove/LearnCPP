#include "tester.h"
#include <iostream>
#include <string>
#include <type_traits>

/**
 * C++11 之后一共有5种值类型
 *
 *                  expression
 *                 /         \
 *              glvalue     rvalue
 *              /       \/     \
 *            lvalue   xvalue  prvalue
 *
 * 1. glvalue: 广义左值。有身份的变量（可取地址）
 *  long-living objects for functions
 * 2. rvalue: 右值。可移动
 * 3. lvalue: 左值。有身份，不可移动
 * 4. xvalue: 将亡值。有身份，可移动
 *  long-living object, but its resources/values are no longer needed
 * 5. prvalue: 纯右值。无身份，可移动
 *  short-living values for initializations
 *
 * 1. 所有具名变量都是左值
 * 2. 字面字符串是左值
 * 3. 非字符串的字面值是纯右值
 * 4. 所有临时变量都是纯右值
 * 5. 所有被std::move标记的都是将亡值
 *
 * 8.2 Special Rules for Value Categories
 *
 * 8.2.1 Value Category of Functions
 *  1. 对function引用一定是lvalue
 *
 * 8.2.2 Value Category of Data Members
 * 1. lvalue 的成员是lvalue
 * 2. rvalue 对象 成员的引用 和 static成员 都是lvalue
 * 3. rvalue的字面成员是 xvalue
 *
 *  std::pair<std::string, std::string&> foo();
 *  std::vector<std::string> coll;
 *  coll.push_back(foo().first);  // first 是一个xvalue
 *  coll.push_back(foo().second); // second 是一个 lvalue
 *
 *  普通成员变量，以下使用方式是等同的，都会触发移动语义
 *  coll.push_back(std::move(sp.first)); // move
 *  coll.push_back(std::move(sp).first); // move
 *
 *  如果是引用或者Static
 *  coll.push_back(std::move(sp.first)); // move
 *  coll.push_back(std::move(sp).first); // copy
 *
 *  移动一个引用或者static成员，通常不是好的做法
 *
 *  如果是template，在不知道 first是不是 reference 或者 static时，使用以下方式更安全
 *  coll.push_back(std::move(sp).first); // copy
 *
 *
 * 8.3.1 Overload Resolution with RValue Reference
 * class X {...};
 * X v{...};
 * const X c{...};
 *
 * void f(cost X&);
 * void f(X&);
 * void f(X&&);
 * void f(const X&&);
 * Call        f(X&)     f(const X&)   f(X&&)     f(const X&&)
 * f(v)          1         2             no        no
 * f(c)          no        1             no        no
 * f(X{})        no        3             1         2
 * f(move(v))    no        3             1         2
 * f(move(c))    no        2             no        1
 *
 * check whether expr is prvalue: !std::is_reference_v<decltype((expr))>
 * check whether expr is lvalue: std::is_lvalue_reference_v<decltype((expr))>
 * check whether expr is xvalue: std::is_rvalue_reference_v<decltype((expr))>
 * check whether expr is rvalue: !std::is_lvalue_reference_v<decltype((expr))>
 *
 * */

namespace cvtest::tester {

void f(int) {}
std::string createString() { return "Hello World!"; }

void foo(std::string &&value) {
  std::cout << "value of value is " << value << "\n";
}

void foo2(std::string &&value, bool tryAgain = true) {
  std::cout << "value of value is " << value << "\n";
  if (tryAgain) {
    // 调用foo2函数时，实参value是 rvalue reference
    // 但是foo2函数内部，value这个形参是具名变量，是一个lvalue reference
    // foo2(value);

    foo2(static_cast<std::string &&>(value));
  }
}

TEST_F(Tester, MS_Value_Category) {
  std::string s = "Hello";
  std::move(s) = "World";
  std::cout << "Value of s is " << s << "\n";

  /**
   * fref1, fref2, ar 都是lvlaue
   * */
  void (&fref1)(int) = f;
  void (&&fref2)(int) = f;
  // ar is type of void(&)(int)
  auto &ar = std::move(f);

  const std::string &r1{createString()};
  // std::string &r2{createString()}; // Error

  // "Hello" 本身是一个左值引用，但是它的构造函数会和它创建一个临时对象，从而可以调用foo函数
  foo("Hello");
}

void foo3(std::string &&str) {
  std::cout << std::boolalpha;
  std::cout << "Output for foo3 \n";
  std::cout << "is string? " << std::is_same<decltype(str), std::string>::value
            << "\n";
  std::cout << "is string? "
            << std::is_same_v<decltype(str), std::string> << "\n";
  std::cout << "is string&? "
            << std::is_same_v<decltype(str), std::string &> << "\n";
  std::cout << "is string&&? "
            << std::is_same_v<decltype(str), std::string &&> << "\n";

  std::cout << "is string ref? " << std::is_reference_v<decltype(str)> << "\n";
  std::cout << "is string lvalue ref? "
            << std::is_lvalue_reference_v<decltype(str)> << "\n";
  std::cout << "is string rvalue ref? "
            << std::is_rvalue_reference_v<decltype(str)> << "\n";

  std::remove_reference_t<decltype(str)> tmp;
  std::cout << "is reference? " << std::is_reference_v<decltype(tmp)> << "\n";
}

void foo4(std::string &&str) {
  // (str) 是一个表达式，表达才有值类型
  std::cout << std::boolalpha;
  std::cout << "Output for foo4 \n";
  std::cout << "is string? "
            << std::is_same<decltype((str)), std::string>::value << "\n";
  std::cout << "is string? "
            << std::is_same_v<decltype((str)), std::string> << "\n";
  std::cout << "is string&? "
            << std::is_same_v<decltype((str)), std::string &> << "\n";
  std::cout << "is string&&? "
            << std::is_same_v<decltype((str)), std::string &&> << "\n";

  std::cout << "is string ref? "
            << std::is_reference_v<decltype((str))> << "\n";
  std::cout << "is string lvalue ref? "
            << std::is_lvalue_reference_v<decltype((str))> << "\n";
  std::cout << "is string rvalue ref? "
            << std::is_rvalue_reference_v<decltype((str))> << "\n";

  std::remove_reference_t<decltype(str)> tmp;
  std::cout << "is reference? " << std::is_reference_v<decltype((tmp))> << "\n";
}

TEST_F(Tester, MS_Check_Type_and_Value_Category) {
  foo3("Hello");
  foo4("Hello");
}
} // namespace cvtest::tester
