#include "tester.h"
#include <chrono>
#include <iostream>
#include <ratio>
#include <string>
#include <vector>

namespace cvtest::tester {
struct Str {
  std::string val;
  Str() : val(100, 'a') {}
  Str(const Str &) = default;
  Str(Str &&s) noexcept : val{std::move(s.val)} {}
};
TEST_F(Tester, MS_Is_Noexcept_Worth) {
  std::vector<Str> coll;
  coll.resize(1000000);
  auto t0 = std::chrono::steady_clock::now();
  coll.reserve(coll.capacity() + 1);
  auto t1 = std::chrono::steady_clock::now();
  std::chrono::duration<double, std::milli> d{t1 - t0};
  std::cout << "count " << d.count() << "\n";
}
} // namespace cvtest::tester
