/**
 *
 * histogram
 * 对图像中中灰度级出现频率的统计
 *  横轴通常为0~255
 *  纵轴通常为灰度像素或归一化的频率
 * 它的主要作用如下:
 *  1.
 * 描述图像灰度分布。可以直观看出图像偏暗，偏亮，对比度高低，动态范围是否充分利用
 *      如果像素集中在低灰度区说明图像偏暗，集中在窄区说明对比度低
 *  2. 判断图像质量与曝光情况
 *      直方图可辅助分析是否欠曝，过曝，低对比度或灰度分布不均
 *  3. 指导图像增强
 *      直方图均衡化 cv2.equalizeHist()
 *      通过重新分配灰度级来增强对比度；直方图规定化/匹配可用于让图像具有指定灰度分布。
 *  4. 辅助阈值分割
 *      很多阈值方法基于灰度直方图，如双峰法、Otsu 法。Otsu
 *      通过最大化类间方差自动选择阈值，常用于二值化。
 *  5. 图像相似度比较与检索
 *      用 cv2.compareHist()
 *      比较两幅图像直方图，可判断灰度分布是否相似，常用于图像检索、匹配和简单分类。它对平移、旋转较不敏感，但丢失空间信息。
 *  6. 直方图反向投影与目标跟踪
 *      cv2.calcBackProject() 可根据目标区域直方图生成概率图，常用于
 * CamShift、MeanShift 等目标跟踪算法。
 *  7. 作为图像统计特征
 *      由直方图可计算均值、方差、偏度、峰度、熵、能量等特征，用于图像分类、质量评价和内容分析。
 *  8. 评估图像处理效果
 *      对比处理前后的直方图，可以判断增强、拉伸、均衡化等操作是否改善了对比度和灰度分布。
 * */
#include "tester.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>

