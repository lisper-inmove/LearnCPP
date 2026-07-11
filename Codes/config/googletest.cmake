enable_testing()

# add_definitions(-DUNIT_TEST)

set(TEST_NAME "LearnCPP_Test")

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE TEST_SRC_LIST CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc)


# 将源代码添加到此项目的可执行文件。
add_executable (${TEST_NAME} tests/main.cpp ${SRC_LIST} ${TEST_SRC_LIST})

# --------------------- googletest -----------------------------
file(GLOB GTEST_LIBS "${GTEST_SOURCE_DIR}/build/lib/*.lib")
target_link_libraries(${TEST_NAME} PRIVATE ${GTEST_LIBS})
target_include_directories(${TEST_NAME} PRIVATE "${GTEST_SOURCE_DIR}/googletest/include")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ========== 链接库 ==========
target_link_libraries(${TEST_NAME}
    PRIVATE
    gtest_main      # GoogleTest 主函数
    gtest           # GoogleTest 核心库
    ${OpenCV_LIBS}  # OpenCV 库
)

# 在添加可执行文件后，添加测试发现
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

target_link_libraries(${TEST_NAME} PRIVATE ${OpenCV_LIBS} TBB::tbb)

# 设置输出目录（可选）
set_target_properties(${TEST_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 可选：安装规则
install(TARGETS ${TEST_NAME} DESTINATION bin)

message(STATUS "Add ${TEST_NAME} test success")
