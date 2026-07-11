# C++17 ~ C++23 关键字详细清单

> 本文档列出 C++17 到 C++23 标准中所有支持的关键字，废弃（deprecated）或已移除的关键字不在此列。每个关键字附有详细用法说明和代码示例。

---

## 目录

- [1. 基本类型关键字](#1-基本类型关键字)
- [2. 类型修饰符](#2-类型修饰符)
- [3. 存储类说明符](#3-存储类说明符)
- [4. 控制流关键字](#4-控制流关键字)
- [5. 类和对象相关](#5-类和对象相关)
- [6. 命名空间和作用域](#6-命名空间和作用域)
- [7. 类型转换](#7-类型转换)
- [8. 异常处理](#8-异常处理)
- [9. 模板和泛型编程](#9-模板和泛型编程)
- [10. 并发编程](#10-并发编程)
- [11. 内存管理](#11-内存管理)
- [12. 类型推导与属性](#12-类型推导与属性)
- [13. C++20 新增关键字](#13-c20-新增关键字)
- [14. C++23 新增关键字](#14-c23-新增关键字)
- [15. 其他关键字](#15-其他关键字)
- [16. 替代标记](#16-替代标记)

---

## 1. 基本类型关键字

### 1.1 `void`

**用途：** 表示空类型或无返回值。

```cpp
// 函数无返回值
void printMessage() {
    std::cout << "Hello, World!" << std::endl;
}

// 函数无参数 (C风格，C++中可省略)
void foo(void);

// void* 泛型指针，可指向任意类型
int x = 42;
void* ptr = &x;
int* intPtr = static_cast<int*>(ptr);  // 使用时需转换回原类型
std::cout << *intPtr << std::endl;      // 42
```

### 1.2 `bool`

**用途：** 布尔类型，取值 `true` 或 `false`。

```cpp
bool flag = true;
bool isReady = false;

// 隐式转换为int: true→1, false→0
int a = flag;   // 1
int b = isReady; // 0

// 条件判断
if (flag) { /* ... */ }
```

### 1.3 `char` / `wchar_t` / `char8_t` / `char16_t` / `char32_t`

**用途：** 字符类型，不同宽度。

- `char`：至少 8 位（通常 8 位）
- `wchar_t`：宽字符，大小由实现定义
- `char8_t`（C++20）：UTF-8 字符
- `char16_t`（C++11）：UTF-16 字符
- `char32_t`（C++11）：UTF-32 字符

```cpp
char c = 'A';

// C++20: char8_t 用于 UTF-8
char8_t u8c = u8'中';  // UTF-8 字符字面量

// C++11: char16_t 和 char32_t
char16_t u16c = u'文';  // UTF-16
char32_t u32c = U'字';  // UTF-32

wchar_t wc = L'宽';

// 字符串字面量
const char8_t* u8str = u8"UTF-8 字符串";    // C++20
const char16_t* u16str = u"UTF-16 字符串";
const char32_t* u32str = U"UTF-32 字符串";
const wchar_t* wstr = L"宽字符串";
```

### 1.4 `short` / `int` / `long` / `long long`

**用途：** 整数类型。

```cpp
short s = 32767;               // 至少 16 位
int i = 42;                     // 至少 16 位（通常 32 位）
long l = 123456789L;            // 至少 32 位
long long ll = 9876543210LL;    // 至少 64 位

// unsigned 变体
unsigned int ui = 100U;
unsigned long long ull = 100ULL;

// 确定宽度的整数类型（推荐用于跨平台）
#include <cstdint>
int8_t   i8  = 127;
int16_t  i16 = 32767;
int32_t  i32 = 2147483647;
int64_t  i64 = 9223372036854775807LL;
uint64_t u64 = 18446744073709551615ULL;
```

### 1.5 `float` / `double`

**用途：** 浮点数类型。

```cpp
float  f = 3.14f;      // 单精度，约 7 位有效数字
double d = 3.1415926;   // 双精度，约 15 位有效数字

// 字面量后缀
double d2 = 1.0;        // double
float  f2 = 1.0f;       // float
long double ld = 1.0L;  // long double
```

### 1.6 `signed` / `unsigned`

**用途：** 指定整数类型有无符号。

```cpp
signed int si = -10;       // 有符号整数（signed int 等同于 int）
unsigned int ui = 10;      // 无符号整数，只能为非负数

unsigned char uc = 255;    // 范围 0~255
signed char sc = -128;     // 范围 -128~127

// 注意：char 是否有符号由实现决定
```

---

## 2. 类型修饰符

### 2.1 `const`

**用途：** 声明常量，表示变量不可修改。

```cpp
// 常量变量：必须在声明时初始化
const int MAX_SIZE = 100;
// MAX_SIZE = 200;  // 编译错误！

// 指向常量的指针：不能通过该指针修改值
int x = 10;
const int* p1 = &x;
// *p1 = 20;  // 编译错误！
x = 20;       // OK，x 本身不是 const
p1 = nullptr; // OK，可以改变 p1 本身的指向

// 常量指针：指针本身不可变
int y = 5;
int* const p2 = &y;
*p2 = 30;     // OK，可以修改 y 的值
// p2 = &x;   // 编译错误！指针不可变

// 指向常量的常量指针
const int* const p3 = &x;  // 既不能改指针，也不能通过指针改值

// const 成员函数：承诺不修改成员变量
class MyClass {
    int value;
public:
    int getValue() const { return value; }  // 不能修改任何成员
    void setValue(int v) { value = v; }     // 可以修改成员
};
```

### 2.2 `volatile`

**用途：** 告诉编译器变量可能被外部因素修改（如硬件寄存器、信号处理函数），禁止编译器优化对该变量的访问。

```cpp
// 用于内存映射 I/O，阻止编译器缓存或消除看似无用的读取
volatile int hardwareFlag = 0;

// 循环读取硬件寄存器，编译器不会优化掉这个循环
while (hardwareFlag == 0) {
    // 等待硬件信号
}

// volatile 指针
volatile int* const statusReg = reinterpret_cast<volatile int*>(0x40001000);
int currentStatus = *statusReg;  // 每次都从硬件地址读取
```

### 2.3 `mutable`

**用途：** 允许在 `const` 成员函数中修改被 `mutable` 修饰的成员变量。

```cpp
class Cache {
    mutable int cacheHits = 0;  // 即使是 const 对象也能修改
    mutable std::string lastAccess;
public:
    int getData() const {
        ++cacheHits;                   // 合法！const 成员函数中修改 mutable 成员
        lastAccess = "getData called";  // 合法！
        return 42;
    }

    int getHits() const { return cacheHits; }
};

Cache c;
c.getData();
c.getData();
std::cout << c.getHits() << std::endl;  // 2
```

### 2.4 `constexpr`

**用途：** 声明可在编译期求值的变量或函数（C++11引入，C++14/C++17/C++20不断增强）。

```cpp
// constexpr 变量：值在编译期确定
constexpr int SIZE = 100;
constexpr double PI = 3.14159;

// constexpr 函数：可在编译期计算
constexpr int square(int n) {
    return n * n;
}
constexpr int result = square(10);  // 编译期计算，result = 100

// C++14: 放宽限制，constexpr 函数中可使用更多控制流
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; ++i)
        result *= i;
    return result;
}
constexpr int fact5 = factorial(5);  // 编译期计算 = 120

// C++17: constexpr lambda
auto lambda = [](int n) constexpr { return n * n; };
constexpr int val = lambda(5);  // 25

// C++20: constexpr 虚函数、constexpr dynamic_cast
// C++20: constexpr string/vector
#include <vector>
#include <string>
constexpr auto vec = [] {
    std::vector<int> v{1, 2, 3};
    v.push_back(4);
    return v.size();  // 4
}();

constexpr auto str = [] {
    std::string s = "Hello";
    s += " World";
    return s.size();  // 11
}();
```

### 2.5 `consteval`

**用途：**（C++20）声明**必须在编译期求值**的函数。与 `constexpr` 不同，`consteval` 函数不允许在运行期调用。

```cpp
// consteval 函数生成编译时常量
consteval int compileTimeSquare(int n) {
    return n * n;
}

constexpr int a = compileTimeSquare(10);  // OK，编译期计算 = 100

int runtimeVar = 5;
// int b = compileTimeSquare(runtimeVar);  // 编译错误！必须在编译期求值

// 典型用途：编译期验证
consteval bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}
static_assert(isPowerOfTwo(64));   // OK
// static_assert(isPowerOfTwo(63)); // 编译错误
```

### 2.6 `constinit`

**用途：**（C++20）声明变量必须通过常量初始化进行**静态初始化**，防止运行时初始化的顺序问题。

```cpp
// constinit 确保变量在编译期/链接期完成初始化
// 变量仍然可以修改，只是初始化必须是编译期

constexpr int initialValue() { return 42; }

constinit int globalVar = initialValue();  // OK，编译期初始化
// constinit int badVar = rand();           // 编译错误！rand() 不是常量表达式

// 解决 Static Initialization Order Fiasco
// thread_local 变量也可以用 constinit
constinit thread_local int counter = 0;

void increment() {
    ++counter;  // 可以修改
}
```

---

## 3. 存储类说明符

### 3.1 `static`

**用途：** 多重含义，取决于上下文。

```cpp
// 1. 函数内静态局部变量：生命周期贯穿程序，首次调用时初始化
int getNextId() {
    static int id = 0;
    return ++id;
}
int a = getNextId();  // 1
int b = getNextId();  // 2

// 2. 类静态成员：属于类本身而非实例
class Counter {
    static int count;  // 声明
public:
    Counter() { ++count; }
    static int getCount() { return count; }  // 静态成员函数
};
int Counter::count = 0;  // 类外定义

// 3. 文件作用域（内部链接）：变量/函数仅本翻译单元可见
static int internalVar = 100;  // 不能被其他 .cpp 文件 extern 引用
static void helperFunc() {}     // 同样内部链接

// C++17: 内联静态成员，可在类内定义
class Config {
public:
    inline static int timeout = 3000;  // C++17，无需类外定义
};
```

### 3.2 `extern`

**用途：** 声明变量或函数在其他翻译单元中定义。

```cpp
// ---- file1.cpp ----
int globalVar = 42;
void sharedFunction() { /* ... */ }

// ---- file2.cpp ----
extern int globalVar;           // 声明，定义在 file1.cpp
extern void sharedFunction();   // 声明，定义在 file1.cpp

// extern "C"：C 语言链接，用于 C/C++ 互操作
extern "C" {
    #include "some_clib.h"      // C 头文件
}

extern "C" void legacyFunction(int);  // C 链接函数

// extern template：显式实例化声明（减少编译时间）
// 在头文件中：
extern template class std::vector<int>;  // 告诉编译器不要在此处实例化
```

### 3.3 `thread_local`

**用途：**（C++11）每个线程拥有独立的变量副本。

```cpp
thread_local int threadId = 0;
thread_local std::string threadName = "worker";

// 典型用法：线程安全的随机数生成器
#include <random>
thread_local std::mt19937 rng(std::random_device{}());

int getRandomInt(int min, int max) {
    return std::uniform_int_distribution<int>{min, max}(rng);
}

// 类的 thread_local 静态成员
class ThreadPool {
    inline static thread_local int taskCount = 0;  // C++17
};
```

### 3.4 `register`

**用途：**（C++17起弃用，C++17仍可使用但保留关键字）提示编译器将变量放在寄存器中。现代编译器会忽略此提示。**C++17 中仍保留为关键字但无实际作用。**

```cpp
// C++17 中可以写但不推荐，编译器会忽略
// register int counter = 0;  // 无实际效果，仅历史遗留
```

### 3.5 `mutable`

见 [2.3 `mutable`](#23-mutable)，属于存储类说明符，允许 const 成员函数修改该成员。

---

## 4. 控制流关键字

### 4.1 `if` / `else`

```cpp
int x = 10;
if (x > 5) {
    std::cout << "x > 5" << std::endl;
} else if (x > 0) {
    std::cout << "0 < x <= 5" << std::endl;
} else {
    std::cout << "x <= 0" << std::endl;
}

// C++17: if 语句中带有初始化器
if (auto it = map.find(key); it != map.end()) {
    std::cout << "Found: " << it->second << std::endl;
} else {
    std::cout << "Not found" << std::endl;
}  // it 的作用域仅限于 if/else 块

// C++17: constexpr if（编译期条件分支）
template<typename T>
auto getValue(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t;      // 只有 T 是指针时才编译此分支
    } else {
        return t;
    }
}
```

### 4.2 `switch` / `case` / `default` / `break`

```cpp
int day = 3;
switch (day) {
    case 1:
        std::cout << "Monday" << std::endl;
        break;
    case 2:
        std::cout << "Tuesday" << std::endl;
        break;
    default:
        std::cout << "Other day" << std::endl;
        break;  // 最后的 break 可选但推荐
}

// C++17: switch 带初始化器
switch (int code = getErrorCode(); code) {
    case 0:
        std::cout << "Success" << std::endl;
        break;
    default:
        std::cout << "Error: " << code << std::endl;
}
// code 作用域结束

// [[fallthrough]] 标注故意穿透
switch (int value = 1; value) {
    case 1:
        std::cout << "Step 1" << std::endl;
        [[fallthrough]];  // 显式标注穿透意图，消除编译器警告
    case 2:
        std::cout << "Step 2 (after fallthrough)" << std::endl;
        break;
}
```

### 4.3 `for`

```cpp
// 传统 for 循环
for (int i = 0; i < 10; ++i) {
    std::cout << i << " ";
}

// C++11: 范围 for
std::vector<int> vec = {1, 2, 3, 4, 5};
for (const auto& v : vec) {
    std::cout << v << " ";
}

// C++20: 范围 for 带初始化语句
for (auto items = getItems(); auto&& item : items) {
    process(item);
}  // items 的生命周期与循环绑定

// 无限循环
for (;;) {
    // 需要 break 退出
}
```

### 4.4 `while` / `do...while`

```cpp
// while：先判断条件
int i = 0;
while (i < 5) {
    std::cout << i++ << " ";
}

// do...while：至少执行一次
int j = 0;
do {
    std::cout << j++ << " ";
} while (j < 5);

// 带初始化器的 while (C++23)
while (auto conn = pool.acquire(); conn.isValid()) {
    conn.process();
}
```

### 4.5 `continue` / `break`

```cpp
for (int i = 0; i < 10; ++i) {
    if (i % 2 == 0) continue;  // 跳过偶数，直接进入下一次迭代
    if (i == 7) break;         // i=7 时退出循环
    std::cout << i << " ";     // 输出：1 3 5
}
```

### 4.6 `goto`

```cpp
void cleanupExample() {
    FILE* f1 = fopen("file1.txt", "r");
    if (!f1) return;

    FILE* f2 = fopen("file2.txt", "r");
    if (!f2) goto cleanup_f1;  // 文件2打开失败，清理文件1

    // ... 处理两个文件 ...

    fclose(f2);
cleanup_f1:
    fclose(f1);
}
```

### 4.7 `return`

```cpp
int add(int a, int b) {
    return a + b;
}

// void 函数中 return 用于提前退出
void process(int value) {
    if (value < 0) return;
    std::cout << "Processing: " << value << std::endl;
}

// C++14: 自动返回类型推导
auto multiply(int a, int b) {
    return a * b;  // 推导为 int
}
```

---

## 5. 类和对象相关

### 5.1 `class` / `struct`

```cpp
// class：默认成员为 private
class Person {
    std::string name;  // private
    int age;           // private
public:
    Person(std::string n, int a) : name(std::move(n)), age(a) {}
    std::string getName() const { return name; }
};

// struct：默认成员为 public
struct Point {
    int x, y;  // public
    double distance() const { return std::sqrt(x*x + y*y); }
};
```

### 5.2 `public` / `protected` / `private`

**用途：** 访问控制说明符。

```cpp
class Base {
public:     // 任何地方可访问
    int publicVar = 1;
protected:  // 派生类和本类可访问
    int protectedVar = 2;
private:    // 仅本类可访问
    int privateVar = 3;
};

class Derived : public Base {
public:
    void accessTest() {
        std::cout << publicVar << std::endl;     // OK
        std::cout << protectedVar << std::endl;  // OK
        // std::cout << privateVar << std::endl; // 编译错误！派生类不可访问基类 private 成员
    }
};

Derived d;
std::cout << d.publicVar << std::endl;     // OK
// std::cout << d.protectedVar << std::endl; // 编译错误！
// std::cout << d.privateVar << std::endl;   // 编译错误！
```

### 5.3 `virtual` / `override` / `final`

```cpp
// virtual：声明虚函数，支持多态
class Base {
public:
    virtual void draw() const { std::cout << "Base::draw" << std::endl; }
    virtual ~Base() = default;  // 虚析构函数
};

// override：显式标注重写
class Derived : public Base {
public:
    void draw() const override { std::cout << "Derived::draw" << std::endl; }
    // 如果写错函数签名，编译器会报错：
    // void draw(int x) override {}  // 编译错误！Base 中没有 draw(int)
};

// final：阻止进一步重写
class FinalDerived : public Derived {
public:
    void draw() const final override {}  // 不能再被重写
};

class MoreDerived : public FinalDerived {
public:
    // void draw() const override {}  // 编译错误！FinalDerived::draw 是 final
};

// final 也可用于类，阻止被继承
class NoInherit final : public Base {
    // ...
};
// class Attempt : public NoInherit {};  // 编译错误！NoInherit 是 final

// virtual 的典型多态用法
void render(const Base& obj) {
    obj.draw();  // 根据实际类型调用对应的 draw
}
```

### 5.4 `explicit`

**用途：** 阻止构造函数或转换运算符进行隐式类型转换。

```cpp
class String {
public:
    explicit String(int capacity) {  // 不能隐式转换
        std::cout << "Allocate " << capacity << std::endl;
    }

    explicit String(const char* s) {  // 不能隐式转换
        length = strlen(s);
    }
private:
    size_t length = 0;
};

// String s1 = 100;   // 编译错误！explicit 禁止隐式转换
String s1(100);        // OK，显式调用
// String s2 = "hello"; // 编译错误！
String s2("hello");     // OK

// C++20: explicit(bool) — 条件性 explicit
template<typename T>
class Wrapper {
public:
    // 当 T 可隐式转换为 int 时禁用显式构造函数
    explicit(!std::is_convertible_v<T, int>)
    Wrapper(T value) : val(value) {}
private:
    int val;
};

struct Convertible { operator int() { return 42; } };
struct NonConvertible { };
Wrapper<Convertible> w1 = Convertible{};  // OK，可隐式构造
// Wrapper<NonConvertible> w2 = NonConvertible{}; // 错误，必须显式构造
```

### 5.5 `friend`

**用途：** 授予函数或类访问私有和保护成员的权限。

```cpp
class Box {
    int width;
public:
    Box(int w) : width(w) {}

    // 1. 友元函数
    friend void printWidth(const Box& b);

    // 2. 友元类
    friend class BoxFactory;
};

void printWidth(const Box& b) {
    std::cout << b.width << std::endl;  // 可访问私有成员
}

class BoxFactory {
public:
    static Box createDefault() {
        return Box(100);
    }
};

// 3. 友元成员函数
class Printer;  // 前向声明
class Data {
    int secret = 42;
    friend void Printer::print(const Data&);  // Printer::print 是友元
};
```

### 5.6 `this` / `*this`

`this` 是指向当前对象本身的指针（保留标识符，非关键字，但在所有成员函数中可用）。

```cpp
class Entity {
    int id;
public:
    Entity(int id) {
        this->id = id;  // 区分成员变量和参数
    }

    Entity& setId(int id) {
        this->id = id;
        return *this;  // 返回自身引用，支持链式调用
    }

    Entity& add() {
        ++id;
        return *this;
    }
};

Entity e(1);
e.setId(10).add().add();  // 链式调用
```

### 5.7 `using`（类型别名与引入名字）

```cpp
// 1. 类型别名（C++11，替代 typedef）
using String = std::string;
using IntVector = std::vector<int>;
template<typename T>
using Ptr = std::shared_ptr<T>;

// 2. 引入基类成员
class Base {
protected:
    void foo(int) {}
    int value = 10;
};
class Derived : public Base {
public:
    using Base::foo;    // 将 Base::foo 引入到 public 作用域
    using Base::value;  // 将 Base::value 引入到 public 作用域
};

// 3. 引入命名空间
using namespace std::literals;

// 4. using enum (C++20)
enum class Color { Red, Green, Blue };
void process(Color c) {
    using enum Color;  // 引入枚举值，避免 Color:: 前缀
    switch (c) {
        case Red:   /* ... */ break;
        case Green: /* ... */ break;
        case Blue:  /* ... */ break;
    }
}
```

### 5.8 `enum`

```cpp
// 传统枚举（非作用域限定）
enum Day { Sun, Mon, Tue, Wed, Thu, Fri, Sat };
Day today = Mon;
int n = today;  // 隐式转换为 int

// C++11: 枚举类（作用域限定），推荐！
enum class Color : uint8_t {  // 指定底层类型
    Red = 1,
    Green = 2,
    Blue = 4
};
Color c = Color::Red;
// int x = c;  // 编译错误！不能隐式转换
int x = static_cast<int>(c);  // 正确：显式转换

// C++20: using enum
using enum Color;
Color c2 = Red;  // 无需 Color:: 前缀
```

---

## 6. 命名空间和作用域

### 6.1 `namespace`

```cpp
// 命名空间定义
namespace MyLib {
    const int VERSION = 1;
    void initialize() { /* ... */ }

    namespace Detail {  // 嵌套命名空间
        void helper() { /* ... */ }
    }
}

// C++17: 嵌套命名空间的简洁语法
namespace A::B::C {
    void foo() {}
}
// 等价于 namespace A { namespace B { namespace C { void foo() {} } } }

// 匿名命名空间：内部链接，替代文件级 static
namespace {
    int internalCounter = 0;  // 仅在本翻译单元可见
}

// 内联命名空间：版本管理
namespace Api {
    inline namespace v2 {  // 默认使用 v2
        int getValue() { return 2; }
    }
    namespace v1 {
        int getValue() { return 1; }
    }
}
int value = Api::getValue();     // 使用 v2
int oldVal = Api::v1::getValue(); // 显式使用 v1
```

### 6.2 `inline`

**用途：** 多重含义：

```cpp
// 1. 内联函数：建议编译期内联展开
inline int max(int a, int b) {
    return (a > b) ? a : b;
}

// 2. C++17: 内联变量，允许多个翻译单元定义
// 在头文件中直接定义
inline int globalConfig = 100;
inline constexpr double PI = 3.14159265;

class Config {
    static inline int timeout = 3000;  // C++17 类内内联静态成员
};

// 3. 内联命名空间：已在上方展示
```

---

## 7. 类型转换

### 7.1 `static_cast`

**用途：** 编译期类型转换，用于明确定义的、安全的转换。

```cpp
// 基本类型转换
double d = 3.14159;
int i = static_cast<int>(d);  // 3

// 基类到派生类的向上转型
class Base {};
class Derived : public Base {};
Derived derived;
Base* base = static_cast<Base*>(&derived);  // 向上转型，安全

// void* 转回原类型
int x = 42;
void* vp = &x;
int* ip = static_cast<int*>(vp);

// 枚举到整数
enum class Color { Red = 1 };
int colorVal = static_cast<int>(Color::Red);
```

### 7.2 `dynamic_cast`

**用途：** 运行时类型转换，用于多态类型的向下转型（带类型检查）。

```cpp
class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() = 0;
};
class Dog : public Animal {
public:
    void speak() override { std::cout << "Woof!" << std::endl; }
    void wagTail() { std::cout << "Wagging..." << std::endl; }
};
class Cat : public Animal {
public:
    void speak() override { std::cout << "Meow!" << std::endl; }
};

Animal* pet = new Dog();
Dog* dog = dynamic_cast<Dog*>(pet);  // 成功，pet 确实是 Dog
if (dog) dog->wagTail();

Cat* cat = dynamic_cast<Cat*>(pet);  // 失败，返回 nullptr
if (!cat) std::cout << "Not a Cat!" << std::endl;
```

### 7.3 `const_cast`

**用途：** 添加或移除 `const` 和 `volatile` 限定符。

```cpp
void print(char* str) {
    std::cout << str << std::endl;
}

const char* msg = "Hello, World!";
// print(msg);  // 编译错误！const char* 不能转为 char*
print(const_cast<char*>(msg));  // OK（但要确保函数不修改内容）

// 更安全的做法：只在确认安全时使用
class Storage {
    int value;
public:
    Storage(int v) : value(v) {}
    int& getRef() const {
        return const_cast<int&>(value);  // 临时移除 const
    }
};
```

### 7.4 `reinterpret_cast`

**用途：** 底层重新解释位模式，最危险的转换。

```cpp
// 整数与指针互转（低层次系统编程）
int value = 42;
int* ptr = &value;
uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
int* ptr2 = reinterpret_cast<int*>(addr);

// 结构体位模式转换（不可跨平台）
struct FloatBits { uint32_t sign:1; uint32_t exp:8; uint32_t mantissa:23; };
float f = 3.14f;
auto& bits = reinterpret_cast<FloatBits&>(f);
bits.exp += 1;  // 修改浮点数的指数部分

// 不相关类型的指针转换（极度危险）
int i = 0x4141;
// char* c = reinterpret_cast<char*>(&i);  // 仅在特定场景使用
```

---

## 8. 异常处理

### 8.1 `try` / `catch` / `throw`

```cpp
class MyError : public std::exception {
    std::string msg;
public:
    MyError(std::string m) : msg(std::move(m)) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

int divide(int a, int b) {
    if (b == 0) throw MyError("Division by zero!");
    return a / b;
}

// 多异常捕获
try {
    int result = divide(10, 0);
    std::cout << result << std::endl;
} catch (const MyError& e) {
    std::cerr << "Custom error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Standard error: " << e.what() << std::endl;
} catch (...) {
    std::cerr << "Unknown error" << std::endl;
}

// 重新抛出
try {
    // ...
} catch (const MyError& e) {
    std::cerr << "Logging: " << e.what() << std::endl;
    throw;  // 重新抛出原异常，保留类型信息
}
```

### 8.2 `noexcept`

**用途：**（C++11）声明函数不会抛出异常。

```cpp
// noexcept 说明符：承诺不抛异常
int safeAdd(int a, int b) noexcept {
    return a + b;  // 保证不抛异常
}

// 条件 noexcept
template<typename T>
T copy(T value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
    return value;
}

// noexcept 运算符：在编译期检查表达式是否 noexcept
static_assert(noexcept(safeAdd(1, 2)));  // true

// 移动构造函数的典型用法
class Buffer {
    int* data;
    size_t size;
public:
    // 声明为 noexcept 让标准容器优先使用移动语义
    Buffer(Buffer&& other) noexcept
        : data(std::exchange(other.data, nullptr))
        , size(std::exchange(other.size, 0)) {}
};

// 析构函数默认是 noexcept
~Buffer() { delete[] data; }  // 隐式 noexcept
```

---

## 9. 模板和泛型编程

### 9.1 `template` / `typename`

```cpp
// 函数模板
template<typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

// 类模板（多个参数）
template<typename T, typename Alloc = std::allocator<T>>
class Container {
    T* data;
public:
    template<typename U>  // 成员模板
    void insert(U&& value) { /* ... */ }
};

// 依赖类型中的 typename
template<typename T>
class Wrapper {
    // typename 告诉编译器 T::value_type 是一个类型
    typename T::value_type get() { return T::value_type{}; }
};

// 非类型模板参数
template<typename T, int Size>
class Array {
    T data[Size];
public:
    constexpr int size() const { return Size; }
};
Array<int, 10> arr;

// C++20: 类模板实参推导（CTAD）的 auto 非类型模板参数
template<auto Value>
struct Constant {
    static constexpr auto value = Value;
};
Constant<42> c1;
Constant<'A'> c2;
```

### 9.2 `concept` / `requires`

**用途：**（C++20）约束模板参数，提供更好的错误信息。

```cpp
#include <concepts>

// 定义 concept
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 使用 concept 约束函数模板
template<Numeric T>
T add(T a, T b) {
    return a + b;
}

// requires 子句
template<typename T>
requires Numeric<T>
T multiply(T a, T b) {
    return a * b;
}

// 在 requires 中定义约束
template<typename T>
concept Printable = requires(T a, std::ostream& os) {
    { os << a } -> std::same_as<std::ostream&>;
    // a 必须能用 ostream 输出
};

// 多个 concept 组合
template<typename T>
concept SortableContainer = requires(T c) {
    typename T::value_type;
    requires std::sortable<typename T::iterator>;
};

template<SortableContainer Container>
void sortContainer(Container& c) {
    std::sort(c.begin(), c.end());
}

// 简写函数模板
void print(Numeric auto value) {  // 等价于 template<Numeric T> void print(T value)
    std::cout << value << std::endl;
}
```

### 9.3 `decltype`

**用途：**（C++11）获取表达式的声明类型。

```cpp
int x = 10;
decltype(x) y = 20;  // y 的类型为 int

// 常用于返回类型推导
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// C++14: 简化
template<typename T, typename U>
auto add(T a, U b) {
    return a + b;  // 自动推导
}

// decltype(auto)：保留引用性
int& getRef() {
    static int val = 42;
    return val;
}
decltype(auto) ref = getRef();  // ref 的类型是 int&（保留了引用）

// decltype 与 auto 的区别
int value = 0;
auto a = value;       // int（剥离引用和 cv 限定）
decltype(auto) b = value;  // int（value 不是引用，所以 b 也是 int）

const int cval = 10;
auto c = cval;        // int（剥离 const）
decltype(auto) d = cval;  // const int（保留 const）
```

### 9.4 `decltype(auto)` / `auto`

```cpp
// auto：自动类型推导
auto i = 42;           // int
auto d = 3.14;         // double
auto s = "hello";      // const char*

std::vector<int> vec = {1, 2, 3};
auto it = vec.begin(); // std::vector<int>::iterator

// auto 作为返回类型（C++14）
auto square(int n) {
    return n * n;
}

// auto 作为参数类型（C++20，lambda 中的万能引用）
// auto lambda = [](auto&& a, auto&& b) { return a + b; };  // 泛型 lambda

// decltype(auto) 与 auto 的区别
const std::string text = "hello";
auto copy = text;           // std::string，创建了副本（剥离 const 和引用）
decltype(auto) copy2 = text; // const std::string，保留 const 和引用
```

---

## 10. 并发编程

### 10.1 `thread_local`

参见 [3.3 `thread_local`](#33-thread_local)。

---

## 11. 内存管理

### 11.1 `new` / `delete`

```cpp
// 基本用法
int* p = new int(42);     // 分配并初始化
delete p;                 // 释放内存

// 数组
int* arr = new int[100];  // 分配 100 个 int
delete[] arr;             // 释放数组，必须用 delete[]

// placement new：在已有内存上构造对象
alignas(std::string) char buffer[sizeof(std::string) * 3];
std::string* sp = new(&buffer) std::string("Hello");  // placement new
sp->~std::string();  // 必须手动析构，不能 delete

// nothrow new：分配失败返回 nullptr 而非抛异常
int* safe = new(std::nothrow) int[1000000];
if (!safe) {
    std::cerr << "Allocation failed!" << std::endl;
}

// 自定义 operator new/delete
class Pool {
public:
    static void* operator new(size_t size) {
        std::cout << "Custom new: " << size << " bytes" << std::endl;
        return ::operator new(size);
    }
    static void operator delete(void* ptr) {
        std::cout << "Custom delete" << std::endl;
        ::operator delete(ptr);
    }
};
```

### 11.2 `alignas` / `alignof`

**用途：**（C++11）内存对齐控制。

```cpp
// alignof：查询类型的对齐要求
std::cout << "int alignment: " << alignof(int) << std::endl;  // 通常 4

// alignas：指定对齐方式
alignas(64) int cacheLineAligned[4];  // 64字节对齐，避免伪共享

struct alignas(16) AlignedStruct {
    float x, y, z, w;  // SIMD 友好的 16 字节对齐
};

// 线程间避免 false sharing 的典型用法
struct alignas(64) PaddedCounter {
    std::atomic<int> value;
    // 剩下的字节自动填充到 64 字节
};
```

### 11.3 `sizeof`

**用途：** 查询类型或对象的大小（字节数）。

```cpp
std::cout << sizeof(int) << std::endl;   // 通常 4
std::cout << sizeof(char) << std::endl;  // 1
std::cout << sizeof(void*) << std::endl; // 4 (32位) 或 8 (64位)

// 可以接受类型或表达式
int arr[10];
std::cout << sizeof(arr) << std::endl;   // 40（10 * 4）
std::cout << sizeof(arr[0]) << std::endl; // 4

// 类大小（考虑对齐和填充）
struct A {
    char c;   // 1 byte
    int i;    // 4 bytes
    // 总共 8 bytes（填充了 3 bytes）
};
std::cout << sizeof(A) << std::endl;  // 8
```

---

## 12. 类型推导与属性

### 12.1 `typeid`

**用途：** 运行时类型信息，返回 `std::type_info` 对象。

```cpp
#include <typeinfo>

class Base { virtual ~Base() = default; };
class Derived : public Base {};
Base* b = new Derived();

std::cout << "Type: " << typeid(*b).name() << std::endl;  // 运行时类型

// 比较类型
if (typeid(*b) == typeid(Derived)) {
    std::cout << "It's a Derived!" << std::endl;
}

// 需要有虚函数才能返回正确类型
```

### 12.2 `static_assert`

**用途：**（C++11）编译期断言。

```cpp
// 基本用法
static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
static_assert(sizeof(void*) == 8, "64-bit only!");

// C++17 起第二个参数可选
static_assert(sizeof(long) >= 8);

// 模板中常用
template<typename T>
void process(T value) {
    static_assert(std::is_arithmetic_v<T>,
                  "T must be an arithmetic type");
    // ...
}

// concept 出现前用于 SFINAE
template<typename T>
T safeDivide(T a, T b) {
    static_assert(!std::is_floating_point_v<T> || true,
                  "Integer division is recommended");
    return a / b;
}
```

### 12.3 `typedef`

**用途：** 类型别名（历史方式，推荐用 `using`）。

```cpp
typedef unsigned long ulong;
typedef std::vector<int> IntVector;
typedef void (*FuncPtr)(int, double);  // 函数指针

// 现代 C++ 推荐用 using
using ulong2 = unsigned long;
using IntVector2 = std::vector<int>;
using FuncPtr2 = void(*)(int, double);
```

### 12.4 `asm`

**用途：** 内联汇编声明。

```cpp
// 内联汇编（GCC 扩展语法，非标准但保留为关键字）
int value = 10;
// asm("nop");  // 插入一个 NOP 指令（GCC扩展）

// 通常用于极低层次优化或操作系统开发
// asm volatile("cli" : : : "memory");  // x86 关中断
```

---

## 13. C++20 新增关键字

### 13.1 `concept`

参见 [9.2 `concept` / `requires`](#92-concept--requires)。

### 13.2 `requires`

参见 [9.2 `concept` / `requires`](#92-concept--requires)。

### 13.3 `consteval`

参见 [2.5 `consteval`](#25-consteval)。

### 13.4 `constinit`

参见 [2.6 `constinit`](#26-constinit)。

### 13.5 `co_await`

**用途：** 协程支持 — 挂起当前协程直到操作完成。

```cpp
#include <coroutine>
#include <future>

// 异步 task 类型（简化版）
template<typename T>
struct Task {
    struct promise_type {
        T value;
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_value(T v) { value = v; }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    T get() { return handle.promise().value; }
};

// 异步读取函数
Task<std::string> readFileAsync(const std::string& path) {
    // 模拟异步操作
    co_await std::suspend_never{};  // 简化示例
    co_return "file content";
}

// 使用协程
Task<void> process() {
    std::string content = co_await readFileAsync("data.txt");
    std::cout << "Got: " << content << std::endl;
}
```

### 13.6 `co_return`

**用途：** 协程支持 — 从协程返回值。

```cpp
Task<int> computeAsync() {
    int result = 42;
    co_return result;  // 返回结果
}

// void 协程
Task<void> process() {
    // 做异步工作...
    co_return;  // 不返回值，仅标记函数结束
}
```

### 13.7 `co_yield`

**用途：** 协程支持 — 生成值到调用者（协程生成器）。

```cpp
struct Generator {
    struct promise_type {
        int current;
        std::suspend_always yield_value(int value) {
            current = value;
            return {};
        }
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    int current() const { return handle.promise().current; }
    bool next() {
        if (handle.done()) return false;
        handle.resume();
        return !handle.done();
    }
};

Generator range(int from, int to) {
    for (int i = from; i < to; ++i) {
        co_yield i;  // 产生下一个值
    }
}

// 使用
Generator gen = range(1, 5);
while (gen.next()) {
    std::cout << gen.current() << " ";  // 输出: 1 2 3 4
}
```

### 13.8 `char8_t`

参见 [1.3 `char` / `wchar_t` / `char8_t` / `char16_t` / `char32_t`](#13-char--wchar_t--char8_t--char16_t--char32_t)。

---

## 14. C++23 新增关键字

### 14.1 `static_operator[]`

**用途：**（C++23）允许 `operator[]` 为静态成员函数。此关键字组合赋予 `operator[]` 多下标支持。

```cpp
// C++23: 多维下标运算符
class Matrix {
    std::vector<int> data;
    size_t rows, cols;
public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c) {}

    int& operator[](size_t row, size_t col) {  // C++23: 多参数 operator[]
        return data[row * cols + col];
    }
};

Matrix m(3, 4);
m[1, 2] = 42;  // 等价于 m.operator[](1, 2)
```

### 14.2 `auto(Type)`

**用途：**（C++23）`auto(value)` 和 `auto{value}` 语法，创建与 value 相同类型并剥离引用的 prvalue 副本。

```cpp
int x = 10;
auto(x) copy1 = x;  // int copy1 = x

const int& ref = x;
auto(ref) copy2 = ref;  // int copy2 = ref（剥离引用和 const）

// 与 decltype(auto) 的区别
int& getX() { static int v = 0; return v; }
auto{getX()} val = 42;  // int val = 42，创建了副本
```

### 14.3 `if consteval`

**用途：**（C++23）判断当前上下文是否是编译期求值。

```cpp
constexpr int smartFunction(int n) {
    if consteval {
        // 编译期执行路径
        return n * n;
    } else {
        // 运行期执行路径
        return n * 2;
    }
}

constexpr int a = smartFunction(5);  // 编译期：25
int x = 5;
int b = smartFunction(x);             // 运行期：10
```

### 14.4 `static_assert(false)` 修复

C++23 之前 `static_assert(false)` 会在模板定义时就触发编译错误；C++23 修复了此行为。

```cpp
// C++23: 这种写法现在正常工作
template<typename T>
void shouldNotCompile() {
    static_assert(false, "This type should not be instantiated");
    // 在 C++17/20 中这会在模板定义时就报错
    // C++23: 只在实例化时报错
}

// C++20 的 workaround（C++23 不再需要）
template<typename T>
void shouldNotCompile20() {
    static_assert(sizeof(T) == 0, "This type should not be instantiated");
}
```

### 14.5 `static operator()`

**用途：**（C++23）允许 `operator()` 声明为静态成员函数。

```cpp
// C++23: 静态调用运算符
struct StaticLambda {
    static int operator()(int a, int b) {
        return a + b;
    }
};

using Fn = int(*)(int, int);
Fn ptr = StaticLambda{};  // 可退化为普通函数指针！
int result = ptr(3, 4);    // 7
```

---

## 15. 其他关键字

### 15.1 `sizeof...`

**用途：**（C++11）查询参数包中元素个数。

```cpp
template<typename... Args>
void printCount(Args... args) {
    std::cout << "Number of arguments: " << sizeof...(args) << std::endl;
    std::cout << "Number of types: " << sizeof...(Args) << std::endl;
}

printCount(1, "hello", 3.14);  // 输出 3

// 常用于变参模板
template<typename... Ts>
constexpr size_t typeCount = sizeof...(Ts);
```

### 15.2 `static_cast` / `dynamic_cast` / `const_cast` / `reinterpret_cast`

参见 [7. 类型转换](#7-类型转换)。

### 15.3 `true` / `false`

```cpp
bool flag = true;
bool isDone = false;

// C++ 中的字面量类型
std::integral_constant<bool, true> alwaysTrue;
std::true_type;   // 类型级别的 true
std::false_type;  // 类型级别的 false

// constexpr 条件中
if constexpr (std::is_integral_v<int>) {
    // 总是进入这个分支
}
```

### 15.4 `nullptr`

**用途：**（C++11）空指针字面量，替代旧的 `NULL` 宏。

```cpp
int* ptr = nullptr;

// 函数重载区分
void func(int);
void func(char*);

func(0);       // 调用 func(int)，有歧义！
func(nullptr); // 调用 func(char*)，安全

// nullptr_t 类型
std::nullptr_t null = nullptr;

// 检查指针
if (ptr == nullptr) {
    std::cout << "Pointer is null" << std::endl;
}
```

### 15.5 `operator`

**用途：** 定义运算符重载。

```cpp
class Complex {
    double real, imag;
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // 二元运算符
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    // 一元运算符
    Complex operator-() const {
        return Complex(-real, -imag);
    }

    // 比较运算符（C++20 后可用 <=> 简化）
    auto operator<=>(const Complex&) const = default;

    // 下标运算符
    double& operator[](int index) {
        return (index == 0) ? real : imag;
    }

    // C++23: 多维下标
    // double operator[](int r, int c) const { ... }

    // 类型转换运算符
    explicit operator double() const {
        return real;
    }
};

Complex a(1, 2), b(3, 4);
Complex c = a + b;     // 调用 operator+
Complex neg = -a;       // 调用 operator-
```

### 15.6 `export`

**用途：**（C++20 引入模块）声明模块接口单元。

```cpp
// ---- math_module.ixx (模块接口文件) ----
export module Math;  // 声明模块

export int add(int a, int b) { return a + b; }
export int multiply(int a, int b) { return a * b; }

// 内部实现（不导出）
int helper() { return 42; }

// ---- main.cpp ----
import Math;  // 导入模块（C++20）

int main() {
    int result = add(3, 4);  // 10
    return 0;
}
```

### 15.7 `import` / `module`

**用途：**（C++20）模块系统。

```cpp
// 模块声明和导入
// ---- geometry.ixx ----
export module Geometry;

export struct Point {
    double x, y;
};

export double distance(Point a, Point b) {
    return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

// ---- main.cpp ----
import Geometry;  // C++20

int main() {
    Point a{0, 0}, b{3, 4};
    double d = distance(a, b);  // 5.0
    return 0;
}
```

### 15.8 `noexcept` / `throw`

- `noexcept`：参见 [8.2 `noexcept`](#82-noexcept)
- `throw`：参见 [8.1 `try` / `catch` / `throw`](#81-try--catch--throw)

---

## 16. 替代标记

**用途：** 某些键盘布局或编码不支持 `& | ! ^ ~` 等符号时使用。

```cpp
// 替代标记表
// && → and
// || → or
// !  → not
// &  → bitand
// |  → bitor
// ^  → xor
// ~  → compl
// &= → and_eq
// |= → or_eq
// ^= → xor_eq
// != → not_eq

// 示例
bool result = (true and false) or (not false);  // 等价于 (true && false) || (!false)
int a = 5 bitand 3;  // 等价于 5 & 3 = 1
int b = 5 bitor 3;   // 等价于 5 | 3 = 7
int c = 5 xor 3;     // 等价于 5 ^ 3 = 6
int d = compl 5;     // 等价于 ~5

#include <iso646.h>  // C 兼容头文件（C++ 中这些是内置关键字）
```

---

## 附录：关键字快速索引表

| 关键字 | 类别 | C++标准 |
|--------|------|---------|
| `void` | 基本类型 | C++98 |
| `bool` | 基本类型 | C++98 |
| `char` | 基本类型 | C++98 |
| `char8_t` | 基本类型 | C++20 |
| `char16_t` | 基本类型 | C++11 |
| `char32_t` | 基本类型 | C++11 |
| `wchar_t` | 基本类型 | C++98 |
| `short` | 基本类型 | C++98 |
| `int` | 基本类型 | C++98 |
| `long` | 基本类型 | C++98 |
| `float` | 基本类型 | C++98 |
| `double` | 基本类型 | C++98 |
| `signed` | 基本类型 | C++98 |
| `unsigned` | 基本类型 | C++98 |
| `const` | 类型修饰 | C++98 |
| `volatile` | 类型修饰 | C++98 |
| `mutable` | 类型修饰 | C++98 |
| `constexpr` | 类型修饰 | C++11 |
| `consteval` | 类型修饰 | C++20 |
| `constinit` | 类型修饰 | C++20 |
| `static` | 存储类 | C++98 |
| `extern` | 存储类 | C++98 |
| `thread_local` | 存储类 | C++11 |
| `register` | 存储类 | C++98(保留) |
| `inline` | 存储类 | C++98 |
| `if` | 控制流 | C++98 |
| `else` | 控制流 | C++98 |
| `switch` | 控制流 | C++98 |
| `case` | 控制流 | C++98 |
| `default` | 控制流 | C++98 |
| `break` | 控制流 | C++98 |
| `continue` | 控制流 | C++98 |
| `goto` | 控制流 | C++98 |
| `return` | 控制流 | C++98 |
| `for` | 控制流 | C++98 |
| `while` | 控制流 | C++98 |
| `do` | 控制流 | C++98 |
| `class` | 类 | C++98 |
| `struct` | 类 | C++98 |
| `public` | 类 | C++98 |
| `protected` | 类 | C++98 |
| `private` | 类 | C++98 |
| `virtual` | 类 | C++98 |
| `override` | 类 | C++11 |
| `final` | 类 | C++11 |
| `explicit` | 类 | C++98 |
| `friend` | 类 | C++98 |
| `using` | 类/别名 | C++98 |
| `enum` | 类 | C++98 |
| `namespace` | 作用域 | C++98 |
| `static_cast` | 转换 | C++98 |
| `dynamic_cast` | 转换 | C++98 |
| `const_cast` | 转换 | C++98 |
| `reinterpret_cast` | 转换 | C++98 |
| `try` | 异常 | C++98 |
| `catch` | 异常 | C++98 |
| `throw` | 异常 | C++98 |
| `noexcept` | 异常 | C++11 |
| `template` | 模板 | C++98 |
| `typename` | 模板 | C++98 |
| `concept` | 模板 | C++20 |
| `requires` | 模板 | C++20 |
| `decltype` | 类型推导 | C++11 |
| `auto` | 类型推导 | C++11 |
| `new` | 内存 | C++98 |
| `delete` | 内存 | C++98 |
| `alignas` | 内存 | C++11 |
| `alignof` | 内存 | C++11 |
| `sizeof` | 类型查询 | C++98 |
| `sizeof...` | 类型查询 | C++11 |
| `typeid` | 类型查询 | C++98 |
| `static_assert` | 类型查询 | C++11 |
| `typedef` | 别名 | C++98 |
| `true` | 字面量 | C++98 |
| `false` | 字面量 | C++98 |
| `nullptr` | 字面量 | C++11 |
| `operator` | 运算符重载 | C++98 |
| `asm` | 汇编 | C++98 |
| `export` | 模块 | C++20 |
| `import` | 模块 | C++20 |
| `module` | 模块 | C++20 |
| `co_await` | 协程 | C++20 |
| `co_return` | 协程 | C++20 |
| `co_yield` | 协程 | C++20 |
| `consteval` | 编译期 | C++20 |
| `constinit` | 编译期 | C++20 |
| `and` | 替代标记 | C++98 |
| `or` | 替代标记 | C++98 |
| `not` | 替代标记 | C++98 |
| `bitand` | 替代标记 | C++98 |
| `bitor` | 替代标记 | C++98 |
| `xor` | 替代标记 | C++98 |
| `compl` | 替代标记 | C++98 |
| `and_eq` | 替代标记 | C++98 |
| `or_eq` | 替代标记 | C++98 |
| `xor_eq` | 替代标记 | C++98 |
| `not_eq` | 替代标记 | C++98 |

---

> **参考标准：** ISO/IEC 14882:2017 (C++17), ISO/IEC 14882:2020 (C++20), ISO/IEC 14882:2024 (C++23)
