/**
 *
 * Chapter two: Template Fundamentals
 *
 * */
#include "tester.h"
#include <iostream>
namespace cvtest::tester {
template <typename T> T add(T const a, T const b) { return a + b; }
TEST_F(Tester, T_Add_Template) {
  add<int>(10, 11);
  add<double>(1.0, 3.9);
}

template <typename Input, typename Predicate>
int count_if(Input start, Input end, Predicate p) {
  int total = 0;
  for (Input i = start; i != end; i++) {
    if (p(*i)) {
      total++;
    }
  }
  return total;
}
TEST_F(Tester, T_Count_If_Template) {
  int arr[]{1, 2, 3, 4, 5, 6};
  int odds = count_if(std::begin(arr), std::end(arr),
                      [](int const n) { return n % 2 == 1; });
  std::cout << odds << "\n";
}
} // namespace cvtest::tester
