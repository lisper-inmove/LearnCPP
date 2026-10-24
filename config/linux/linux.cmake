message(STATUS "Linux build type: ${CMAKE_BUILD_TYPE}")

# OpenGL 在两种构建类型下都注册目标；Debug 的测试二进制会链接它
include(config/opengl.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Current Build type is Debug")
  include(config/linux/debug.cmake)
  include(config/googletest.cmake)
  message(STATUS "Linux Debug build type")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(config/linux/release.cmake)
endif()

