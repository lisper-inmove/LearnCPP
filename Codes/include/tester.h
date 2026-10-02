#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <opencv2/core/types.hpp>

namespace cvtest::tester {
class Tester : public ::testing::Test {
protected:
  std::string assets_ = "/home/inmove/code/LearnCPP/Codes/assets/";
  std::string test0_ = std::filesystem::path(assets_) / "test0.png";
  std::string test0New_ = std::filesystem::path(assets_) / "test0New.png";
  std::string test0Mp4_ = std::filesystem::path(assets_) / "test0.mp4";
  std::string test0Mp4New_ = std::filesystem::path(assets_) / "test0New.mp4";

public:
  void SetUp() override;
  void TearDown() override;
};
} // namespace cvtest::tester
