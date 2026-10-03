# 编译安装第三方库

    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/inmove/code/third_party/googletest ..
    cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=/home/inmove/code/sources/opencv_contrib/modules -DCMAKE_INSTALL_PREFIX=/home/inmove/code/third_party/opencv ..
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/inmove/code/third_party/googletest ..
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/inmove/code/third_party/spdlog ..
