message(STATUS "Windows build type: ${CMAKE_BUILD_TYPE}")

# OpenGL 在两种构建类型下都注册目标；Debug 的测试二进制会链接它
include(config/opengl.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(config/windows/debug.cmake)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(config/windows/release.cmake)
endif()