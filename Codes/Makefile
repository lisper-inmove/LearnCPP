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

TESTS += TCG_TemplateBasicTester
$(eval $(call TEST_TEMPLATE,TCG_TemplateBasicTester))

TESTS += CIA_Concurrency_In_Action_CH2_Tester
$(eval $(call TEST_TEMPLATE,CIA_Concurrency_In_Action_CH2_Tester))

TESTS += CIA_StartThread1
$(eval $(call TEST_TEMPLATE,CIA_StartThread1))

TESTS += CIA_StartThread2
$(eval $(call TEST_TEMPLATE,CIA_StartThread2))

TESTS += CIA_StartThread3
$(eval $(call TEST_TEMPLATE,CIA_StartThread3))

TESTS += CIA_ThreadTerminate
$(eval $(call TEST_TEMPLATE,CIA_ThreadTerminate))

TESTS += CIA_Concurrency_In_Action_CH5_Tester
$(eval $(call TEST_TEMPLATE,CIA_Concurrency_In_Action_CH5_Tester))

TESTS += CIA_AtomicBasicTester
$(eval $(call TEST_TEMPLATE,CIA_AtomicBasicTester))

TESTS += CIA_MemoryOrderTester
$(eval $(call TEST_TEMPLATE,CIA_MemoryOrderTester))

TESTS += CIA_BoolTester
$(eval $(call TEST_TEMPLATE,CIA_BoolTester))

TESTS += CIA_AtomicPointerTester
$(eval $(call TEST_TEMPLATE,CIA_AtomicPointerTester))

TESTS += CIA_UserDefineAtomicTester
$(eval $(call TEST_TEMPLATE,CIA_UserDefineAtomicTester))

TESTS += CIA_SequentialConsistencyTester
$(eval $(call TEST_TEMPLATE,CIA_SequentialConsistencyTester))

TESTS += CIA_RelaxedOperationTester
$(eval $(call TEST_TEMPLATE,CIA_RelaxedOperationTester))

TESTS += CIA_RelaxedOperationMultipleThreadsTester
$(eval $(call TEST_TEMPLATE,CIA_RelaxedOperationMultipleThreadsTester))

TESTS += CIA_AcquireReleaseTester
$(eval $(call TEST_TEMPLATE,CIA_AcquireReleaseTester))

TESTS += CIA_AcquireReleaseImposeOrderingTester
$(eval $(call TEST_TEMPLATE,CIA_AcquireReleaseImposeOrderingTester))

TESTS += CIA_Concurrency_In_Action_CH3_Tester
$(eval $(call TEST_TEMPLATE,CIA_Concurrency_In_Action_CH3_Tester))

TESTS += CIA_IdentifyThreadTester
$(eval $(call TEST_TEMPLATE,CIA_IdentifyThreadTester))

TESTS += CIA_LockGuardTester
$(eval $(call TEST_TEMPLATE,CIA_LockGuardTester))

TESTS += CIA_SolveDeadlockTester
$(eval $(call TEST_TEMPLATE,CIA_SolveDeadlockTester))

TESTS += CIA_DoOnceTester
$(eval $(call TEST_TEMPLATE,CIA_DoOnceTester))

TESTS += CIA_HierarchicalMutexTester
$(eval $(call TEST_TEMPLATE,CIA_HierarchicalMutexTester))

TESTS += CIA_UniqueLockTester
$(eval $(call TEST_TEMPLATE,CIA_UniqueLockTester))

TESTS += CIA_Concurrency_In_Action_CH4_Tester
$(eval $(call TEST_TEMPLATE,CIA_Concurrency_In_Action_CH4_Tester))

TESTS += CIA_AsyncTester
$(eval $(call TEST_TEMPLATE,CIA_AsyncTester))

TESTS += CIA_AsyncStrategyTester
$(eval $(call TEST_TEMPLATE,CIA_AsyncStrategyTester))

TESTS += CIA_LoopWaitForFutureTester
$(eval $(call TEST_TEMPLATE,CIA_LoopWaitForFutureTester))

TESTS += CIA_RaiseExceptionTester
$(eval $(call TEST_TEMPLATE,CIA_RaiseExceptionTester))

