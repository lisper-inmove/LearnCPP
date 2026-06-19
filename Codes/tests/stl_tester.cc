#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

/**
 * C++标准库
 * */

namespace cvtest::tester {
TEST_F(Tester, STLTester) { std::cout << "C++ STL tester" << "\n"; }

TEST_F(Tester, EraseTester) {
  /**
   * Remove from a continer
   * */
  std::vector<uint32_t> arr{};
  arr.reserve(10);
  for (int i = 0; i < 10; i++) {
    arr.push_back(i);
  }
  arr.push_back(9);
  std::cout << "Before erase: ";
  for (int v : arr)
    std::cout << v << " ";
  std::cout << "\n";

  std::erase(arr, 9);
  std::cout << "After erase 9: ";
  for (int v : arr)
    std::cout << v << " ";
  std::cout << "\n";

  std::erase_if(arr, [](auto x) { return x % 2 == 0; });
  std::cout << "After erase even: ";
  for (int v : arr)
    std::cout << v << " ";
  std::cout << "\n";

  std::map<int, std::string> m{{1, "One"}, {2, "Two"}, {3, "Three"}};
  std::erase_if(m, [](auto &p) {
    auto &[k, v] = p;
    return k % 2 == 0;
  });
  for (auto &[k, v] : m) {
    std::cout << k << " : " << v << "\n";
  }
}
} // namespace cvtest::tester
