/**
 *
 * Chapter one: Function Templates
 *
 * */
#include "tester.h"
#include <cstdarg>
#include <ios>
#include <iostream>
#include <type_traits>
namespace cvtest::tester {

template <typename T> void describe_type() {
  if constexpr (std::is_integral_v<T>) {
    std::cout << "- It is integral type.\n";
  } else if constexpr (std::is_floating_point_v<T>) {
    std::cout << "- It is floating-point type.\n";
  } else {
    std::cout << "- It is neither integral nor floating-point.\n";
  }
  if constexpr (std::is_pointer_v<T>) {
    std::cout << "- It is a pointer.\n";
  }

  if constexpr (std::is_const_v<T>) {
    std::cout << "- It is const qualified.\n";
  }

  std::cout << std::boolalpha;
  std::cout << "- Is T the same as int?" << std::is_same_v<T, int> << "\n";
}

TEST_F(Tester, T_Type_Query) {
  // check if int, char, long and their unsigned variants
  std::cout << "check is integral " << std::is_integral<int>() << "\n";
  // check if float double or long double
  std::cout << "check is float " << std::is_floating_point<float>() << "\n";
  // check if T is a pointer
  std::cout << "check is pointer " << std::is_pointer<int *>() << "\n";
  // check if T is an lvalue or rvalue reference
  std::cout << "check is lvalue or rvalue reference " << std::is_reference<int &>() << "\n";
  std::cout << "check is lvalue or rvalue reference " << std::is_reference<int &&>() << "\n";
  std::cout << "check is const " << std::is_const<const int>() << "\n";
  std::cout << "check is same " << std::is_same<int, int>() << "\n";

  describe_type<int>();
  describe_type<float>();
  describe_type<int *>();
}

/**
 *
 * std::remove_const<T>, std::remove_volatile<T>: Remove const or volatile qualifiers.
 * std::remove_reference<T> remove reference qualifers.
 * std::add_const<T> add a const qualifier.
 * std::add_pointer<T> turn a type into a pointer type.
 * std::decay<T> transform a type as if it were passed to a function by value, removing reference,
 * cv-qualifiers, and converting array and function types to pointers
 *
 * */
template <typename T> void show_type_transformations() {
  using NoRef = std::remove_reference_t<T>;
  using NoConst = std::remove_const_t<NoRef>;
  using Ptr = std::remove_pointer_t<NoConst>;
  using Decayed = std::decay_t<T>;
  std::cout << "Original type:" << typeid(T).name() << "\n";
  std::cout << "After removing reference:" << typeid(NoRef).name() << "\n";
  std::cout << "After removivg const:" << typeid(NoConst).name() << "\n";
  std::cout << "Pointer to no-const-no-ref:" << typeid(Ptr).name() << "\n";
  std::cout << "Decayed type:" << typeid(Decayed).name() << "\n";
  std::cout << "-----------------------------------------\n";
}

TEST_F(Tester, T_Type_Transformation) {
  show_type_transformations<int &>();
  show_type_transformations<const double &>();
  show_type_transformations<int[5]>();
}

/**
 * constexpr，condition compilation
 * */
template <typename T> T multiply_by_two(T value) {
  if constexpr (std::is_integral_v<T>) {
    return value << 1;
  } else {
    return value * 2;
  }
}

// C++20's concepts simplify this further'
// c++17 使用 constexpr更直观
template <typename T> std::enable_if_t<std::is_integral_v<T>, void> process(T value) {
  std::cout << "is integral " << std::is_integral_v<T> << " value " << value;
}
template <typename T> std::enable_if_t<!std::is_integral_v<T>, void> process(T value) {
  std::cout << "is not integral " << std::is_integral_v<T> << " value " << value;
}

// C++20, concepts
template <std::integral T> void process(T value) {
  std::cout << "is integral " << std::is_integral_v<T> << " value " << value;
}
template <typename T>
  requires(!std::integral<T>)
void process(T value) {
  std::cout << "is not integral " << std::is_integral_v<T> << " value " << value;
}

/**
 * 模板特化
 * The core technique behind most custom type traits is template specialization
 * ofen combined with SFINAE(Substitution Failure Is Not An Error)
 * SFINA: the compiler's way of silently discarding invalid template instantiations during overload
 * resolution
 *
 * this example demonstrates a fundamental pattern: providing a primary template for the default
 * case, and specializations for cases where the trait should yield true.
 * */
template <typename T> struct is_pointer_custom : std::false_type {};
template <typename T> struct is_pointer_custom<T *> : std::true_type {};

TEST_F(Tester, T_Custom_Is_Pointer_Tester) {
  std::cout << std::boolalpha;
  std::cout << "int is pointer?" << std::is_pointer<int>::value << "\n";
  std::cout << "int* is pointer?" << std::is_pointer<int *>::value << "\n";
}

/**
 * Detecting Memebr function
 * Before c++17, using decltype
 *
 * std::void_t是C++17引入的工具，近似定义为
 * template <typename ...>
 * using void_t = void;
 * 它把任意类型都映射为void。它不关心参数是什么，在这个例子中，主要用于触发SFINAE。
 * 特化版本中，如果void_t中的内容合法，则特化成功，否则触发SFINAE
 *
 * decltype(expr) 是关键字，用于在编译期获取表达式 expr 的类型，不会真正求值。
 * declval: 返回一个T&&，不会真正构造对象。
 * */

template <typename, typename = std::void_t<>> struct has_serialize : std::false_type {};
template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>>
    : std::is_same<void, decltype(std::declval<T>().serialize())> {};

/**
 * C++17，由于 variable templates，可以像下面这样定义，简化使用
 * if constexpr (has_serialize_v<T>) {}
 * */
template <typename T> constexpr bool has_serialize_v = has_serialize<T>::value;

/**
 *
 * Detection Idiom
 *
 * Op: 本身是一个模板，它接受任意数量的模板
 *  template <typename...> 表示任意参数
 *  typename Op（也写做 class Op）：表示Op是一个模板模板参数
 *
 * typename... Args：可变类型参数包，可以是0个，1个，任意个类型
 *
 * AlwaysVoid: 总是void，在偏特化中，它会被替换成 std::void_t<Op<Args...>>
 *
 * */

template <typename Default, typename AlwaysVoid, template <typename...> typename Op,
          typename... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

/**
 * std::void_t<Op<Args...>> 在Op<Args...>合法时，偏特化成功
 * */
template <typename Default, template <typename...> typename Op, typename... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

// 用来判断是否存在
template <template <typename...> typename Op, typename... Args>
using is_detected = typename detector<void, void, Op, Args...>::value_t;

// 获取检测到的类型
template <template <typename...> typename Op, typename... Args>
using detected_t = typename detector<void, void, Op, Args...>::type;

// 在失败时返回自定义默认类型
template <typename Default, template <typename...> typename Op, typename... Args>
using detected_or = detector<Default, void, Op, Args...>;

/**
 * C++20 引入 Concepts
 * Concepts（概念）是 C++20 引入的对模板参数进行约束的机制。它把以前靠 SFINAE / enable_if
 * 写出的"天书"式代码，变成了可读、可复用、可组合的约束。下面从基础到进阶系统讲一遍。
 * */
template <typename T>
concept HasSize = requires(T a) {
  { a.size() };
};
template <typename T> void print_size_if_possible(const T &container) {
  if constexpr (HasSize<T>) {
    std::cout << "Size is: " << container.size() << "\n";
  } else {
    std::cout << "No size() member found;\n";
  }
}

/**
 * Detecting Type Relationships and Conversions
 * std::is_base_of<Base, Derived>: check if Derived inherits from Base.
 * std::is_convertible<From, To>: check if From can be implicitly converted to To
 * std::is_constructible<T, Args...>: check if T is constructible from Args...
 * */
template <typename Base, typename Derived> void check_inheritance() {
  if constexpr (std::is_base_of_v<Base, Derived>) {
    std::cout << "Derived inherits from Base\n";
  } else {
    std::cout << "No inheritance relationships\n";
  }
}

/**
 * Variadic Templates
 * */
int min(int count, ...) {
  va_list args;
  va_start(args, count);
  int val = va_arg(args, int);
  for (int i = 1; i < count; i++) {
    int n = va_arg(args, int);
    if (n < val)
      val = n;
  }
  va_end(args);
  return val;
}

template <typename T> T min(int count, ...) {
  va_list args;
  va_start(args, count);
  T val = va_arg(args, T);
  for (int i = 1; i < count; i++) {
    T n = va_arg(args, T);
    if (n < val)
      val = n;
  }
  va_end(args);
  return val;
}

/**
 * 三处 ...
 * 1. typename... Args: template parameter pack
 * 2. Args... args: function parameter pack
 * 3. min(args...): parameter pack expansion
 * */
template <typename T> T min(T a, T b) { return a < b ? a : b; }
template <typename T, typename... Args> T min(T a, Args... args) { return min(a, min(args...)); }
TEST_F(Tester, T_Variadic_Templates_Tester) {
  // 一共会生成 6,5,4,3,2 这么多个参数的函数
  min<int>(1, 2, 3, 4, 5, 7);
}

} // namespace cvtest::tester
