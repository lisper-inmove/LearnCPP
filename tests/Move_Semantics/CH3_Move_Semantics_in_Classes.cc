/**
 * 第三章: Move Semantics in Classes
 *
 * 从C++11开始，编译器会为类自动生成移动构造函数和移动赋值操作符
 *
 * 当以下函数中有一个被定义时，移动函数不会自动生成:
 * 1. Copy Constructor
 * 2. Copy Assignment Operator
 * 3. Another Move Operation（如果定义了 Move Constructor则另一个指 Move Operator，反之亦然）
 * 4. Destructor
 *
 * 当基类的移动函数 因为某些原因未自动生成，不会影响派生类
 *
 * 移动语义不会自动传递，自己实现的移动函数中，也要移动对应的成员变量。
 * void insertTwice(std::vector<std::string>& coll, std::string&& str) {
 *    coll.push_back(str);  // 如果是传递的话，这一句之后，str就不能再用了
 *    coll.push_back(std::move(str));
 * }
 * */
#include "tester.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace cvtest::tester {
class Customer {
private:
  std::string m_name;
  std::vector<int> m_values;

public:
  Customer(const std::string &n) : m_name(n) { assert(!m_name.empty()); }
  std::string getName() const { return m_name; }
  void addValue(int val) { m_values.push_back(val); }
  friend std::ostream &operator<<(std::ostream &strm, const Customer &cust) {
    strm << '[' << cust.m_name << ": ";
    for (int val : cust.m_values) {
      strm << val << ' ';
    }
    strm << ']';
    return strm;
  }

  Customer(const Customer &cust)
      : m_name{cust.m_name}, m_values(cust.m_values) {
    std::cout << "Copy " << cust.m_name << "\n";
  }

  Customer(Customer &&cust) noexcept
      : m_name{std::move(cust.m_name)}, m_values{std::move(cust.m_values)} {
    std::cout << "Move " << m_name << "\n";
  }

  Customer &operator=(const Customer &cust) {
    std::cout << "Copy assign " << m_name << "\n";
    m_name = cust.m_name;
    m_values = cust.m_values;
    return *this;
  }

  Customer &operator=(Customer &&cust) noexcept {
    std::cout << "Move assign " << m_name << "\n";
    m_name = std::move(cust.m_name);
    m_values = std::move(cust.m_values);
    return *this;
  }
};
TEST_F(Tester, MS_Move_Semantics_in_Ordinary_Classes) {
  Customer c{"Wolfgang Amadeus Mozart"};
  for (int val : {0, 8, 15}) {
    c.addValue(val);
  }
  std::cout << "c: " << c << "\n";
  std::vector<Customer> customers;
  customers.push_back(c);
  customers.push_back(std::move(c));
  std::cout << "c: " << c << "\n";
  std::cout << "customers: \n";
  for (const Customer &cust : customers) {
    std::cout << " " << cust << '\n';
  }
}

TEST_F(Tester, MS_Using_Move_or_Copy) {
  std::vector<Customer> coll;
  /**
   * 移动构造函数没有noexcept时，每次扩容都会发生Copy
   * 有noexcept时，则是Move
   * */
  for (int i = 0; i < 12; i++) {
    coll.push_back(Customer("My name is Customer " + std::to_string(i - 5)));
    std::cout << "Current capacity " << coll.capacity() << "\n";
  }

  std::cout << "---- sort() \n";
  std::sort(coll.begin(), coll.end(),
            [](const Customer &c1, const Customer &c2) {
              return c1.getName() < c2.getName();
            });
}

} // namespace cvtest::tester
