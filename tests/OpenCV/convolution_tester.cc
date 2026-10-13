/**
 * 卷积操作
 * */

#include "tester.h"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace cvtest::tester {

/**
 * 均值模糊，值全为1的卷积核
 * void cv::blur(
 *  InputArray src, OutputArray dst, Size ksize,
 *  Point anchor = Point(-1, -1), int borderType=BORDER_DEFAULT
 * )
 * 1. anchor: 锚点，(-1, -1)表示中心
 * 2. borderType，边缘处理的方式:
 *  BORDER_DEFAULT: 和常量填充, 11|abcdefgh|11
 *  BORDER_REPLICATE: 边界复制, aa|abcdefgh|gg
 *  BORDER_REFLECT: 边界反射, fedcba|abcdefgh|hgfedc
 *  BORDER_WRAP: 边界换行（高版本的OpenCV4，deprecated）
 *  BORDER_REFLECT_101: 与Default一致
 *  BORDER_TRANSPARENT: 边界透明（OpenCV4, deprecated）
 * 3. ksize: 卷积核大小，越大，越模糊
 * */
TEST_F(Tester, OpenCV_Mean_Blur) {
  cv::Mat src = cv::imread(test0_);
  cv::Mat dst;
  cv::blur(src, dst, cv::Size(3, 3));
  cv::imshow("Image", src);
  cv::imshow("Blur Image", dst);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * void cv::GaussianBlur(
 *  InputArray src,
 *  OutputArray dst,
 *  Size ksize,
 *  double sigmaX,
 *  double sigmaY = 0,
 *  int borderType = BORDER_DEFAULT
 * )
 * sigmaX和sigmaY是二维高斯分布的数学公式中的两个参数
 *
 * ksize 必須为奇数。不为0时，用ksize计算sigmaX
 * ksize 设置为(0,0)，根据sigmaX计算ksize
 * sigmaX = 0.3 * ((Size - 1) * 0.5 - 1) + 0.8
 *
 * */
TEST_F(Tester, Opencv_Gauss_Blur) {
  cv::Mat src = cv::imread(test0_);
  cv::Mat dst;
  cv::GaussianBlur(src, dst, cv::Size(7, 7), 0);
  cv::imshow("Image", src);
  cv::imshow("Blur Image", dst);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 中值模糊
 * void cv::medianBlur(
 *  InputArray src,
 *  OutputArray dst,
 *  int ksize,
 * )
 * 不进行点乘计算，只需要对窗口的像素值进行排序即可
 * */
TEST_F(Tester, OpenCV_Median_Blur) {
  cv::Mat src = cv::imread(test0_);
  cv::Mat dst;
  cv::medianBlur(src, dst, 7);
  cv::imshow("Image", src);
  cv::imshow("Blur Image", dst);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 自定义滤波
 * void cv::filter2D(
 *  InputArray src,
 *  OutputArray dst,
 *  int ddepth,
 *  InputArray kernel,
 *  Point anchor = Point(-1, -1),
 *  double delta = 0,
 *  int borderType = BORDER_DEFAULT
 * )
 *
 * 常用卷积核
 * Mat::ones(Size(25, 1), CV_32FC1) / (25 * 1);: 水平模糊
 * Mat::ones(Size(1, 25), CV_32FC1) / (1 * 25);: 垂直模糊
 * Mat::eye(Size(25, 25), CV_32FC1) / (25);: 对角线模糊
 *
 * filter2D做的是邻域加权求和。如果卷积核总和为1，那么输出像素值就是邻域像素的加权平均值
 * 图像整体亮度基本不变
 * */
TEST_F(Tester, OpenCV_Filter2D_Blur) {
  cv::Mat image = cv::imread(test0_);
  cv::Mat k1 = cv::Mat::ones(cv::Size(25, 1), CV_32FC1);
  k1 = k1 / (25 * 1); // 归一化卷积核
  cv::Mat result;
  cv::filter2D(image, result, -1, k1);
  cv::imshow("Source", image);
  cv::imshow("Filtered", result);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

} // namespace cvtest::tester
