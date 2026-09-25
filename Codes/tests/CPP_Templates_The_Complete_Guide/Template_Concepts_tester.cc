/**
 *
 * C++ 20 Concepts
 *
 * */
#include "tester.h"
#include <cmath>
#include <concepts>
#include <iostream>
#include <type_traits>
namespace cvtest::tester {
TEST_F(Tester, T_Concepts_Tester) { std::cout << "C++ 20 Concepts\n"; }

/**
 * requires 有两种
 * 1. requires 子句
 * 2. requires 表达式
 * */

/**
 * requires 子句
 * 1. 出现在模板参数列表之后（函数与类均可）
 * 2. 函数声明的末尾（普通函数或者成员函数）
 */
template <typename T>
  requires std::integral<T>
T add(T a, T b) {
  return a + b;
}

template <typename T>
T square(T x)
  requires std::integral<T> || std::floating_point<T>
{
  return x * x;
}

/**
 * requires 表达式
 *  1. requires (参数列表) {要求序列}
 *  2. 参数列表可省略
 *  3. 不产生运行时代码，不构造对象，不调用函数，只做声明，表达式的类型层面检查
 * */
template <typename T>
concept HasIntSize = requires(const T &t) {
  { t.size() } -> std::convertible_to<std::size_t>;
};

/**
 * 1. 要求序列为 简单表达式
 * */
template <typename T>
concept Addable = requires(T a, T b) {
  a + b;
  a += b;
  a - b;
};

/**
 * 2. 要求序列 类型要求
 *  以typename开头，检查某个类型是否存在
 * */
template <typename T>
concept HasValueType = requires {
  typename T::value_type;
  typename T::iterator;
};

// 检查类型T是否具有一个名为rebind的嵌套模板，并且该模板能够用类型参数int进行实例化
// 且实例化结果是一个类型
template <typename T>
concept HasTemplateParam = requires { typename T::template rebind<int>; };
struct Good {
  template <typename U> using rebind = Good;
};
struct Bad {};

/**
 * 3. 复合要求
 *  同是检查多种条件
 *  表达式合法 + 是否noexcept + 返回值是否满足约束
 * */
template <typename T>
concept GoodSize = requires(const T &t) {
  { t.size() } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept NoThrowSwap = requires(T a, T b) {
  { swap(a, b) } noexcept;
};

template <typename T>
concept IntSize = requires(const T &t) {
  { t.size() } noexcept -> std::same_as<int>;
};

/**
 * 4. 嵌套要求
 * */
template <typename T>
concept Sized = requires(T t) {
  t.size();
  requires std::integral<decltype(t.size())>;
};

/**
 * requires 表达式+ requires 子句
 *  第一个requires是子句，后面接一个约束
 *  第二个requires是表达式，产生一个约束
 * */
template <typename T>
  requires requires(T t) { t.size(); }
void f3(T &t);

/**
 * 表达式短路
 *  按顺序检查，遇到失败立即停止，返回false
 * */
template <typename T>
concept HasFrontAndSize = requires(T t) {
  requires std::ranges::range<T>;
  t.front();
  t.size();
};

// -----------------------------------------------------------------------------------

// concepts 四种定义方式

// 简单定义
template <typename T>
concept Integral = std::is_integral_v<T>;

// requires (最常用)
template <typename T>
concept HasSize = requires(T a) {
  { a.size() } -> std::convertible_to<std::size_t>;
};

// 组合 concepts
template <typename T>
concept SizedContainer = requires(T a) {
  a.size();
  a.begin();
  a.end();
};

template <typename T>
concept PrintableSized = SizedContainer<T> && std::integral<T>;

// 直接约束类型参数
template <typename T>
  requires std::integral<T> || std::floating_point<T>
T add(T a, T b) {
  return a + b;
}

// 四种使用位置
template <std::integral t> t gcd(t a, t b) { return a + b; }

template <typename T>
  requires std::integral<T>
void f0(T x) {
  std::cout << x << "\n";
}

void f1(std::integral auto x) { std::cout << x << "\n"; }

struct Widget {
  template <std::integral T> void process(T x);
};

// 重载决议: 约束更严格的版本会被优先选择
// 如果一个约束蕴含另一个 (A => B)，则A比B更特化，更优先
template <typename T>
  requires std::integral<T>
void f2(T) {
  std::cout << "integral \n";
}

template <typename T>
  requires std::floating_point<T>
void f2(T) {
  std::cout << "floating\n";
}

template <typename T> void f2(T) { std::cout << "generic\n"; }

TEST_F(Tester, T_Concepts_Overloading_Tester) {
  f2(1);    // integral
  f2(1.0);  // floating
  f2("hi"); // generic
}

} // namespace cvtest::tester
