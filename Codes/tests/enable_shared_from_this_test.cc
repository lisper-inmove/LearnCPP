#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>

/**
 * 如果一个对象已经被 shared_ptr 管理
 * 但是在它的成员函数中需要使用 this 来构造一个新的 shared_ptr
 * 会导致两个独立的 shared_ptr 管理同一个原始指针，造成重复 析构
 *
 * 使用方法:
 * 1. 类需要有公有继承 enable_shared_from_this<ClassName>
 * 2. 对象必须已经被 shared_ptr 管理（通过 std::make_shared 或 shared_ptr
 *      构造函数创建）
 * 3. 在成员函数中调用 shared_from_this() 来获取指向当前对象的 shared_ptr
 * */

namespace cvtest::tester {

TEST_F(Tester, EnableSharedFromThisTester) {

  class Bad {
  public:
    // 从this创建新的 shared_ptr，会与外部已有的 shared_ptr 独立
    void process() { std::shared_ptr<Bad> p(this); }
  };

  class Good : public std::enable_shared_from_this<Good> {
  public:
    void process() {
      std::shared_ptr<Good> self = shared_from_this();
      std::cout << "Sharet_ptr use_count: " << self.use_count() << "\n";
    }
  };
}
} // namespace cvtest::tester
