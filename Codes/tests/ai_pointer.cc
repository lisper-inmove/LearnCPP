#include "tester.h"
#include "gtest/gtest.h"

/**
 * 智能指针
 * unique_ptr
 * shared_ptr
 * weak_ptr
 * make_unique
 * make_shared
 * make_unique_for_overwrite
 *
 * unique_ptr使用值初始化，基本类型初始化为0,对象被默认构造
 *   如果知道一定覆盖初始值，可以使用 make_unique_for_overwrite
 *   来跳过初始化。对于基本类型，处于未初始化的状态，对象仍然是默认构造的
 * */

namespace cvtest::tester {
TEST_F(Tester, AIPointerTester) {}
} // namespace cvtest::tester
