// OpenGL 接入冒烟测试：验证 GLFW 窗口 + glad 函数加载 + 一个真实 GL 调用。
//
// include 顺序很重要：先 glad 再 GLFW。GLFW_INCLUDE_NONE 阻止 GLFW 自己
// 引入系统 <GL/gl.h>，否则会和 glad 的声明冲突。
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

#include "tester.h"

namespace cvtest::tester {

TEST_F(Tester, OpenGL_Smoke) {
  if (glfwInit() != GLFW_TRUE) {
    GTEST_SKIP() << "GLFW 初始化失败（没有可用的显示环境）";
  }

  // 3.3 core 是学习 OpenGL 的常用基线；本机 Mesa 最高支持 4.1 core
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // 测试不弹窗

  GLFWwindow *window =
      glfwCreateWindow(320, 240, "LearnCPP OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    const char *description = nullptr;
    glfwGetError(&description);
    glfwTerminate();
    FAIL() << "创建 GLFW 窗口失败: " << (description ? description : "unknown");
  }

  glfwMakeContextCurrent(window);

  // glad 通过 GLFW 的 proc address 加载 OpenGL 4.6 的函数指针
  if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
    glfwDestroyWindow(window);
    glfwTerminate();
    FAIL() << "glad 加载 OpenGL 函数失败";
  }

  EXPECT_GE(GLVersion.major, 3);
  EXPECT_NE(glGetString(GL_VERSION), nullptr);

  glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);
  EXPECT_EQ(glGetError(), GL_NO_ERROR);

  glfwDestroyWindow(window);
  glfwTerminate();
}

} // namespace cvtest::tester
