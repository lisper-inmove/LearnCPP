message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")

if(WIN32)
  set(OpenCV_DIR "${OPENCV_DIR}")
  set(OpenCV_LIB_PATH "${OPENCV_DIR}/x64/vc18/lib")
endif()

if(LINUX)
  set(OpenCV_DIR "${OPENCV_DIR}/lib/cmake/opencv4")
  set(OpenCV_LIB_PATH "${OPENCV_DIR}/lib")
endif()

message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
message(STATUS "OpenCV_LIB_PATH: ${OpenCV_LIB_PATH}")

# 设置 CUDA 路径（现代方式）
# set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda-13.0")
# set(CUDA_INCLUDE_DIRS "${CUDA_TOOLKIT_ROOT_DIR}/include")
# set(CUDA_CUDART_LIBRARY "${CUDA_TOOLKIT_ROOT_DIR}/lib64/libcudart.so")

# 使用现代 CUDAToolkit 替代旧的 CUDA
# find_package(CUDAToolkit REQUIRED)

# 查找OpenCV库
find_package(OpenCV REQUIRED)

# 添加头文件路径
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加 CUDA 头文件路径
# include_directories(${CUDAToolkit_INCLUDE_DIRS})

# 打印信息用于调试
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")
message(STATUS "OpenCV lib path: ${OpenCV_LIB_PATH}")
# message(STATUS "CUDA include dirs: ${CUDAToolkit_INCLUDE_DIRS}")
message(STATUS "CUDA library dir: ${CUDAToolkit_LIBRARY_DIR}")
