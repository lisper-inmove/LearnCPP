#include "tester.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace cvtest::tester {
void Tester::SetUp() {}

void Tester::TearDown() {}

/**
 * 绘制一维直方图
 * @param hist 输入的直方图数据（CV_32F类型）
 * @param histName 直方图名称（用于窗口标题）
 * @param histSize 直方图的bins数量
 * @param color 绘制颜色（BGR格式）
 */
void Tester::drawHistogram2D(const cv::Mat &hist, const std::string &histName, int histSize) {
  // 归一化直方图到 0-400 高度范围
  cv::Mat hist_normalized;
  normalize(hist, hist_normalized, 0, 400, cv::NORM_MINMAX, -1, cv::Mat());

  // 创建画布（黑色背景）
  int hist_w = 512;
  int hist_h = 400;
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

  // 计算每个bin的宽度
  auto value = static_cast<double>(hist_w);
  int bin_w = cvRound(value / histSize);
  cv::Scalar color = cv::Scalar(0, 255, 0);

  // 绘制直方图
  for (int i = 1; i < histSize; i++) {
    line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist_normalized.at<float>(i - 1))),
         cv::Point(bin_w * i, hist_h - cvRound(hist_normalized.at<float>(i))), color, 2, 8, 0);
  }

  imshow(histName, histImage);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 绘制彩色图像的BGR三通道直方图
 * @param src 输入彩色图像
 */
void Tester::drawColorHistogram3Channel(const cv::Mat &src) {
  if (src.empty())
    return;

  // 分离通道
  std::vector<cv::Mat> bgr_planes;
  split(src, bgr_planes);

  int histSize = 256;
  float range[] = {0, 256};
  const float *histRange = {range};
  int channels[] = {0};

  cv::Mat b_hist, g_hist, r_hist;
  calcHist(&bgr_planes[0], 1, channels, cv::Mat(), b_hist, 1, &histSize, &histRange);
  calcHist(&bgr_planes[1], 1, channels, cv::Mat(), g_hist, 1, &histSize, &histRange);
  calcHist(&bgr_planes[2], 1, channels, cv::Mat(), r_hist, 1, &histSize, &histRange);

  // 归一化
  int hist_h = 400;
  int hist_w = 512;
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

  normalize(b_hist, b_hist, 0, hist_h, cv::NORM_MINMAX);
  normalize(g_hist, g_hist, 0, hist_h, cv::NORM_MINMAX);
  normalize(r_hist, r_hist, 0, hist_h, cv::NORM_MINMAX);

  int bin_w = cvRound((double)hist_w / histSize);

  // 绘制三条曲线
  for (int i = 1; i < histSize; i++) {
    // 蓝色通道
    line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
         cv::Point(bin_w * i, hist_h - cvRound(b_hist.at<float>(i))), cv::Scalar(255, 0, 0), 2);

    // 绿色通道
    line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
         cv::Point(bin_w * i, hist_h - cvRound(g_hist.at<float>(i))), cv::Scalar(0, 255, 0), 2);

    // 红色通道
    line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
         cv::Point(bin_w * i, hist_h - cvRound(r_hist.at<float>(i))), cv::Scalar(0, 0, 255), 2);
  }

  imshow("Color Histogram", histImage);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

/**
 * 绘制带统计信息的详细直方图
 * @param src 输入图像（灰度图）
 */
