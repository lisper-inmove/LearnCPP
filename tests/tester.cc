#include "tester.h"
#include <algorithm>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace {

const cv::Scalar kBlack(0, 0, 0);
const cv::Scalar kWhite(255, 255, 255);
const cv::Scalar kPanelGray(230, 230, 230);

/**
 * 绘制 ImageJ 风格的绘图区：白色背景 + 黑色轴线 + X轴刻度与数值标签。
 * @param canvas 目标画布
 * @param plot 绘图区矩形
 * @param maxValue X轴最大刻度值（如 255）
 * @param nTicks 刻度数量（含两端，通常为 5）
 */
void drawIjAxes(cv::Mat &canvas, const cv::Rect &plot, int maxValue,
                int nTicks) {
  cv::rectangle(canvas, plot, kWhite, cv::FILLED);

  // 黑色轴线（细实线）
  const int baseline = plot.y + plot.height - 1;
  cv::line(canvas, cv::Point(plot.x, plot.y), cv::Point(plot.x, baseline),
           kBlack);
  cv::line(canvas, cv::Point(plot.x, baseline),
           cv::Point(plot.x + plot.width - 1, baseline), kBlack);

  // X轴刻度线与数值标签
  for (int i = 0; i < nTicks; i++) {
    const int value = cvRound(i * maxValue / (double)(nTicks - 1));
    const int x =
        plot.x + cvRound(value * (plot.width - 1) / (double)maxValue);
    cv::line(canvas, cv::Point(x, baseline + 1), cv::Point(x, baseline + 4),
             kBlack);
    const std::string label = std::to_string(value);
    int fontBaseline = 0;
    const cv::Size textSize = cv::getTextSize(
        label, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &fontBaseline);
    cv::putText(canvas, label, cv::Point(x - textSize.width / 2, baseline + 15),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, kBlack);
  }
}

/**
 * 在绘图区左上角标注 Y 轴最大值、原点处标注 0（ImageJ 风格）。
 */
void drawIjYLabels(cv::Mat &canvas, const cv::Rect &plot, double maxVal) {
  const int baseline = plot.y + plot.height - 1;
  cv::putText(canvas, cv::format("%.0f", maxVal),
              cv::Point(plot.x + 3, plot.y + 12), cv::FONT_HERSHEY_SIMPLEX, 0.4,
              kBlack);
  cv::putText(canvas, "0", cv::Point(plot.x + 3, baseline - 2),
              cv::FONT_HERSHEY_SIMPLEX, 0.4, kBlack);
}

} // namespace

namespace cvtest::tester {
void Tester::SetUp() {}

void Tester::TearDown() {}

/**
 * 绘制一维直方图
 * @param hist 输入的直方图数据（CV_32F类型）
 * @param histName 直方图名称（用于窗口标题）
 * @param histSize 直方图的bins数量
 */
void Tester::drawHistogram2D(const cv::Mat &hist, const std::string &histName,
                             int histSize) {
  // 统计峰值（用于Y轴标注）
  const float *data = hist.ptr<float>();
  const double maxCount = *std::max_element(data, data + histSize);

  // 归一化直方图到绘图区高度（顶部留出标注空间）
  const int hist_w = 512;
  const int hist_h = 420;
  const cv::Rect plot(55, 25, 430, 335);
  const int maxBarHeight = plot.height - 15;
  cv::Mat hist_normalized;
  normalize(hist, hist_normalized, 0, maxBarHeight, cv::NORM_MINMAX);

  // 画布：面板灰底 + 白色绘图区（ImageJ 风格）
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, kPanelGray);
  drawIjAxes(histImage, plot, histSize - 1, 5);

  // 黑色实心柱，柱间留 1px 间隙
  const float *values = hist_normalized.ptr<float>();
  const int baseline = plot.y + plot.height - 1;
  for (int i = 0; i < histSize; i++) {
    const int left = plot.x + cvRound(i * (plot.width - 1) / (double)histSize);
    const int right =
        plot.x + cvRound((i + 1) * (plot.width - 1) / (double)histSize) - 1;
    const int height = cvRound(values[i]);
    cv::rectangle(histImage, cv::Point(left, baseline - height),
                  cv::Point(right, baseline), kBlack, cv::FILLED);
  }