namespace cvtest::tester {
/**
 * 计算图像均值函数，数据总和除以个数
 * Scalar cv::mean(
 *  InputArray src,
 *  InputArray mask = noArray()
 * )
 * 返回值为Scalar(BlueMean, GreenMean, ReadMean, AlphaMean)
 *
 * 计算图像均值与方差函数
 * 方差: (值 - 均值) ^ 2 / 个数
 * Scalar cv::meanStdDev(
 *  InputArray src,
 *  OutputArray mean,
 *  OutputArray stddev,
 *  InputArray mask = noArray()
 * )
 *
 * 方差越大，数据越分散
 * 方差越小，数据越集中
 * */
TEST_F(Tester, OpenCV_Pixel_Info_Statistics) {
  cv::Mat src = cv::imread(test0_, cv::IMREAD_COLOR);
  cv::Scalar mean = cv::mean(src);
  std::cout << "Image mean " << mean << "\n";
  // cv::Mat dst; // 用Mat时返回3x1的数据
  cv::Scalar dst; // 返回1x4的数据，与mean函数的含义一致
  cv::meanStdDev(src, mean, dst);
  std::cout << "Image meanStdDev " << dst << "\n";
}

/**
 * cv::calcHist(
 *  const Mat* images,
 *  int nimages,
 *  const int* channels,
 *  InputArray mask,
 *  OutputArray hist,
 *  int dims,
 *  const int* histSize,
 *  const float** ranges,
 *  bool uniform = true,
 *  bool accumulate = false
 * )
 * images: 输入图像，一张或多张，通道与类型一致
 * nimages: 图像张数
 * channels: 不同图像的通道索引，编号从0开始
 * mask: 掩码
 * hist: 输出直方图
 * dims: 必须是正整数
 * histSize: 直方图大小，可以理解为X轴上的直方图的取值范围
 *            假设为0~31，那么这32个级别会将0~255分成32份
 * ranges: 通道的取值范围RGB通常为0~256, HSV通常为 0~180
 *  float h_ranges[] = {0, 180};
 *  float s_ranges[] = {0, 256};
 *  float *ranges[] = {h_ranges, s_ranges};
 * uniform: 一致性，对边界数据的处理方式，取false表示不处理
 * accumulate: 是否计算累积直方图
 * */
TEST_F(Tester, OpenCV_Cal_Histogram) {
  cv::Mat src = cv::imread(test0_, cv::IMREAD_COLOR);
  drawColorHistogram3Channel(src);

  cv::Mat gray, grayHist;
  cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  int graySize = 256;
  float grayRange[] = {0, 256};
  const float *grayHistRange = {grayRange};
  int channels[] = {0};
  cv::calcHist(&gray, 1, channels, cv::Mat(), grayHist, 1, &graySize,
               &grayHistRange);
  drawHistogram2D(grayHist, "Gray Histgram", 256);

  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 直方图均衡化：调整图像的对比度，使得灰度值分布更加均匀
 * cv::equalizeHist(
 *  InputArray src,
 *  OutputArray dst
 * )
 * src: 输入灰度图像
 * dst: 输出均衡化后的图像
 * */
TEST_F(Tester, OpenCV_Equalize_Hist) {
  cv::Mat src = cv::imread(test0_, cv::IMREAD_GRAYSCALE);
  cv::Mat dst;
  cv::equalizeHist(src, dst);
  cv::imshow("Original", src);
  cv::imshow("Equalized", dst);
  drawHistogram2D(src, "Original Histgram");
  drawHistogram2D(dst, "Equalized Histgram");
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 局部自适应直方图均衡化：对图像的局部区域进行对比度增强，使得局部细节更加清晰 
 */
TEST_F(Tester, OpenCV_Adaptive_Equalize_Hist) {
  cv::Mat src = cv::imread(test0_, cv::IMREAD_GRAYSCALE);
  cv::Mat dst;

  // tileGridSize: 表示把输入图像分为多个 8x8 的像素风格
  cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));

  // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
  // // 对比度限制参数，默认是40
  // // 直方图均衡化容易放大噪声，通过"裁剪"过高的直方图峰值，抑制噪声被过度增强。
  // // 值越大，对比度增强越强；值越小，越平滑
  // clahe->setClipLimit(4);

  // 将均衡化应用到图片上，并保存在dst
  clahe->apply(src, dst);
  cv::imshow("Original", src);
  cv::imshow("Adaptive Equalized", dst);
  drawHistogram2D(src, "Original Histgram");
  drawHistogram2D(dst, "Adaptive Equalized Histgram");
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 彩色图像直方图均衡化
 *  先将图像转换成HSV色彩空间
 *  然后对V通道进行直方图均衡化
 */
TEST_F(Tester, OpenCV_Color_Equalize_Hist) {
  cv::Mat src = cv::imread(test0_, cv::IMREAD_COLOR);
  cv::Mat hsv, dst;
  // 将BGR转成HSV
  cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
  // 通道分离
  std::vector<cv::Mat> channels;
  cv::split(hsv, channels);
  // 将V通道进行直方图均衡化
  cv::equalizeHist(channels[2], channels[2]);
  // 合并通道并转换回HSV
  cv::merge(channels, hsv);
  // 将HSV图像转换回BGR图像
  cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);
  cv::imshow("Original", src);
  cv::imshow("Equalized", dst);
  // 两个直方图用不同窗口标题，避免后一次 imshow 覆盖前一次
  drawColorHistogram3Channel(src, "Original Color Histgram");
  drawColorHistogram3Channel(dst, "Equalized Color Histgram");
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 直方图的比较
 * double cv::compareHist(
 *  InputArray h1, InputArray h2, int method
 * )
 * 1. h1, h2 是两个图片直方图的数据
 * 2. method:
 *  HISTCMP_CORREL: 相关性相似比较
 *  HISTCMP_CHISQR: 卡方相似性比较
 *  HIST_CMP_INTERSECT: 交叉相似比较
 *  HISTCMP_BHATTACHARYYA: 巴氏距离相似性比较
 *  HISTCMP_HELLINGER: 海林格距离相似比较
 *  HISTCMP_CHISQR_ALT: 可变卡方相似比较
 *  HISTCMP_KL_DIV: 基于KL散度相似比较
 * */
TEST_F(Tester, OpenCV_Hist_Comppare) {
  cv::Mat src1 = cv::imread(test0_, cv::IMREAD_COLOR);
  cv::Mat src2 = cv::imread(test0New_, cv::IMREAD_COLOR);

  cv::Mat hsv1, hsv2;
  cv::cvtColor(src1, hsv1, cv::COLOR_BGR2HSV);
  cv::cvtColor(src2, hsv2, cv::COLOR_BGR2HSV);

  int h_bins = 60;
  int s_bins = 64;

  int histSize[] = {h_bins, s_bins};

  // h通道, 0~180
  float h_ranges[] = {0, 180};
  // s通道, 0~256
  float s_ranges[] = {0, 256};

  const float *ranges[] = {h_ranges, s_ranges};
  int channels[] = {0, 1};
  cv::Mat hist1, hist2;
  cv::calcHist(&hsv1, 1, channels, cv::Mat(), hist1, 2, histSize, ranges, true,
               false);
  cv::calcHist(&hsv2, 1, channels, cv::Mat(), hist2, 2, histSize, ranges, true,
               false);

  cv::normalize(hist1, hist1, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  cv::normalize(hist2, hist2, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  std::string methods[] = {"HISTCMP_CORREL", "HISTCMP_CHISQR",
                           "HISTCMP_INTERSECT", "HISTCMP_BHATTACHARYYA"};

  for (int i = 0; i < 4; i++) {
    int compare_method = i;
    double src1_src2 = cv::compareHist(hist1, hist2, compare_method);
    std::cout << " Method [" << methods[i].c_str() << "]: " << src1_src2
              << "\n";
  }
}

/**
 * 直方图反向投影
 * 常用于图像定位与分割
 * 选取一块目标区域做为模板。然后在全图中查找哪些像素的颜色与模板相似
 * 通常基于HSV色彩空间的H,S得到2D直方图来实现
 *
 * void cv::calcBackProject(
 *  const Mat *images,
 *  int nimages,
 *  const int *channels,
 *  InputArray hist,
 *  OutputArray output, // 反向投影之后的输出
 *  const float **ranges,
 *  double scale = 1,
 *  bool uniform = true
 * )
 *
 * */
TEST_F(Tester, OpenCV_Back_Pprojection) {
  cv::Mat src = cv::imread(grape_);
  cv::Mat tpl = cv::imread(grapeTplp_);
  cv::Mat hsvSrc, hsvTpl;
  cv::cvtColor(src, hsvSrc, cv::COLOR_BGR2HSV);
  cv::cvtColor(tpl, hsvTpl, cv::COLOR_BGR2HSV);
  int h_bins = 32;
  int s_bins = 32;
  int histSize[] = {h_bins, s_bins};
  float h_ranges[] = {0, 180};
  float s_ranges[] = {0, 256};
  const float *ranges[] = {h_ranges, s_ranges};
  int channels[] = {0, 1};
  cv::Mat roiHist;
  cv::calcHist(&hsvTpl, 1, channels, cv::Mat(), roiHist, 2, histSize, ranges,
               true, false);
  cv::normalize(roiHist, roiHist, 0, 255, cv::NORM_MINMAX, -1, cv::Mat());
  cv::Mat backgroj;
  cv::calcBackProject(&hsvSrc, 1, channels, roiHist, backgroj, ranges, 1.0);
  cv::imshow("Backproj", backgroj);
  cv::imshow("Source", src);
  cv::waitKey(3000);
  cv::destroyAllWindows();
}

/**
 * 累积直方图计算
 * @hist: 计算好的直方图
 * */
static std::vector<double> computeCDF(const cv::Mat &hist) {
  /*
   * hist.total() 返回矩阵元素总数，rows * cols * channels
   * */
  std::vector<double> cdf(hist.total());
  double sum = 0.0;
  for (int i = 0; i < (int)hist.total(); ++i) {
    sum += hist.at<float>(i);
    cdf[i] = sum;
  }
  if (sum > 0) {
    for (auto &v : cdf)
      v /= sum;
  }
  return cdf;
}

/**
 * 直方图匹配
 * 把源图像的直方图变换成与参考图像直方图相近的形状
 * 常用于图像拼接，色彩一致性处理，医学图像增强等
 *
 * 1. 计算源图，参考图直方图
 * 2. 分别求两者的累积分布函数(CDF)
 * 3. 对源图的每个灰度级i，在参考CDF中找到最接近的灰度级，建立映射关系 map[i] = j
 * 4. 用该映射对源图每个像素做替换
 * */
TEST_F(Tester, OpenCV_Hist_Match) {
  cv::Mat image = cv::imread(test0_, cv::IMREAD_GRAYSCALE);
  cv::Mat tpl = cv::imread(tpl_, cv::IMREAD_GRAYSCALE);

  int histSize = 256;
  float range[] = {0, 256};
  const float *histRange = {range};

  cv::Mat histImage, histTpl;
  cv::calcHist(&image, 1, 0, cv::Mat(), histImage, 1, &histSize, &histRange,
               true, false);
  cv::calcHist(&tpl, 1, 0, cv::Mat(), histTpl, 1, &histSize, &histRange, true,
               false);

  std::vector<double> cdfImage = computeCDF(histImage);
  std::vector<double> cdfTpl = computeCDF(histTpl);

  std::vector<uchar> lookup(256);
  for (int i = 0; i < 256; i++) {
    double minDiff = 1e10;
    int bestJ = 0;
    for (int j = 0; j < 256; j++) {
      double diff = std::abs(cdfImage[i] - cdfTpl[j]);
      if (diff < minDiff) {
        minDiff = diff;
        bestJ = j;
      }
    }
    lookup[i] = static_cast<uchar>(bestJ);
  }

  cv::Mat dst = image.clone();
  for (int y = 0; y < dst.rows; y++) {
    for (int x = 0; x < dst.cols; x++) {
      dst.at<uchar>(y, x) = lookup[image.at<uchar>(y, x)];
    }
  }

  cv::imshow("Image", image);
  cv::imshow("Tpl", tpl);
  cv::imshow("Matched", dst);

  cv::waitKey(0);
  cv::destroyAllWindows();
}

} // namespace cvtest::tester
