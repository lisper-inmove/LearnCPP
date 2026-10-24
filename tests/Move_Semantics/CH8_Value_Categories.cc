#include "tester.h"
#include <iostream>
#include <string>

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
 * */

namespace cvtest::tester {
TEST_F(Tester, MS_Value_Category) {
  std::string s = "Hello";
  std::move(s) = "World";
  std::cout << "Value of s is " << s << "\n";
}
} // namespace cvtest::tester
