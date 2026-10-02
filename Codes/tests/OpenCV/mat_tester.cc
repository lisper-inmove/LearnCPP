/**
 *
 * OpenCV Mat
 *
 * */
#include "tester.h"
#include "gtest/gtest.h"
#include <ios>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <vector>

namespace cvtest::tester {
/**
 *
 * 图片加载与保存
 * Mat cv::imread(const String& filename, int flags = IMREAD_COLOR)
 * bool cv::imwrite(const String& filename, InputArray img, const std::vector<int>& params =
 *  std::vector<int>());
 *  params: 保存图片时需要的优化参数
 *    1. 如果是png
 *      则调整压缩质量的参数为 IMWRITE_PNG_COMPRESSION，参数等级为
 *      0~9，默认值为1。值越大，压缩时间越长，图片越小
 *    2. 如果是jpg
 *      则调整压缩质量的参数为 IMWRITE_JPEG_QUALITY，取值为 0~100，默认95
 *      值越大，图片质量越高，图片越小
 *
 * */
TEST_F(Tester, OpenCV_Image_Load_And_Save) {
  cv::Mat test0_png = cv::imread(test0_, cv::IMREAD_COLOR);
  std::vector<int> opts;

  // 保存为单通道灰度图像
  // opts.push_back(cv::IMWRITE_PAM_FORMAT_GRAYSCALE);

  // 保存为压缩的彩色图像
  // opts.push_back(cv::IMWRITE_PNG_COMPRESSION);
  // opts.push_back(9);

  // 保存为jpg高压缩比图像
  // opts.push_back(cv::IMWRITE_JPEG_QUALITY);
  // opts.push_back(50);
  // opts.push_back(cv::IMWRITE_JPEG_OPTIMIZE);

  // 保存为png,带透明通道
  opts.push_back(cv::IMWRITE_PAM_FORMAT_RGB_ALPHA);

  cv::imwrite(test0New_, test0_png);
  cv::imshow("Test0.png", test0_png);
  cv::waitKey(500);
}

/**
 * 视频的加载与保存
 *
 * 读取摄像头
 * cv::VideoCapture::VideoCapture(
 *  int index,
 *  int apiPreference = CAP_ANY
 * );
 *
 * 读取视频文件
 * cv::VideoCapture::VideoCapture(
 *  const String& filename,
 *  int apiPreference = CAP_ANY
 * )
 *
 * apiPreference: 实际读取视频底层支持库，默认自动检测支持的库
 *  1. CAP_FFMPEG
 *  2. CAP_IMAGES
 *  3. CAP_DSHOW
 *
 * CAP_ANY总是第一选择
 * capture.open(0, CAP_DSHOW);
 * capture.open(filename, CAP_ANY);
 * capture.open(url, CAP_ANY)
 *
 * */
TEST_F(Tester, OpenCV_Load_Video) {
  cv::VideoCapture capture;
  bool flag = capture.open(test0Mp4_, cv::CAP_FFMPEG);
  double fps = capture.get(cv::CAP_PROP_FPS);
  int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
  int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);

  // Four Character Code，4字符组成的代码。
  // 在视频，多媒体里，它用来标识这一段数据是用什么编码格式压缩的
  // 比如 H.264, MPEG-4, MJPEG等
  int fourcc = capture.get(cv::CAP_PROP_FOURCC);
  std::array<char, 5> code = {0};
  code[0] = fourcc & 0xFF;
  code[1] = (fourcc >> 8) & 0xFF;
  code[2] = (fourcc >> 16) & 0xFF;
  code[3] = (fourcc >> 24) & 0xFF;
  std::cout << "FOURCC: " << code[0] << code[1] << code[2] << code[3] << "\n";

  std::cout << std::boolalpha;
  std::cout << "Open file " << flag << "\n";
  cv::VideoWriter writer(test0Mp4New_, fourcc, fps, cv::Size(width, height));
  cv::Mat frame;
  std::cout << "height " << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";
  std::cout << "width " << capture.get(cv::CAP_PROP_FRAME_WIDTH) << "\n";
  std::cout << "fps " << capture.get(cv::CAP_PROP_FPS) << "\n";
  std::cout << "count " << capture.get(cv::CAP_PROP_FRAME_COUNT) << "\n";
  std::cout << "fourcc " << fourcc << "\n";
  while (true) {
    bool ret = capture.read(frame);
    if (!ret)
      break;
    writer.write(frame);
    cv::imshow("frame", frame);
    char c = cv::waitKey(100);
    if (c == 27) {
      break;
    }
  }
  cv::waitKey(100);
  cv::destroyAllWindows();
}
} // namespace cvtest::tester
