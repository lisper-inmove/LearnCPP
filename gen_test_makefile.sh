#!/bin/bash
# 自动从测试二进制中提取所有 gtest 用例名，生成 Makefile
# 用法: ./gen_test_makefile.sh [build_dir]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${1:-$SCRIPT_DIR/build-Debug}"
TEST_BIN="$BUILD_DIR/bin/LearnCPP_Test"
MAKEFILE="$SCRIPT_DIR/Makefile"

# ── 编译（始终重新编译以确保测试列表是最新的）──────────────────────────

echo "[gen] 正在编译..."
cmake --build "$BUILD_DIR" --target LearnCPP_Test -- -j"$(nproc)" || {
  echo "[gen] 编译失败"
  exit 1
}

# ── 从 gtest 提取用例名 ───────────────────────────────────────────────────

echo "[gen] 从 $TEST_BIN 提取测试列表..."

# --gtest_list_tests 输出格式:
#   SuiteName.
#     TestName1
#     TestName2
mapfile -t TEST_NAMES < <(
  "$TEST_BIN" --gtest_list_tests 2>/dev/null |
    grep -v '^$' |
    grep -v '^  #' |
    sed -n 's/^[[:space:]]\+//p'
)

if [[ ${#TEST_NAMES[@]} -eq 0 ]]; then
  echo "[gen] 未找到任何测试用例"
  exit 1
fi

echo "[gen] 找到 ${#TEST_NAMES[@]} 个测试用例"

# ── 生成 Makefile ──────────────────────────────────────────────────────────

cat >"$MAKEFILE" <<'HEADER'
# ============================================================================
#  自动生成的测试 Makefile
#  运行 ./gen_test_makefile.sh 重新生成
# ============================================================================

BUILD_DIR := build-Debug
TEST_BIN  := $(BUILD_DIR)/bin/LearnCPP_Test
SUITE     := Tester
NO_COLOR  :=

# 检测终端是否支持颜色
ifeq ($(shell test -t 1 && echo yes),yes)
  GREEN  := \033[0;32m
  RED    := \033[0;31m
  YELLOW := \033[0;33m
  RESET  := \033[0m
else
  GREEN  :=
  RED    :=
  YELLOW :=
  RESET  :=
endif

# ── list 目标 ──────────────────────────────────────────────────────────────

.PHONY: list
list:
	@echo "$(YELLOW)=== 可用测试目标 ($(words $(TESTS)) 个) ===$(RESET)"
	@$(foreach t,$(TESTS),echo "  make $(t)";)

# ── help ───────────────────────────────────────────────────────────────────

.PHONY: help
help:
	@echo "用法:"
	@echo "  make <测试名>     运行单个测试"
	@echo "  make list         列出所有测试"
	@echo "  make all          运行全部测试"
	@echo "  make build        编译测试二进制"
	@echo "  make regen        重新生成此 Makefile"

# ── build ──────────────────────────────────────────────────────────────────

.PHONY: build
build:
	@echo "[build] 编译 $(TEST_BIN)..."
	@cmake --build $(BUILD_DIR) --target LearnCPP_Test -- -j$$(nproc)

# ── all ────────────────────────────────────────────────────────────────────

.PHONY: all
all: build
	@echo "$(YELLOW)=== 运行全部测试 ===$(RESET)"
	@$(TEST_BIN)

# ── regen ──────────────────────────────────────────────────────────────────

.PHONY: regen
regen:
	@./gen_test_makefile.sh

# ── 单个测试模板 ───────────────────────────────────────────────────────────

define TEST_TEMPLATE
.PHONY: $(1)
$(1): build
	@printf "$(YELLOW)[$(1)]$(RESET) "
	@$(TEST_BIN) --gtest_filter="$(SUITE).$(1)" \
		&& printf "$(GREEN)PASS$(RESET)\n" \
		|| printf "$(RED)FAIL$(RESET)\n"
endef

TESTS :=

HEADER

# 追加每个测试的规则
for name in "${TEST_NAMES[@]}"; do
  cat >>"$MAKEFILE" <<EOF
TESTS += $name
\$(eval \$(call TEST_TEMPLATE,$name))

EOF
done

# 追加 list 目标（需要在 TESTS 变量定义之后）
cat >>"$MAKEFILE" <<'FOOTER'

# ============================================================================
FOOTER

echo "[gen] Makefile 已生成: $MAKEFILE"
echo "[gen] 可用命令: make list | make <test-name> | make all"
