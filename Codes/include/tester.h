#pragma once
#include <filesystem>

#include "pch.h"
#include <gtest/gtest.h>
#include <opencv2/core/types.hpp>

namespace cvtest::tester {
class Tester : public ::testing::Test {
public:
  void SetUp() override;
  void TearDown() override;
};
} // namespace cvtest::tester