TESTS += CIA_PackagedTaskTester
$(eval $(call TEST_TEMPLATE,CIA_PackagedTaskTester))

TESTS += CIA_PromiseTester
$(eval $(call TEST_TEMPLATE,CIA_PromiseTester))

TESTS += DIDP_AbstractFactoryTester
$(eval $(call TEST_TEMPLATE,DIDP_AbstractFactoryTester))

TESTS += DIDP_Builder_Tester
$(eval $(call TEST_TEMPLATE,DIDP_Builder_Tester))

TESTS += DIDP_FactoryMethodTester
$(eval $(call TEST_TEMPLATE,DIDP_FactoryMethodTester))

TESTS += AtomicTester
$(eval $(call TEST_TEMPLATE,AtomicTester))

TESTS += BarrierTester
$(eval $(call TEST_TEMPLATE,BarrierTester))

TESTS += CouroutineTester
$(eval $(call TEST_TEMPLATE,CouroutineTester))

TESTS += LatchTester
$(eval $(call TEST_TEMPLATE,LatchTester))

TESTS += LockTester
$(eval $(call TEST_TEMPLATE,LockTester))

TESTS += CounterValue
$(eval $(call TEST_TEMPLATE,CounterValue))

TESTS += CounterValueWithMutex
$(eval $(call TEST_TEMPLATE,CounterValueWithMutex))

TESTS += LockManagementTester
$(eval $(call TEST_TEMPLATE,LockManagementTester))

TESTS += ConditionVariableTester
$(eval $(call TEST_TEMPLATE,ConditionVariableTester))

TESTS += PFTester
$(eval $(call TEST_TEMPLATE,PFTester))

TESTS += PFWithTimeoutTester
$(eval $(call TEST_TEMPLATE,PFWithTimeoutTester))

TESTS += PFPackagedTaskTester
$(eval $(call TEST_TEMPLATE,PFPackagedTaskTester))

TESTS += PFAsyncTester
$(eval $(call TEST_TEMPLATE,PFAsyncTester))

TESTS += PFSharedFutureTester
$(eval $(call TEST_TEMPLATE,PFSharedFutureTester))

TESTS += SemaphoreTester
$(eval $(call TEST_TEMPLATE,SemaphoreTester))

TESTS += AsyncTester
$(eval $(call TEST_TEMPLATE,AsyncTester))

TESTS += ThreadCreateTester
$(eval $(call TEST_TEMPLATE,ThreadCreateTester))

TESTS += ThreadSleepTester
$(eval $(call TEST_TEMPLATE,ThreadSleepTester))

TESTS += ThreadParameters
$(eval $(call TEST_TEMPLATE,ThreadParameters))

TESTS += DaemonThreadTester
$(eval $(call TEST_TEMPLATE,DaemonThreadTester))

TESTS += JThreadTester
$(eval $(call TEST_TEMPLATE,JThreadTester))

TESTS += YieldTester
$(eval $(call TEST_TEMPLATE,YieldTester))

TESTS += CancelThreadTester
$(eval $(call TEST_TEMPLATE,CancelThreadTester))

TESTS += CancelThreadTesterStopToken
$(eval $(call TEST_TEMPLATE,CancelThreadTesterStopToken))

TESTS += CancelThreadTesterStopSource
$(eval $(call TEST_TEMPLATE,CancelThreadTesterStopSource))

TESTS += CancelThreadTesterCallback
$(eval $(call TEST_TEMPLATE,CancelThreadTesterCallback))

TESTS += ThreadLocalTester
$(eval $(call TEST_TEMPLATE,ThreadLocalTester))

TESTS += ClassTest
$(eval $(call TEST_TEMPLATE,ClassTest))

TESTS += TemplateTester
$(eval $(call TEST_TEMPLATE,TemplateTester))

TESTS += TemplateBaseTester
$(eval $(call TEST_TEMPLATE,TemplateBaseTester))

TESTS += NonTypeTemplateTester
$(eval $(call TEST_TEMPLATE,NonTypeTemplateTester))

TESTS += TemplateTemplateTester
$(eval $(call TEST_TEMPLATE,TemplateTemplateTester))


# ============================================================================
