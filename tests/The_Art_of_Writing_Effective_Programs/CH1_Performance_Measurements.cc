#include "tester.h"
#include "gtest/gtest.h"
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace cvtest::tester {

bool compare(const char *s1, const char *s2, unsigned int l) {
  if (s1 == s2)
    return false;
  for (unsigned int i = 0; i < l; i++) {
    if (s1[i] != s2[i])
      return s1[i] > s2[i];
  }
  return false;
}

TEST_F(Tester, WEP_01_Substring_Sort) {
  constexpr unsigned int L = 1 << 18, N = 1 << 14;
  std::unique_ptr<char[]> s(new char[L]);
  std::vector<const char *> vs(N);
  std::minstd_rand rgen;
  ::memset(s.get(), 'a', L * sizeof(char));
  for (unsigned int i = 0; i < L / 1024; ++i) {
    s[rgen() % (L - 1)] = 'a' + (rgen() % ('z' - 'a' + 1));
  }
  s[L - 1] = '\0';
  for (unsigned int i = 0; i < N; ++i) {
    vs[i] = &s[rgen() % (L - 1)];
  }
}
} // namespace cvtest::tester
