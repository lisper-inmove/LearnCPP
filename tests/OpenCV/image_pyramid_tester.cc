/**
 * 图像金字塔
 * 1. 高斯金字塔
 * 2. 拉普拉斯金字塔
 * */

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <tester.h>
#include <vector>
namespace cvtest::tester {
/**
 * 1. void cv::pyrDown(InArray src, OutputArray dst, const Size& dstsize = sizd(), int borderType = BORDER_DEFAULT)
 * */
TEST_F(Tester, OpenCV_Pyramid) {
  cv::Mat image = cv::imread(test0_);
  std::vector<cv::Mat> gauss;
  int level = 3;
  gauss.push_back(image);
  for (int i = 0; i < level; i++) {
    cv::Mat gauss_;
    cv::pyrDown(gauss[i], gauss_);
    gauss.push_back(gauss_);
  }
}
} // namespace cvtest::tester
