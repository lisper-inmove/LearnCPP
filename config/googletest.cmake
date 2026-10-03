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
    TBB::tbb
)

# 让 gtest 支持多线程（如果用到 std::thread）
find_package(Threads REQUIRED)
target_link_libraries(${TEST_NAME} PRIVATE Threads::Threads)

# 测试发现
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

set_target_properties(${TEST_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS ${TEST_NAME} DESTINATION bin)

message(STATUS "Add ${TEST_NAME} test success")
