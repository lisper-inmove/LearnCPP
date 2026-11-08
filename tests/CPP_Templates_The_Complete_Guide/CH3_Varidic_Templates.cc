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
/**
 * 1. Variadic function templates
 *  1. typename... Args: 模板参数包,template parameter pack
 *  2. Args... args: 函数参数包,function parameter pack
 *  3. min(args...: 参数解包),parameter pack expansion
 * */
/**
 * 特化的两参数必须要在非特化的前面
 * */
template <typename T> T min(T a) { return a; }
template <typename T> T min(T a, T b) { return a > b ? b : a; }
template <typename T, typename... Args> T min(T a, Args... args) {
  return min(a, min(args...));
}
TEST_F(Tester, T_Variadic_Function_Template) {
  min(1, 2, 3, 4, 5);
  min(10);
}

/**
 * 包展开
 * 1. sizeof... 是一个运算符,用于计算 args 的个数,最终得到一个数字
 * 2. sizeof(args)... 得到一串数字,相当于 sizeof(args0), sizeof(args1) ...
 * 3. sizeof...(Args) 和 sizeof...(args)
 * 4. sum(args...) 包展开
 * */
template <typename T, typename... Args> T sum(T a, Args... args) {
  if constexpr (sizeof...(args) == 0) {
    return a;
  } else
    return a + sum(args...);
}

template <typename... Ts, typename... Us>
constexpr auto multipacks(Ts... args1, Us... args2) {
  std::cout << sizeof...(args1) << "," << sizeof...(args2) << "\n";
}

TEST_F(Tester, T_Multi_Packs) {
  multipacks(1, 2, 1.3, 1.4, 1.78);
  multipacks<int, int, int>(3, 4, 910, 1.1, 1.2);
}

/**
 * R1(A...) 理解成一种固定的写法,参考 std::function<ReturnTpye(arg1, arg2, ...)>
 * */
template <typename, typename> struct func_pair;
template <typename R1, typename... A1, typename R2, typename... A2>
struct func_pair<R1(A1...), R2(A2...)> {
  std::function<R1(A1...)> f;
  std::function<R2(A2...)> g;
};
bool twice_as(int a, int b) { return a >= b * 2; }
double sum_and_div(int a, int b, double c) { return (a + b) / c; }
TEST_F(Tester, T_Function_Pair) {
  func_pair<bool(int, int), double(int, int, double)> funcs{twice_as,
                                                            sum_and_div};
  funcs.f(42, 33);
  funcs.g(42, 12, 10.1);
}

/**
 * parameters expansion
 * */

/**
 * T... 包展开
 * outer<int, char>::inner<42, 'a'>;
 * 此时的 inner相当于
 *   template <int arg0, char arg1>
 *   struct inner {};
 * */
template <typename... T> struct outer {
  template <T... args> struct inner {};
};

template <typename... T> struct tag {};
template <typename T, typename U, typename... Args> void tagger() {
  tag<T, U, Args...> t1;
  tag<T, Args..., U> t2;
  tag<Args..., T, U> t3;
  tag<U, T, Args...> t4;
}

/**
 * make_it(42);
 * make_it(42, 'a');
 * */
template <typename... Args> void make_it(Args... args) {}

template <typename T> T step_it(T value) { return value + 1; }
/**
 * (... + args): 一元左折叠,等同于 ((arg0 + arg1) + arg2) + ...
 * (args + ...): 一元右折叠,等同于 arg0 + (arg1 + (arg2 + ...))
 * (0 + ... + args): 支持空包
 *
 * */
template <typename... T> int sum2(T... args) { return (... + args); }
template <typename... T> void do_sums(T... args) {
  auto s1 = sum2(args...);
  auto s2 = sum2(42, args...);
  /**
   * 用每个arg去调用step_it,再用所有的 step_it的结果去调用sum2
   * */
  auto s3 = sum2(step_it(args)...);
}

/**
 * 用于初始化
 * */
template <typename... T> struct sum_wrapper {
  sum_wrapper(T... args) { value = (... + args); }
  std::common_type<T...> value;
};
template <typename... T> void parenthesized(T... args) {
  // std::array<int, 4> {1, 2, 3, 4};
  std::array<std::common_type_t<T...>, sizeof...(T)> arr{args...};

  // value = 1 + 2 + 3 + 4
  sum_wrapper sw1(args...);

  // value = 2 + 3 + 4 + 5
  sum_wrapper sw2(++args...);
}

template <typename... T> void brace_enclosed(T... args) {
  int arr1[sizeof...(args) + 1] = {args...};
  int arr2[sizeof...(args)] = {step_it(args)...};
}

/**
 * 多重继续父类调用
 * */
struct A {};
struct B {};
struct C {};
template <typename... Bases> struct X : public Bases... {
  X(Bases const &...args) : Bases(args)... {}
};

/**
 * using
 * */
struct AA {
  void execute() { std::cout << "AA execute\n"; }
};
struct BB {
  void execute() { std::cout << "BB execute\n"; }
};
struct CC {
  void execute() { std::cout << "CC execute\n"; }
};
template <typename... Bases> struct XX : public Bases... {
  XX(Bases const &...args) : Bases(args)... {}

  using Bases::execute...;
};

/**
 * Lambda Capture
 * */
template <typename... T> void captures(T... args) {
  auto l = [args...] { return sum(step_it(args)...); };
  auto s = l();
}

/**
 * Variadic class template
 * */
template <typename T, typename... Ts> struct tuple {
  tuple(T const &t, Ts const &...ts) : value(t), rest(ts...) {}
  constexpr int size() const { return 1 + rest.size(); }
  T value;
  tuple<Ts...> rest;
};
template <typename T> struct tuple<T> {
  tuple(const T &t) : value(t) {}
  constexpr int size() const { return 1; }
  T value;
};

// template <size_t N, typename... Ts>
// typename nth_type<N, Ts...>::value_type &get(tuple<Ts...> &t);

// nth_type主模板,N继续N-1
template <size_t N, typename T, typename... Ts>
struct nth_type : nth_type<N - 1, Ts...> {
  static_assert(N < sizeof...(Ts) + 1, "Index out of bounds");
};
// 基础模板
template <typename T, typename... Ts> struct nth_type<0, T, Ts...> {
  using value_type = T;
};
// 返回第N个元素的类型
template <size_t N> struct getter {
  template <typename... Ts>
  static typename nth_type<N, Ts...>::value_type &get(tuple<Ts...> &t) {
    return getter<N - 1>::get(t.rest);
  }
};
// 返回第0个元素的类型
template <> struct getter<0> {
  template <typename T, typename... Ts> static T &get(tuple<T, Ts...> &t) {
    return t.value;
  }
};
template <size_t N, typename... Ts>
typename nth_type<N, Ts...>::value_type &get(tuple<Ts...> &t) {
  return getter<N>::get(t);
}

TEST_F(Tester, T_Simple_Tuple) {
  tuple<int> one(42);
  tuple<int, double> two(42, 42.0);
  tuple<int, double, char> three(42, 42.0, 'a');
  std::cout << three.rest.rest.value << "\n";
  std::cout << get<2>(three) << "\n";
}

/**
 * Variadic alias templates
 * */
template <int, int> using int_sum = sum_wrapper<int, int>;

/**
 * Variadic variable templates
 * */
template <int... R> constexpr int Sum = (... + R);

} // namespace cvtest::tester
