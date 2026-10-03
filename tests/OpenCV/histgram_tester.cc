/**
 *
 * histogram
 * 对图像中中灰度级出现频率的统计
 *  横轴通常为0~255
 *  纵轴通常为灰度像素或归一化的频率
 * 它的主要作用如下:
 *  1. 描述图像灰度分布。可以直观看出图像偏暗，偏亮，对比度高低，动态范围是否充分利用
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
 *      cv2.calcBackProject() 可根据目标区域直方图生成概率图，常用于 CamShift、MeanShift
 *      等目标跟踪算法。
 *  7. 作为图像统计特征
 *      由直方图可计算均值、方差、偏度、峰度、熵、能量等特征，用于图像分类、质量评价和内容分析。
 *  8. 评估图像处理效果
 *      对比处理前后的直方图，可以判断增强、拉伸、均衡化等操作是否改善了对比度和灰度分布。
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
} // namespace cvtest::tester
