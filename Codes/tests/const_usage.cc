#include "tester.h"
#include "gtest/gtest.h"

/**
 * const的不同用法
 * 1. 修饰普通变量，保护其不被修改
 *        const int a = 10;
 * 2. 修饰引用，不能通过引用修改被引用的对象的值
 *        const int& r = a;
 * 3. 保护实参，保护其不被修改
 *        double test(const AClass& a);
 * 4. 标记成员函数，防止成员函数修改数据成员。
 *    但是如果成员是 mutable的，可以在const函数中被修改
 *        double test() const;
 * 5. 常量对象，只能调用const成员函数
 *        const MyClass obj();  // 调用的是构造函数
 * 6. 修饰返回值，返回的对象只能调用 const 成员函数
 *        const MyClass create();  // 调用的是一个普通成员函数
 * */

namespace cvtest::tester {
TEST_F(Tester, ConstTester) {}
} // namespace cvtest::tester
