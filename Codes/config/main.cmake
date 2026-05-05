file(GLOB_RECURSE SOURCES "src/*.cc")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp)

# 设置输出目录（可选）
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 添加头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接OpenCV库
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# 可选：安装规则
install(TARGETS ${PROJECT_NAME} DESTINATION bin)