# 第三方库

    see Code/docs

# use makefile

    1. make list-all-tests: Display all test-cases
       ```$ make list-all-tests
       All test cases in LearnCPP_Test:
       ========================================
       Tester.
       QtSupportTest
       ```

    2. run all test-cases in file
       ```shell
       $ make CH1_basic_operation        <- filename in tests directory
       ========================================
       Running all tests in CH1_basic_operation.cc
       ========================================
       Note: Google Test filter = *NamedWindowTest*:*ReadWriteTest*:*OpenVideo*
       [==========] Running 3 tests from 1 test suite.
       [----------] Global test environment set-up.
       [----------] 3 tests from Tester
       [ RUN      ] Tester.NamedWindowTest
       [       OK ] Tester.NamedWindowTest (976 ms)
       [ RUN      ] Tester.ReadWriteTest
       [       OK ] Tester.ReadWriteTest (837 ms)
       [ RUN      ] Tester.OpenVideo
       Video Info:
           FPS: 29
           Total Frames: 713
           Resolution: 720x540
           Press 'q' or 'ESC' to quit
           [       OK ] Tester.OpenVideo (1353 ms)
           [----------] 3 tests from Tester (3167 ms total)
       [----------] Global test environment tear-down
       [==========] 3 tests from 1 test suite ran. (3167 ms total)
       [  PASSED  ] 3 tests.
       ```
    3. run a test-case

    ```
    $ make test-pattern-NamedWindowTest
    Running tests matching pattern: NamedWindowTest
    Note: Google Test filter = *NamedWindowTest*
    [==========] Running 1 test from 1 test suite.
    [----------] Global test environment set-up.
    [----------] 1 test from Tester
    [ RUN      ] Tester.NamedWindowTest
    [       OK ] Tester.NamedWindowTest (781 ms)
    [----------] 1 test from Tester (781 ms total)

    [----------] Global test environment tear-down
    [==========] 1 test from 1 test suite ran. (781 ms total)
    [  PASSED  ] 1 test.

    ```

# 当系统gcc版本太低时的解决

  可以，已经搞定 —— 完全不用碰系统（无 sudo、无系统级安装，全部在 ~/ 下）。
     
  方案

  用 micromamba（单文件、装到 ~/micromamba）+ conda-forge 的 GCC 16.2.0 工具链（装到 ~/cpp20），完整支持 C++20/23。删除这两个目录即可完全还原。

  过程中改的东西

  工具链适配（1 处）：conda 自带 glibc 2.28 sysroot，而预编译的 OpenCV 是按 glibc 2.31 编译的（引用了 pow@GLIBC_2.29 等符号），链接会失败。我把 conda sysroot 的 lib64/usr/lib64
  重定向到系统 /lib/x86_64-linux-gnu（原目录保留为 .orig），只改了 ~/cpp20 内部，不影响系统。

  项目代码（2 处，都是真实问题）：
  1. tests/template_tester.cc — SimpleWrapper<T> 缺带参构造函数（编译 bug）。
  2. config/googletest.cmake — 补上 find_package(Threads REQUIRED)（GTest 的 config 引用了 Threads::Threads）。

  结果

  - ✅ LearnCPP 和 LearnCPP_Test 两个目标全部编译通过（仅剩若干无用的变量/参数警告，是原有的）。
  - ✅ 测试 89/101 通过。

  剩下的 12 个失败——都与 C++20 无关

  - 2 个是故意崩溃/死锁的测试（CIA_ThreadTerminate、CIA_HierarchicalMutexTester，设计上就 abort）。
  - 10 个是 OpenCV 测试，因为 include/tester.h:13 硬编码了资源路径 /home/inmove/code/LearnCPP/assets/（是别人机器的路径），本机应为 /home/JC1136/code/LearnCPP/assets/。

  以后编译命令

  cmake -S . -B build-Debug-cpp20 -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_COMPILER=/home/JC1136/cpp20/bin/x86_64-conda-linux-gnu-g++ \
    -DCMAKE_CXX_FLAGS="-pthread" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,-rpath-link=/usr/lib/x86_64-linux-gnu -Wl,-rpath-link=/lib/x86_64-linux-gnu" \
    -DCMAKE_BUILD_RPATH=/home/JC1136/cpp20/lib
  cmake --build build-Debug-cpp20 --parallel

