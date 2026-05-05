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

