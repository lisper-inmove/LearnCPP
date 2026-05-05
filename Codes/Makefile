# ============================================
# 主 Makefile - 统一管理所有可执行文件和测试
# ============================================

.PHONY: all debug release clean help
.PHONY: run run-% test test-% list

# 默认构建类型（可以通过命令行覆盖）
BUILD_TYPE ?= Debug
BUILD_DIR = build-$(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')

# 获取所有可执行文件名称（从 makes/*.mk 中提取）
# 假设每个 .mk 文件定义了 EXECUTABLE 变量
-include makes/*.mk

# 收集所有可执行文件和测试
EXECUTABLES := $(sort $(foreach mk, $(wildcard makes/*.mk), \
                 $(if $(findstring test, $(mk)),, \
                   $(shell grep -E '^EXECUTABLE\s*:=' $(mk) 2>/dev/null | cut -d'=' -f2))))
TESTS := $(sort $(foreach mk, $(wildcard makes/*test*.mk), \
            $(shell grep -E '^TEST_NAME\s*:=' $(mk) 2>/dev/null | cut -d'=' -f2)))

# ============================================
# 主要目标
# ============================================

all: debug

# 配置项目
config-%:
	@cmake -B build-$* -DCMAKE_BUILD_TYPE=$* $(CMAKE_ARGS)

config-debug: config-debug
config-release: config-release

# 构建所有目标
build-%: config-%
	@cmake --build build-$*

debug: build-Debug
release: build-Release

# ============================================
# 运行单个可执行文件
# ============================================

# 通用运行规则（自动检测构建目录）
run-%: build-$(BUILD_TYPE)
	@EXEC="$(shell find build-$(BUILD_TYPE) -name '$*' -type f -executable 2>/dev/null | head -1)"; \
	if [ -n "$$EXEC" ] && [ -x "$$EXEC" ]; then \
		echo "Running: $$EXEC"; \
		$$EXEC; \
	else \
		echo "Error: Executable '$*' not found in build-$(BUILD_TYPE)"; \
		exit 1; \
	fi

# 智能运行（自动选择 Debug 或 Release）
smart-run-%:
	@for dir in build-Debug build-Release; do \
		EXEC="$$(find $$dir -name '$*' -type f -executable 2>/dev/null | head -1)"; \
		if [ -n "$$EXEC" ]; then \
			echo "Running: $$EXEC"; \
			$$EXEC; \
			exit 0; \
		fi; \
	done; \
	echo "Error: Executable '$*' not found in build-Debug or build-Release"; \
	exit 1

# ============================================
# 单元测试
# ============================================

# 运行所有测试
test: build-$(BUILD_TYPE)
	@cd build-$(BUILD_TYPE) && ctest --output-on-failure

# 运行特定测试（通过名称匹配）
test-%: build-$(BUILD_TYPE)
	@cd build-$(BUILD_TYPE) && ctest --output-on-failure -R '$*'

# 运行测试并显示详细输出
test-verbose: build-$(BUILD_TYPE)
	@cd build-$(BUILD_TYPE) && ctest --output-on-failure --verbose

# 运行测试（Debug 版本）
test-debug: build-Debug
	@cd build-Debug && ctest --output-on-failure

# 运行测试（Release 版本）
test-release: build-Release
	@cd build-Release && ctest --output-on-failure

# ============================================
# 清理
# ============================================

clean:
	@rm -rf build-Debug build-Release
	@echo "Cleaned build directories"

clean-debug:
	@rm -rf build-Debug
	@echo "Cleaned build-Debug"

clean-release:
	@rm -rf build-Release
	@echo "Cleaned build-Release"

# ============================================
# 重建（清理 + 构建 + 运行）
# ============================================

rebuild: clean debug

rebuild-run-%: clean
	@$(MAKE) run-$*

# ============================================
# 实用工具
# ============================================

# 列出所有可执行文件和测试
list:
	@echo "Available executables:"
	@$(foreach exec,$(EXECUTABLES),echo "  - $(exec)";)
	@echo ""
	@echo "Available tests:"
	@$(foreach test,$(TESTS),echo "  - $(test)";)
	@echo ""
	@echo "Usage examples:"
	@echo "  make run-app1              # Build and run app1 (Debug)"
	@echo "  make run-app2 BUILD_TYPE=Release  # Build and run app2 (Release)"
	@echo "  make smart-run-app1        # Run app1 (auto-select existing build)"
	@echo "  make test                  # Run all tests"
	@echo "  make test-mytest           # Run tests matching 'mytest'"
	@echo "  make test-verbose          # Run all tests with verbose output"

# ============================================
# 帮助信息
# ============================================

help:
	@echo "Available targets:"
	@echo "  make debug              - Configure and build Debug version"
	@echo "  make release            - Configure and build Release version"
	@echo "  make run-<name>         - Build and run specific executable"
	@echo "  make smart-run-<name>   - Run executable (auto-select build dir)"
	@echo "  make test               - Run all unit tests"
	@echo "  make test-<pattern>     - Run tests matching pattern"
	@echo "  make clean              - Remove build directories"
	@echo "  make rebuild            - Clean and rebuild Debug version"
	@echo "  make list               - List all available executables and tests"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_TYPE=[Debug|Release]  - Set build type (default: Debug)"
	@echo "  CMAKE_ARGS='...'            - Pass additional arguments to CMake"
