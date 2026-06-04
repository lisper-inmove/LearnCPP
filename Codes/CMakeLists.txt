cmake_minimum_required(VERSION 3.16)
project(LearnCPP VERSION 1.0.0 LANGUAGES CXX)

# 设置C++23标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成编译命令数据库（用于VSCode等IDE）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # 启用更多警告
  add_compile_options(-Wall -Wextra -Wpedantic)

  # 对于Debug模式，启用调试信息
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
  endif()

  # 对于Release模式，启用优化
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
  endif()
endif()

include(config/opencv.cmake)
include(config/main.cmake)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++23)

if(WIN32)
  include(config/windows/windows.cmake)
endif()

if(LINUX)
  include(config/linux/linux.cmake)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to source directory"
)
