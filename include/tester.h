#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <opencv2/core/types.hpp>

namespace cvtest::tester {
class Tester : public ::testing::Test {
protected:
#ifdef _WIN32
  std::string assets_ = "F:/codes/CPP/LearnCPP/assets/";
#else
  std::string assets_ = "/home/inmove/code/LearnCPP/assets/";
#endif
  std::string test0_ = (std::filesystem::path(assets_) / "test0.png").string();
  std::string test0New_ =
      (std::filesystem::path(assets_) / "test0New.png").string();
  std::string test0Mp4_ =
      (std::filesystem::path(assets_) / "test0.mp4").string();
  std::string test0Mp4New_ =
      (std::filesystem::path(assets_) / "test0New.mp4").string();

  int sharpenValue(cv::Mat &image);
  void drawHistogram2D(const cv::Mat &hist, const std::string &histName,
                       int histSize);
  void drawHistogram2D(const cv::Mat &gray, const std::string &histName);
  void drawColorHistogram3Channel(const cv::Mat &src);
  void drawDetailedHistogram(const cv::Mat &gray);
  int calculateLaplacianSum(const cv::Mat &src);

public:
  void SetUp() override;
  void TearDown() override;
};
} // namespace cvtest::tester