  drawIjYLabels(histImage, plot, maxCount);
  cv::imshow(histName, histImage);
}

void Tester::drawHistogram2D(const cv::Mat &gray, const std::string &histName) {
  const int graySize = 256;
  float grayRange[] = {0, 256};
  const float *grayHistRange[] = {grayRange};
  const int channels[] = {0};
  cv::Mat grayHist;
  cv::calcHist(&gray, 1, channels, cv::Mat(), grayHist, 1, &graySize,
               grayHistRange);
  drawHistogram2D(grayHist, histName, graySize);
}

/**
 * 绘制彩色图像的BGR三通道直方图
 * @param src 输入彩色图像
 * @param winName 窗口标题（多次调用时用不同标题，避免后一次覆盖前一次）
 */
void Tester::drawColorHistogram3Channel(const cv::Mat &src,
                                        const std::string &winName) {
  if (src.empty())
    return;

  // 分离通道（视图，不复制数据）
  std::vector<cv::Mat> bgr_planes;
  split(src, bgr_planes);

  const int histSize = 256;
  float range[] = {0, 256};
  const float *histRange[] = {range};
  const int channels[] = {0};

  // 各通道直方图与绘制颜色（B、G、R）一一对应；
  // 三个通道共用同一Y轴尺度（全局最大计数），柱高真实可比
  cv::Mat hists[3];
  const cv::Scalar colors[] = {cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0),
                               cv::Scalar(0, 0, 255)};
  double maxCount = 0;
  for (int c = 0; c < 3; c++) {
    calcHist(&bgr_planes[c], 1, channels, cv::Mat(), hists[c], 1, &histSize,
             histRange);
    const float *data = hists[c].ptr<float>();
    maxCount =
        std::max(maxCount, (double)*std::max_element(data, data + histSize));
  }

  // 画布：面板灰底 + 白色绘图区（ImageJ 风格）
  const int hist_w = 512;
  const int hist_h = 420;
  const cv::Rect plot(55, 25, 430, 335);
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, kPanelGray);
  drawIjAxes(histImage, plot, histSize - 1, 5);

  // 半透明彩色柱（按 B→G→R 依次叠加，重叠处自然混色）
  const int baseline = plot.y + plot.height - 1;
  const int maxBarHeight = plot.height - 15;
  cv::Mat layer(plot.height, 3, CV_8UC3); // 复用的纯色图层
  for (int c = 0; c < 3; c++) {
    const float *values = hists[c].ptr<float>();
    for (int i = 0; i < histSize; i++) {
      const int left =
          plot.x + cvRound(i * (plot.width - 1) / (double)histSize);
      const int right =
          plot.x + cvRound((i + 1) * (plot.width - 1) / (double)histSize) - 1;
      const int height = cvRound(values[i] * maxBarHeight / maxCount);
      if (height <= 0)
        continue;
      cv::Mat roi = histImage(cv::Rect(left, baseline - height,
                                       right - left + 1, height));
      cv::Mat layerBar = layer(cv::Rect(0, 0, roi.cols, roi.rows));
      layerBar.setTo(colors[c]);
      cv::addWeighted(roi, 0.5, layerBar, 0.5, 0, roi);
    }
  }

  drawIjYLabels(histImage, plot, maxCount);

  // 图例：色块 + 黑色文字，避免仅靠颜色区分通道
  const char *names[] = {"B", "G", "R"};
  for (int c = 0; c < 3; c++) {
    const int ly = plot.y + 8 + c * 14;
    cv::rectangle(histImage,
                  cv::Rect(plot.x + plot.width - 40, ly, 9, 9), colors[c],
                  cv::FILLED);
    cv::putText(histImage, names[c], cv::Point(plot.x + plot.width - 28, ly + 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, kBlack);
  }
  cv::imshow(winName, histImage);
}

