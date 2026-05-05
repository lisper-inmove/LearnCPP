#!/bin/bash

# 自动生成测试规则的脚本
# 用法: ./scripts/generate_test_rules.sh

TEST_DIR="tests"
OUTPUT_FILE="makes/LearnCPP_Test.mk"
EXECUTABLE="LearnCPP_Test"

# 确保 makes 目录存在
mkdir -p makes

# 获取所有测试文件（排除 pch.cc, tester.cc, main.cc）
TEST_FILES=$(ls $TEST_DIR/*.cc 2>/dev/null | grep -v "pch.cc" | grep -v "tester.cc" | grep -v "main.cc" | xargs -n1 basename | sed 's/\.cc$//')

# 开始生成 Makefile
cat > $OUTPUT_FILE << 'EOF'
# ============================================
# LearnCPP_Test 测试模块配置
# 此文件由 scripts/generate_test_rules.sh 自动生成
# 请勿手动编辑，运行脚本即可更新
# ============================================

TEST_EXECUTABLE := LearnCPP_Test
TEST_SRC_DIR := tests

# 构建目录检测
ifneq ($(wildcard build-Debug),)
    BUILD_DIR_DEBUG_ACTUAL := build-Debug
else
    BUILD_DIR_DEBUG_ACTUAL := build-debug
endif

TEST_EXEC_DEBUG := $(firstword $(wildcard $(BUILD_DIR_DEBUG_ACTUAL)/bin/$(TEST_EXECUTABLE)) $(wildcard $(BUILD_DIR_DEBUG_ACTUAL)/$(TEST_EXECUTABLE)))

# ============================================
# 构建检查
# ============================================

build-debug-check:
	@if [ ! -f "$(TEST_EXEC_DEBUG)" ]; then \
		echo "Building Debug version..."; \
		$(MAKE) debug; \
	fi

EOF

# 为每个测试文件生成规则
for test_file in $TEST_FILES; do
    # 提取测试用例名称
    test_cases=$(grep -oP 'TEST_F\(\s*\w+\s*,\s*\K[^)]+' $TEST_DIR/${test_file}.cc 2>/dev/null | tr '\n' ' ')
    
    if [ -n "$test_cases" ]; then
        # 生成批量运行的规则
        echo "# ============================================" >> $OUTPUT_FILE
        echo "# $test_file 测试规则" >> $OUTPUT_FILE
        echo "# ============================================" >> $OUTPUT_FILE
        echo "" >> $OUTPUT_FILE
        
        echo ".PHONY: $test_file" >> $OUTPUT_FILE
        echo "$test_file: build-debug-check" >> $OUTPUT_FILE
        echo "	@echo \"========================================\"" >> $OUTPUT_FILE
        echo "	@echo \"Running all tests in $test_file.cc\"" >> $OUTPUT_FILE
        echo "	@echo \"========================================\"" >> $OUTPUT_FILE
        
        # 生成过滤模式
        filter_pattern=""
        for test_case in $test_cases; do
            if [ -z "$filter_pattern" ]; then
                filter_pattern="*$test_case*"
            else
                filter_pattern="$filter_pattern:*$test_case*"
            fi
        done
        
        echo "	@\$(TEST_EXEC_DEBUG) --gtest_filter=\"$filter_pattern\"" >> $OUTPUT_FILE
        echo "" >> $OUTPUT_FILE
        
        # 为每个测试用例生成单独运行的规则
        for test_case in $test_cases; do
            echo ".PHONY: $test_file.$test_case" >> $OUTPUT_FILE
            echo "$test_file.$test_case: build-debug-check" >> $OUTPUT_FILE
            echo "	@echo \"Running $test_case...\"" >> $OUTPUT_FILE
            echo "	@\$(TEST_EXEC_DEBUG) --gtest_filter=\"*$test_case*\"" >> $OUTPUT_FILE
            echo "" >> $OUTPUT_FILE
        done
        
        echo "" >> $OUTPUT_FILE
    fi
done

# 添加辅助功能
cat >> $OUTPUT_FILE << 'EOF'

# ============================================
# 辅助功能
# ============================================

.PHONY: list-test-files
list-test-files:
	@echo "Available test files:"
	@for file in $(TEST_FILES); do echo "  - $$file"; done
	@echo ""
	@echo "Usage examples:"
	@echo "  make <test_filename>                    # Run all tests in a file"
	@echo "  make <test_filename>.<test_case>        # Run specific test case"
	@echo "  make list-all-tests                     # List all test cases"

.PHONY: list-all-tests
list-all-tests: build-debug-check
	@echo "All test cases in $(TEST_EXECUTABLE):"
	@echo "========================================"
	@$(TEST_EXEC_DEBUG) --gtest_list_tests

.PHONY: test-all
test-all: build-debug-check
	@echo "Running all tests..."
	@$(TEST_EXEC_DEBUG)

# 运行匹配模式的测试
test-pattern-%: build-debug-check
	@pattern="$*"; \
	echo "Running tests matching pattern: $$pattern"; \
	$(TEST_EXEC_DEBUG) --gtest_filter="*$$pattern*"

EOF

# # 添加 TEST_FILES 变量供辅助功能使用
# echo "" >> $OUTPUT_FILE
# echo "# 测试文件列表" >> $OUTPUT_FILE
# echo "TEST_FILES := $TEST_FILES" >> $OUTPUT_FILE
# 
# echo "Generated $OUTPUT_FILE"
# echo "Found test files:"
# echo "$TEST_FILES"
