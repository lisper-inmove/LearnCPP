/**
 *
 * Chapter two: Class Templates
 *
 * */
#include "tester.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
namespace cvtest::tester {
template <typename T> class Stack {
private:
  std::vector<T> elems_;

public:
  void push(T const &elem);
  void pop();
  T const &top() const;
  bool empty() const { return elems_.empty(); }
};

template <typename T> void Stack<T>::push(T const &elem) { elems_.push_back(elem); }
template <typename T> void Stack<T>::pop() {
  assert(!elems_.empty());
  elems_.pop_back();
}
template <typename T> T const &Stack<T>::top() const {
  assert(!elems_.empty());
  return elems_.back();
}

TEST_F(Tester, T_Class_Template) {
  Stack<int> stk;
  stk.push(10);
}
} // namespace cvtest::tester