/**
 * 绘制带统计信息的详细直方图（ImageJ 风格：缩略图 + 统计面板 + 柱状图）
 * @param gray 输入图像（灰度图）
 */
void Tester::drawDetailedHistogram(const cv::Mat &gray) {
  if (gray.empty())
    return;

  // 计算直方图
  const int histSize = 256;
  float range[] = {0, 256};
  const float *histRange[] = {range};
  const int channels[] = {0};
  cv::Mat hist;
  calcHist(&gray, 1, channels, cv::Mat(), hist, 1, &histSize, histRange);

  // 统计信息（ImageJ 风格面板）
  const float *data = hist.ptr<float>();
  const float *maxIt = std::max_element(data, data + histSize);
  const double maxCount = *maxIt;
  const int modeBin = (int)(maxIt - data); // 众数灰度级
  double minVal, maxVal;
  cv::minMaxLoc(gray, &minVal, &maxVal);
  cv::Scalar mean, stddev;
  meanStdDev(gray, mean, stddev);

  // 画布：面板灰底
  const int hist_w = 800;
  const int hist_h = 520;
  cv::Mat histImage(hist_h, hist_w, CV_8UC3, kPanelGray);

  // 左上角：图像缩略图（等比缩放到 100x100 框内）
  const int thumbBox = 100;
  cv::Mat thumb;
  const double scale = std::min(thumbBox / (double)gray.cols,
                                thumbBox / (double)gray.rows);
  cv::resize(gray, thumb, cv::Size(), scale, scale, cv::INTER_AREA);
  cv::rectangle(histImage, cv::Rect(12, 12, thumbBox + 4, thumbBox + 4),
                kBlack);
  cv::rectangle(histImage, cv::Rect(14, 14, thumbBox, thumbBox), kWhite,
                cv::FILLED);
  const cv::Point thumbTopLeft(14 + (thumbBox - thumb.cols) / 2,
                               14 + (thumbBox - thumb.rows) / 2);
  cv::Mat thumbBgr;
  cv::cvtColor(thumb, thumbBgr, cv::COLOR_GRAY2BGR); // copyTo 要求通道数一致
  thumbBgr.copyTo(
      histImage(cv::Rect(thumbTopLeft.x, thumbTopLeft.y, thumb.cols, thumb.rows)));

  // 右侧：统计信息
  const std::string stats[] = {
      cv::format("Count: %d", (int)gray.total()),
      cv::format("Mean: %.2f", mean[0]),
      cv::format("StdDev: %.2f", stddev[0]),
      cv::format("Bins: %d", histSize),
      cv::format("Min: %.0f", minVal),
      cv::format("Max: %.0f", maxVal),
      cv::format("Mode: %d (%.0f)", modeBin, maxCount),
  };
  for (int i = 0; i < 7; i++)
    cv::putText(histImage, stats[i], cv::Point(140, 32 + i * 18),
                cv::FONT_HERSHEY_SIMPLEX, 0.45, kBlack);

  // 绘图区（ImageJ 风格）：白色背景 + 黑色轴线
  const cv::Rect plot(60, 165, 720, 325);
  drawIjAxes(histImage, plot, histSize - 1, 5);

  // 黑色实心柱，柱间留 1px 间隙
  const int baseline = plot.y + plot.height - 1;
  const int maxBarHeight = plot.height - 15;
  cv::Mat hist_normalized;
  normalize(hist, hist_normalized, 0, maxBarHeight, cv::NORM_MINMAX);
  const float *values = hist_normalized.ptr<float>();
  for (int i = 0; i < histSize; i++) {
    const int left = plot.x + cvRound(i * (plot.width - 1) / (double)histSize);
    const int right =
        plot.x + cvRound((i + 1) * (plot.width - 1) / (double)histSize) - 1;
    const int height = cvRound(values[i]);
    cv::rectangle(histImage, cv::Point(left, baseline - height),
                  cv::Point(right, baseline), kBlack, cv::FILLED);
  }

  drawIjYLabels(histImage, plot, maxCount);
  imshow("Detailed Histogram", histImage);
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
