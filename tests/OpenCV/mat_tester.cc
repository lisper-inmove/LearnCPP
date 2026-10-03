/**
 *
 * OpenCV Mat
 * OpenCV 中支持的图片类型与深度
 *
 * 单通道
 * CV_8UC1
 * CV_32SC1
 * CV_32FC1
 * CV_64FC1
 * CV_16SC1
 *
 * 双通道
 * CV_8UC2
 * CV_32SC2
 * CV_32FC2
 * CV_64FC2
 * CV_16SC2
 *
 * 8U, 32S 分别表示数据类型为8位符号整型，32位整型
 * C表示Channel
 * 其它还包括3通道，4通道
 *
 * */
#include "tester.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

namespace cvtest::tester {
TEST_F(Tester, OpenCV_Create_Mat) {
  // 创建Mat
  cv::Mat m1(4, 4, CV_8UC1, cv::Scalar(255));
  cv::Mat m2(cv::Size(4, 4), CV_8UC3, cv::Scalar(255, 255, 0));
  cv::Mat m3(cv::Size(4, 4), CV_8UC3, cv::Scalar::all(255));

  cv::Mat m4 = cv::Mat::zeros(cv::Size(4, 4), CV_8UC3);
  // ones只初始化第一个通道
  cv::Mat m5 = cv::Mat::ones(cv::Size(4, 4), CV_8UC3);

  cv::Mat m6 = m4.clone();

  cv::Mat m7;
  m2.copyTo(m7);

  cv::Mat m8 = (cv::Mat_<double>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
}

/**
 * 提取感兴趣的部分
 * */
TEST_F(Tester, OpenCV_Image_ROI) {
  cv::Mat image = cv::imread(test0_);

  cv::imshow("Image", image);
  cv::Rect rect(10, 10, 100, 100);
  cv::Mat roi = image(rect);
  cv::imshow("Roi", roi);
  cv::waitKey(3000);
  cv::destroyAllWindows();
}

TEST_F(Tester, OpenCV_Visit_Element) {
  int row = 3;
  int col = 3;
  cv::Mat m1 = (cv::Mat_<double>(row, col) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
  /**
   * 按像素点访问
   * 安全性高，效率低
   * */
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      m1.at<double>(i, j) = m1.at<double>(i, j) * 0.1;
      std::cout << "(" << i << "," << j << ") = " << m1.at<double>(i, j) << "\n";
    }
  }
  std::cout << "----------------------------------------\n";

  /**
   * 按行指针访问
   * 安全性中，效率高
   * */
  double *currentRow;
  for (int i = 0; i < row; i++) {
    currentRow = m1.ptr<double>(i);
    for (int j = 0; j < col; j++) {
      std::cout << "(" << i << "," << j << ") = " << currentRow[j] << "\n";
    }
  }
  std::cout << "----------------------------------------\n";

  /**
   * 迭代器
   * 安全性高，效率中
   * 迭代器会自动跳过roi之外的数据
   * */
  cv::MatIterator_<double> it, end;
  for (it = m1.begin<double>(), end = m1.end<double>(); it != end; ++it) {
    std::cout << *it << ",";
  }
  std::cout << "\n----------------------------------------\n";

  /**
   * 指针直接访问
   * 安全性低，效率最高
   * */
  ASSERT_TRUE(m1.isContinuous());
  auto *image_data = reinterpret_cast<double *>(m1.data);
  for (size_t k = 0; k < m1.total(); ++k) {
    std::cout << image_data[k] << ",";
  }
  std::cout << "\n----------------------------------------\n";
}

template <typename T> void print_mat(cv::Mat src, int row = 3, int col = 3) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      std::cout << src.at<T>(i, j) << ",";
    }
    std::cout << "\n";
  }
}

/**
 * 像素的算术运算
 *
 * void cv::add(
 *  InputArray src1,
 *  InputArray src2,
 *  OutputArray dst,
 *  InputArray mask = noArray(),
 *  int dtype = -1
 * )
 *
 * void cv::subtract: 同add
 *
 * void cv:multiply(
 *  InputArray src1,
 *  InputArray src2,
 *  OutputArray dst,
 *  double scale = 1,
 *  int dtype = -1
 * )
 *
 * void cv::divide: 同multiply
 *
 * 1. src1, src2 大小必须一致
 * 2. mask表示掩模层过滤
 *
 * */
