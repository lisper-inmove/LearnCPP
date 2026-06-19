#include "tester.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

/**
 * C++ Template
 * Pros and const
 *
 * Let's start by pointing out the advantages:
 *  1. Templates help us avoid writing repetitive code.
 *  2. Templates foster the creation of generic libraries providing algorithms
 * and types, such as the standard C++ library (sometimes incorrectly referred
 * to as the STL), which can be used in many applications, regardless of their
 * type.
 *  3. The use of templates can result in less and better code. For instance,
 * using algorithms from the standard library can help write less code that is
 * likely easier to understand and maintain and also probably more robust
 * because of the effort put into the development and testing of these
 * algorithms.
 *
 *  When it comes to disadvantages, the following are worth mentioning:
 *
 *  1. The syntax is considered complex and cumbersome, although with a little
 * practice this should not really pose a real hurdle in the development and use
 * of templates.
 *  2. Compiler errors related to template code can often be long and cryptic,
 * making it very hard to identify their cause. Newer versions of the C++
 * compilers have made progress in simplifying these kinds of errors, although
 * they generally remain an important issue. The inclusion of concepts in the
 * C++20 standard has been seen as an attempt, among others, to help provide
 * better diagnostics for compiling errors.
 *  3. They increase the compilation times because they are implemented entirely
 * in headers. Whenever a change to a template is made, all the translation
 * units in which that header is included must be recompiled.
 *  4. Template libraries are provided as a collection of one or more headers
 * that must be compiled together with the code that uses them.
 *
 * */

namespace cvtest::tester {
TEST_F(Tester, TemplateTester) { std::cout << "C++ Template tester" << "\n"; }

// -------------------------------------------------------------------------
// 函数模板 Function Template
template <typename T> T MyMax(T const a, T const b) { return a > b ? a : b; }

// 类模板 class Template
template <typename T> class MyInt {
public:
  MyInt(T value) : value_(value) {}
  [[nodiscard]] T get() const { return value_; }

private:
  T value_;
};

// 变量模板 Variable Template
template <typename T> constexpr T NewLine = T('\n');

// Alias Template

// -------------------------------------------------------------------------
/**
 * 1. Type template parameters: template <typename T>
 * 2. Non-type template parameters:
 *  template <size_t N>
 *  template <auto N>
 * 3. Template template parameters
 *  template <typename K, template V, template<typename> typename C>
 * */

TEST_F(Tester, TemplateBaseTester) {
  const int a = 10, b = 20;
  auto maxValue = MyMax(a, b);
  std::cout << "Max value fo " << a << " and " << b << " is " << maxValue
            << "\n";

  MyInt<int32_t> intValue{10};
  std::cout << "value of intValue is " << intValue.get() << "\n";

  std::wstring test = L"Demo";
  test += NewLine<wchar_t>;
  std::wcout << test;
}

// --------------------------------------------------------------------------
template <typename T, size_t S> class Buffer {
private:
  T data_[S];

public:
  constexpr T const *data() const { return data_; }
  constexpr T &operator[](size_t const index) { return data_[index]; }
  constexpr T const &operator[](size_t const index) const {
    return data_[index];
  }
};

// ------------------
class Device {
public:
  virtual ~Device() = default;
  virtual void output() = 0;
};

template <void (*action)()> class SmartDevice : public Device {
public:
  void output() override { (*action)(); }
};

class HelloCommand {
public:
  void action() { std::cout << "Action in Command" << "\n"; }
};

// 第二个参数 template需要的是从 Command里的一个 无参无返回值的成员函数
template <typename Command, void (Command::*action)()>
class MiniDevice : public Device {
public:
  MiniDevice(Command &command) : cmd(command) {}

  void output() override { (cmd.*action)(); }

private:
  Command &cmd;
};

void say_hello_in_english() { std::cout << "Hello World\n"; }
void say_hello_in_spanish() { std::cout << "Hola Mundo!\n"; }

TEST_F(Tester, NonTypeTemplateTester) {
  Buffer<int, 10> b1;
  Buffer<int, 2 * 5> b2;
  static_assert(std::is_same_v<decltype(b1), decltype(b2)>);

  // w1 w2 虽然都是 SmartDevice，但是它们类型并不相同
  auto w1 = std::make_unique<SmartDevice<&say_hello_in_english>>();
  auto w2 = std::make_unique<SmartDevice<&say_hello_in_spanish>>();
  static_assert(!std::is_same_v<decltype(w1), decltype(w2)>);
  w1->output();
  w2->output();

  // w3 w4 都是 Device
  std::unique_ptr<Device> w3 =
      std::make_unique<SmartDevice<&say_hello_in_english>>();
  std::unique_ptr<Device> w4 =
      std::make_unique<SmartDevice<&say_hello_in_spanish>>();
  static_assert(std::is_same_v<decltype(w3), decltype(w4)>);
  w3->output();
  w4->output();

  HelloCommand helloCmd;
  auto w5 = std::make_unique<MiniDevice<HelloCommand, &HelloCommand::action>>(
      helloCmd);
  w5->output();
}

// -----------------------------------------------
// 用模板做为模板的参数

template <typename T> class SimpleWrapper {
public:
  T value;
};

template <typename T> class FancyWrapper {
public:
  FancyWrapper(T const v) : value(v) {}
  [[nodiscard]] T const &get() const { return value; }
  template <typename U> U as() const { return static_cast<U>(value); }

private:
  T value;
};

template <typename T, typename U, template <typename> typename W = FancyWrapper>
class WrappingPair {
public:
  WrappingPair(T const a, U const b) : item1(a), item2(b) {}
  W<T> item1;
  W<U> item2;
};

TEST_F(Tester, TemplateTemplateTester) {
  WrappingPair<int, double> p1(42, 42.0);
  std::cout << p1.item1.get() << " " << p1.item2.get() << "\n";

  WrappingPair<int, double, SimpleWrapper> p2(42, 42.0);
  std::cout << p2.item1.value << " " << p2.item2.value << "\n";
}

// ---------------------------------------------
// 模板也可以有默认参数

template <typename T, typename U = double> class MyFoo {};

// 模板类: 不能说前面的 typename 有默认值，后面的没有
// 模板函数可以
// template <typename T = int, typename U> class MyBar {}; // Error
template <typename T = int, typename U> void foo() {} // OK

} // namespace cvtest::tester
