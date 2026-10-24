if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CODE_PATH "F:/codes/third_party")
  else()
    set(CODE_PATH "F:/codes/third_party_debug")
  endif()
endif()

if(LINUX)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CODE_PATH "/home/inmove/code/third_party")
  else()
    set(CODE_PATH "/home/inmove/code/third_party_debug")
  endif()
endif()

message(STATUS "is win32" ${WIN32})
message(STATUS "cmake build type ${CMAKE_BUILD_TYPE}")
message(STATUS "CODE_PATH: ${CODE_PATH}")

set(OPENCV_DIR "${CODE_PATH}/opencv")
set(GOOGLE_TEST_DIR "${CODE_PATH}/googletest")
set(SPDLOG_DIR "${CODE_PATH}/spdlog")
set(YAML_CPP_DIR "${CODE_PATH}/yaml-cpp")

# OpenGL 依赖源码（不在 third_party 里：GLFW 从源码构建，glad 直接编译）
set(GLFW_SOURCE_DIR "/home/inmove/code/sources/glfw-3.5.1")
set(GLAD_SOURCE_DIR "/home/inmove/code/sources/glad")