void Tester::drawDetailedHistogram(const cv::Mat &gray) {
  if (gray.empty())
    return;

  // 计算直方图
  cv::Mat hist;
  int histSize = 256;
  float range[] = {0, 256};
  int channels[] = {0};
  const float *histRange = {range};
  calcHist(&gray, 1, channels, cv::Mat(), hist, 1, &histSize, &histRange);

  // 计算统计信息
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  minMaxLoc(hist, &minVal, &maxVal, &minLoc, &maxLoc);

  // 计算均值、标准差
  cv::Scalar mean, stddev;
  meanStdDev(gray, mean, stddev);

  // 创建画布（加大尺寸以显示文字）
  int hist_h = 450;
  int hist_w = 800;
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(50, 50, 50));

  // 归一化直方图
  cv::Mat hist_normalized;
  normalize(hist, hist_normalized, 0, hist_h - 50, cv::NORM_MINMAX);

  // 绘制网格线
  for (int i = 0; i <= 4; i++) {
    int y = hist_h - 50 - i * (hist_h - 50) / 4;
    line(histImage, cv::Point(60, y), cv::Point(hist_w - 10, y), cv::Scalar(100, 100, 100), 1);
    // 添加Y轴标签
    std::string label = std::to_string(i * (int)maxVal / 4);
    putText(histImage, label, cv::Point(10, y + 5), cv::FONT_HERSHEY_SIMPLEX, 0.5,
            cv::Scalar(200, 200, 200));
  }

  int bin_w = cvRound((double)(hist_w - 70) / histSize);

  // 绘制柱状图（使用矩形）
  for (int i = 0; i < histSize; i++) {
    int height = cvRound(hist_normalized.at<float>(i));
    rectangle(histImage, cv::Point(60 + bin_w * i, hist_h - 50),
              cv::Point(60 + bin_w * (i + 1), hist_h - 50 - height), cv::Scalar(100, 100, 255), -1);
  }

  // 绘制X轴和Y轴
  line(histImage, cv::Point(50, hist_h - 50), cv::Point(hist_w - 10, hist_h - 50),
       cv::Scalar(255, 255, 255), 2);
  line(histImage, cv::Point(50, 30), cv::Point(50, hist_h - 50), cv::Scalar(255, 255, 255), 2);

  // 添加统计信息文字
  std::string info =
      cv::format("Mean: %.2f  StdDev: %.2f  Max Count: %.0f", mean[0], stddev[0], maxVal);
  putText(histImage, info, cv::Point(60, 30), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0),
          2);

  // 添加X轴标签
  putText(histImage, "Pixel Intensity", cv::Point(hist_w / 2, hist_h - 10),
          cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(200, 200, 200));

  // 添加Y轴标签
  putText(histImage, "Pixel Count", cv::Point(15, hist_h / 2), cv::FONT_HERSHEY_SIMPLEX, 0.6,
          cv::Scalar(200, 200, 200), 1, 8, true);

  imshow("Detailed Histogram", histImage);
  cv::waitKey(0);
  cv::destroyAllWindows();
}

int Tester::sharpenValue(cv::Mat &image) {
  /**
   * 计算图像的锐度，锐度越大，图像越清晰
   * */
  cv::Mat gray;
  int h = image.rows;
  int w = image.cols;
  float sum = 0;
  cv::cvtColor(image, gray, cv::COLOR_RGBA2GRAY);
  for (int row = 1; row < h - 1; row++) {
    for (int col = 1; col < w - 1; col++) {
      int dx = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row, col + 1) -
               gray.at<uchar>(row, col - 1);
      int dy = gray.at<uchar>(row, col) * 2 - gray.at<uchar>(row + 1, col) -
               gray.at<uchar>(row - 1, col);
      sum += (abs(dx) + abs(dy));
    }
  }
  return sum;
}

int Tester::calculateLaplacianSum(const cv::Mat &src) {

  int sum = 0;
  int step = 1;
  int h = src.rows;
  int w = src.cols;
  for (int i = 1; i < h - 1; i++) {
    for (int j = 1; j < w - 1; j++) {
      uint8_t v1 = src.at<uchar>(i, j);
      uint8_t v2 = src.at<uchar>(i - step, j);
      uint8_t v3 = src.at<uchar>(i, j - step);
      uint8_t v4 = src.at<uchar>(i + step, j);
      uint8_t v5 = src.at<uchar>(i, j + step);
      sum += abs(v1 * 2 - v2 - v4);
      sum += abs(v1 * 2 - v3 - v5);
    }
  }
  return sum;
}

} // namespace cvtest::tester
