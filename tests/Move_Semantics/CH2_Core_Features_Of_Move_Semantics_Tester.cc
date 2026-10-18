/**
 *
 * 2 Core Features of Move Semantics
 *
 * 2.1 Rvalue Reference
 *  1. 一个没有名字的临时对象
 *  2. 和std::move标记的具名对象
 *
 *  std::string s{"Hello World"};
 *  std::string s0{s};      // Error
 *  std::string&& s1 = std::move(s);
 *  std::string&& s2{std::move(s)};
 *  std::string&& s3(std::move(s));
 *
 *  rvalue做函数形参: void foo(std::string&& rv);
 *
 *  std::move 只标记，不做移动操作
 *
 *  std::move 等同于 static_cast<decltype(obj)&&>(obj)
 *
 *  2.3 Move-From Objects
 *    被std::move标记这对象叫Move-From Object
 *    它的值是不确定的，但是它的是状态是确定的
 *
 *  2.4 函数重载
 *    void foo(std::string& arg)
 *      可修改的具名对象
 *
 *    void foo(const std::string& arg): const只是对形参的约束
 *      可修改的具名对象
 *      只读具名对象
 *      没有名字的临时对象
 *      std::move(具名对象)
 *
 *    void foo(std::string&& arg);
 *      无名的临时对象
 *      非const,std::move(具名对象)
 *
 *    void foo(const std::string&& arg)
 *      无名的临时对象
 *      std::move(具名对象)
 *      e.g.
 *        const std::string s{"data"};
 *        foo(std::move(s))
 *
 *  2.5 Passing by Value
 *    void foo(std::string str);
 *    std::string s{"Hello World"};
 *    1. foo(s); s copied to str
 *    2. foo(std::move(s)) s moved to str
 *    3. foo(returnStringByValue()) moved to str
 *
 *  传值的方式优点在于，该函数，可以接收 左值，右值，临时对象
 *  左值: 拷贝一次，移动一次
 *  右值: 移动两次
 *  临时对象: 通常很便宜，甚至是零拷贝
 *
 * */
#include "tester.h"
#include <iostream>
#include <string>

namespace cvtest::tester {
void foo(std::string s) { std::cout << "Value of s " << s << "\n"; }
TEST_F(Tester, MS_Core_Feature) {}
} // namespace cvtest::tester
