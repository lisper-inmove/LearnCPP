if(WIN32)
  # Windows下在vscode中使用 TestMate
  include(CTest)
endif()
enable_testing()

set(TEST_NAME "LearnCPP_Test")
set(GTest_DIR "${GOOGLE_TEST_DIR}/lib/cmake/GTest")

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE TEST_SRC_LIST CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc)

add_executable(${TEST_NAME} tests/main.cpp ${SRC_LIST} ${TEST_SRC_LIST})

# --------------------- GoogleTest -----------------------------
# 用 CMake 官方的 find_package，而不是手动拼 .lib / -lgtest
find_package(GTest REQUIRED)

target_include_directories(${TEST_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)

# 一次性链接所有依赖
target_link_libraries(${TEST_NAME}
    PRIVATE
    GTest::gtest_main     # 已经隐含依赖 GTest::gtest
    ${OpenCV_LIBS}
)

# TBB 仅在存在时链接（Linux 的 OpenCV 自带 TBB 目标，Windows 的没有）
if(TARGET TBB::tbb)
  target_link_libraries(${TEST_NAME} PRIVATE TBB::tbb)
endif()

# 让 gtest 支持多线程（如果用到 std::thread）
find_package(Threads REQUIRED)
target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads)

# Windows 下把运行时 DLL（OpenCV/TBB 等）复制到测试可执行文件旁边，
# 保证 TestMate / ctest 在任何环境下都能直接运行测试
if(WIN32)
  add_custom_command(TARGET ${TEST_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_RUNTIME_DLLS:${TEST_NAME}>
          $<TARGET_FILE_DIR:${TEST_NAME}>
      COMMAND_EXPAND_LISTS
      COMMENT "Copying runtime DLLs next to ${TEST_NAME}"
  )
endif()

# 测试发现
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

set_target_properties(${TEST_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS ${TEST_NAME} DESTINATION bin)

message(STATUS "Add ${TEST_NAME} test success")
