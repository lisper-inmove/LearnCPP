# 第三方库根目录:
#   1. 优先沿用外部传入的 PRJ_LIB_ROOT(构建脚本会按 Debug/Release 设置)
#   2. 其次按构建类型推导:third_party_<build-type 小写>
#   3. 最后回退到旧的硬编码路径,保持原有行为
if(NOT DEFINED PRJ_LIB_ROOT OR PRJ_LIB_ROOT STREQUAL "")
  string(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type_lower)
  if(_build_type_lower STREQUAL "")
    set(_build_type_lower "debug")
  endif()
  set(_candidate "/home/JC1136/code/third_party_${_build_type_lower}")
  if(EXISTS "${_candidate}")
    set(PRJ_LIB_ROOT "${_candidate}")
  else()
    set(PRJ_LIB_ROOT "/home/JC1136/code/third_party_debug")
  endif()
endif()

set(CODE_PATH "${PRJ_LIB_ROOT}")

message(STATUS "is win32" ${WIN32})
message(STATUS "cmake build type ${CMAKE_BUILD_TYPE}")
message(STATUS "CODE_PATH: ${CODE_PATH}")

set(OPENCV_DIR "${CODE_PATH}/opencv")
set(GOOGLE_TEST_DIR "${CODE_PATH}/googletest")
set(SPDLOG_DIR "${CODE_PATH}/spdlog")
set(YAML_CPP_DIR "${CODE_PATH}/yaml-cpp")
