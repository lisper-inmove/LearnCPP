# 编译安装第三方库

    需要编译安装的第三方库，包括opencv,googletest,spdlog,yaml-cpp
    以下以googletest为例

## 1. 配置（去掉无效的 CMAKE_BUILD_TYPE）

  cmake ..

## 2. 编译

  cmake --build . --config Debug -j 12

## 3. 安装

  cmake --install . --config Debug --prefix F:/codes/third_party_debug/googletest

# OpenCV 需要在配置时指定 download mirror

    cmake -DOPENCV_EXTRA_MODULES_PATH=F:/codes/sources/opencv_contrib/modules -DOPENCV_DOWNLOAD_MIRROR_ID=gitcode ..