TEST_F(Tester, OpenCV_Arithmetic_Operation) {
  int row = 3;
  int col = 3;
  cv::Mat src1 = (cv::Mat_<double>(row, col) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
  cv::Mat src2 = (cv::Mat_<double>(row, col) << 1, 1, 1, 1, -5, 1, 1, 1, 1);
  cv::Mat dst;
  std::cout << "Source mat \n";
  print_mat<double>(src1, row, col);
  std::cout << "\n----------------------------------------\n";
  print_mat<double>(src2, row, col);
  std::cout << "\n----------------------------------------\n";

  std::cout << "Add \n";
  cv::add(src1, src2, dst);
  print_mat<double>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "Subtract \n";
  cv::subtract(src1, src2, dst);
  print_mat<double>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "Multiply \n";
  cv::multiply(src1, src2, dst);
  print_mat<double>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "Divide \n";
  cv::divide(src1, src2, dst);
  print_mat<double>(dst);
  std::cout << "\n----------------------------------------\n";
}

/**
 * 位运算
 *
 * 取反
 * void cv::bitwise_not(
 *  InputArray src,
 *  OutputArray dst,
 *  InputArray mask = noArray()
 * )
 *
 * 与
 * void cv::bitwise_and(
 *  InputArray src1,
 *  InputArray src2,
 *  OutputArray dst,
 *  InputArray mask = noArray()
 * )
 * */
TEST_F(Tester, OpenCV_Bit_Operation) {
  int row = 3;
  int col = 3;
  cv::Mat src1 = (cv::Mat_<int>(row, col) << 0, 1, 0, 1, 5, 1, 0, 1, 0);
  cv::Mat src2 = (cv::Mat_<int>(row, col) << 2, 2, 2, 2, 5, 2, 2, 3, 7);
  cv::Mat dst;

  std::cout << "bitewise_not \n";
  cv::bitwise_not(src1, dst);
  print_mat<int>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "bitwise_and \n";
  cv::bitwise_and(src1, src2, dst);
  print_mat<int>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "bitwise_or \n";
  cv::bitwise_or(src1, src2, dst);
  print_mat<int>(dst);
  std::cout << "\n----------------------------------------\n";

  std::cout << "bitwise_xor \n";
  cv::bitwise_xor(src1, src2, dst);
  print_mat<int>(dst);
  std::cout << "\n----------------------------------------\n";
}

/**
 * 图像类型与通道
 * void cv::Mat::convertTo(
 *  OutputArray m,
 *  int rtype,
 *  double alpha = 1,
 *  double beta = 0,
 * ) const
 *
 * void cv::cvtColor(
 *  InputArray src,
 *  OutputArray dst,
 *  int code,
 *  int dstCn = 0
 * )
 * */
TEST_F(Tester, OpenCV_Image_Type_And_Channel) {
  cv::Mat dst;
  cv::Mat image = cv::imread(test0_);

  std::cout << "Source image \n";
  // 通道数
  std::cout << "Channel: " << image.channels() << "\n";
  // 数据深度（CV_8U, CV_16U等）
  std::cout << "Depth: " << image.depth() << "\n";
  // 通道数和深度
  std::cout << "Type: " << image.type() << "\n";

  std::cout << "Converted image \n";
  image.convertTo(dst, CV_16FC3);
  std::cout << "Channel: " << dst.channels() << "\n";
  std::cout << "Depth: " << dst.depth() << "\n";
  std::cout << "Type: " << dst.type() << "\n";

  std::cout << "cvtColor from BGR 2 Gray \n";
  cv::cvtColor(image, dst, cv::COLOR_BGR2GRAY);
  std::cout << "Channel: " << dst.channels() << "\n";
  std::cout << "Depth: " << dst.depth() << "\n";
  std::cout << "Type: " << dst.type() << "\n";
}

/**
 * 通道的操作
 *
 * 通道分离
 * void cv::split(
 *  const Mat& src,
 *  Mat* mvbegin      # 输出 std:::vector<Mat>
 * )
 *
 * 通道合并
 * void cv::merge(
 *  InputArrayOfArrays mv,
 *  OutputArray dst
 * )
 *
 * 通道混合，重排，拆分，合并的底层函数
 * void cv::mixChannels(
 *  const Mat* src,       # 输入
 *  size_t nsrcs,         # 输入数据的个数
 *  Mat* dst,
 *  size_t ndsts,
 *  const int* fromto,    # 通道映射对数组，每两个整数为一组（from, to）
 *  size_t npairs         # 映射对的个数
 * )
 *
 * fromto:
 *  from: 所有输入矩阵的通道从 0开始编号
 *  to: 所有输出矩阵的通道从 0开始编号
 *  std::vector<int> fromTo = {
 *   0, 2,   // B -> R  // 输入的0通道映射到 输出的2通道
 *   1, 1,   // G -> G
 *   2, 0    // R -> B
 *  };
 * */
TEST_F(Tester, OpenCV_Channel_Operation) {
  cv::Mat dst;
  std::vector<cv::Mat> channels;
  cv::Mat image = cv::imread(test0_);

  cv::split(image, channels);
  cv::merge(channels, dst);
}

/**
 * void cv::inRange(
 *  InputArray src,
 *  InputArray lowerb,
 *  InputArray upperb,
 *  OutputArray dst
 * )
 * 判断图像中的像素是否在指定的范围内，并生成一张二值掩码图
 * lowerb, upperb可以是Mat, Scalar, Int, Vector
 * 当像素值在范围内时，输出255，否则输出0。多通道时，必须同时全部满足
 * 如:
 *  lowerb = Scalar(32, 32, 32)
 *  upperb = Scalar(128, 128, 128)
 *  当一个像素点为 Scalar(16, 90, 250)时
 *  对应位置的mask的值为 Scalar(0, 0, 0)
 *  另一个像素点为 Scalar(90, 90, 90)时
 *  对应位置的mask的值为 Scalar(255, 255, 255)
 * */
TEST_F(Tester, OpenCV_InRange) {
  cv::Mat dst;
  cv::Mat image = cv::imread(test0_);
  cv::Scalar upperb(128, 128, 128);
  cv::Scalar lowerb(32, 32, 32);
  cv::inRange(image, lowerb, upperb, dst);
  cv::imshow("dst", dst);
  cv::waitKey(3000);
  cv::destroyAllWindows();
}

} // namespace cvtest::tester
