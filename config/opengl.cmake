# OpenGL 依赖接入
#
# GLFW 3.5.1 从源码构建（sources/glfw-3.5.1），glad 是生成的 GL 4.6 core loader
# （sources/glad：glad.c / glad.h / khrplatform.h）。
#
# 只启用 X11 后端：当前是 X11 会话，Wayland 关掉可以省去 wayland-protocols
# 等额外依赖。docs / tests / examples 都不需要，关掉减少配置和编译时间。

message(STATUS "GLFW source: ${GLFW_SOURCE_DIR}")
message(STATUS "glad source: ${GLAD_SOURCE_DIR}")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

# EXCLUDE_FROM_ALL：只有链接了 glfw 的目标才会触发构建
add_subdirectory(${GLFW_SOURCE_DIR} ${CMAKE_BINARY_DIR}/_deps/glfw EXCLUDE_FROM_ALL)

# glad 是单文件 C 源码，编成静态库
add_library(glad STATIC ${GLAD_SOURCE_DIR}/glad.c)
# glad.h 里用 #include "khrplatform.h"（同目录能解析）；
# 再加父目录是为了业务代码统一写 #include <glad/glad.h>
target_include_directories(glad PUBLIC ${GLAD_SOURCE_DIR}/..)

find_package(OpenGL REQUIRED)

# 业务目标统一链接这个接口库即可
add_library(learncpp_opengl INTERFACE)
target_link_libraries(learncpp_opengl INTERFACE glad glfw OpenGL::GL)

message(STATUS "OpenGL: glad + GLFW 3.5.1 (X11) ready")
